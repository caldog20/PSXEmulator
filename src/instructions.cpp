#include "cpu.hpp"
#include "emulator.hpp"

using Helpers::log;
using Helpers::panic;

void Cpu::ExceptionHandler(Exception cause) {
    u32 sr = m_regs.copr.sr;
    std::bitset<32> b_sr{sr};

    u32 handler_address = (b_sr[22]) ? 0xbfc00180 : 0x80000080;

    u32 mode = sr & 0x3f;
    sr &= ~0x3f;
    sr |= (mode << 2) & 0x3f;
    m_regs.copr.sr = sr;

    m_regs.copr.cause = static_cast<u32>(cause) << 2;
    m_regs.copr.epc = m_regs.backup_pc;

    if (m_inBranchDelaySlot) {
        m_regs.copr.epc -= 4;
        m_regs.copr.cause |= 1 << 31;
    }

    m_regs.pc = handler_address;
    m_branching = true;
}

void Cpu::RFE() {
    if ((m_instruction.ins & 0x3f) != 0x10) {
        m_emulator.log("RFE: Unmatched cop0 instruction\n");
        return;
    }
    u32 mode = m_regs.copr.sr & 0x3f;
    m_regs.copr.sr &= ~0x3f;
    m_regs.copr.sr |= mode >> 2;
}

void Cpu::Special() {
    const auto f = special[m_instruction.fn];
    (this->*f)();
}

void Cpu::SYSCALL() { ExceptionHandler(Exception::Syscall); }

void Cpu::NOP() { log("NOP\n"); }

// LOAD STORE INSTRUCTIONS

void Cpu::LUI() { m_regs.set(m_instruction.rt, m_instruction.immlui); }

void Cpu::LB() {
    checkPendingLoad();
    m_regs.ld_target = m_instruction.rt;
    u32 addr = m_regs.get(m_instruction.rs) + m_instruction.immse;
    m_regs.ld_value = static_cast<s8>(m_emulator.m_mem.read8(addr));
    m_loadDelay = true;
}

void Cpu::LBU() {
    checkPendingLoad();
    m_regs.ld_target = m_instruction.rt;
    u32 addr = m_regs.get(m_instruction.rs) + m_instruction.immse;
    m_regs.ld_value = m_emulator.m_mem.read8(addr);
    m_loadDelay = true;
}

void Cpu::LH() {
    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    if (address % 2 != 0) {
        ExceptionHandler(Exception::BadLoadAddress);
        return;
    }
    checkPendingLoad();
    m_regs.ld_target = m_instruction.rt;

    m_regs.ld_value = static_cast<s16>(m_emulator.m_mem.read16(address));
    m_loadDelay = true;
}

void Cpu::LW() {
    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    if (address % 4 != 0) {
        ExceptionHandler(Exception::BadLoadAddress);
        return;
    }
    checkPendingLoad();
    m_regs.ld_target = m_instruction.rt;

    m_regs.ld_value = m_emulator.m_mem.read32(address);
    m_loadDelay = true;
}

void Cpu::SB() {
    if ((m_regs.copr.sr & 0x10000) != 0) {
        m_emulator.log("Cache Isolated, ignoring write\n");
        return;
    }

    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    u8 value = m_regs.get(m_instruction.rt);
    m_emulator.m_mem.write8(address, value);
}

void Cpu::SH() {
    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    if (address % 2 != 0) {
        ExceptionHandler(Exception::BadStoreAddress);
        return;
    }

    if ((m_regs.copr.sr & 0x10000) != 0) {
        m_emulator.log("Cache Isolated, ignoring write\n");
        return;
    }

    u16 value = (u16)m_regs.get(m_instruction.rt);
    m_emulator.m_mem.write16(address, value);
}

void Cpu::SW() {
    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    if (address % 4 != 0) {
        ExceptionHandler(Exception::BadStoreAddress);
        return;
    }

    if ((m_regs.copr.sr & 0x10000) != 0) {
        m_emulator.log("Cache Isolated, ignoring write\n");
        return;
    }

    u32 value = m_regs.get(m_instruction.rt);
    m_emulator.m_mem.write32(address, value);
}

// ALU

// TODO: overflow for ADDI
void Cpu::ADD() {
    s32 rs = m_regs.get(m_instruction.rs);
    s32 rt = m_regs.get(m_instruction.rt);
    u32 value = rs + rt;

    bool overflow = ((rs ^ value) & (rt ^ value)) >> 31;
    if (overflow) {
        ExceptionHandler(Exception::Overflow);
        return;
    }

    if (m_instruction.rd != 0) {
        m_regs.set(m_instruction.rd, value);
    }
}

void Cpu::ADDI() {
    s32 rs = m_regs.get(m_instruction.rs);
    s32 imm = m_instruction.immse;
    u32 value = rs + imm;

    bool overflow = ((rs ^ value) & (imm ^ value)) >> 31;
    if (overflow) {
        ExceptionHandler(Exception::Overflow);
        return;
    }

    if (m_instruction.rt != 0) {
        m_regs.set(m_instruction.rt, value);
    }
}

void Cpu::ADDIU() {
    if (!m_instruction.rt) return;
    u32 rs = m_regs.get(m_instruction.rs);
    u32 imm = m_instruction.immse;
    u32 value = rs + imm;
    m_regs.set(m_instruction.rt, value);
}

void Cpu::ADDU() {
    if (!m_instruction.rd) return;
    u32 rs = m_regs.get(m_instruction.rs);
    u32 rt = m_regs.get(m_instruction.rt);
    u32 value = rs + rt;
    m_regs.set(m_instruction.rd, value);
}

void Cpu::SUBU() {
    if (!m_instruction.rd) return;

    u32 value = m_regs.get(m_instruction.rs) - m_regs.get(m_instruction.rt);
    m_regs.set(m_instruction.rd, value);
}

void Cpu::OR() {
    u32 value = m_regs.get(m_instruction.rs) | m_regs.get(m_instruction.rt);
    m_regs.set(m_instruction.rd, value);
}

void Cpu::ORI() {
    u32 value = m_regs.get(m_instruction.rs) | m_instruction.imm;
    m_regs.set(m_instruction.rt, value);
}

void Cpu::SLL() {
    if (!m_instruction.rd) return;

    m_regs.set(m_instruction.rd, m_regs.get(m_instruction.rt) << m_instruction.sa);
}

void Cpu::SRA() {
    if (!m_instruction.rd) return;
    s32 rt = m_regs.get(m_instruction.rt);
    u32 value = rt >> m_instruction.sa;
    m_regs.set(m_instruction.rd, value);
}

void Cpu::SRL() {
    if (!m_instruction.rd) return;
    u32 value = m_regs.get(m_instruction.rt) >> m_instruction.sa;
    m_regs.set(m_instruction.rd, value);
}

void Cpu::AND() {
    u32 value = m_regs.get(m_instruction.rs) & m_regs.get(m_instruction.rt);
    m_regs.set(m_instruction.rd, value);
}

void Cpu::ANDI() {
    u32 value = m_regs.get(m_instruction.rs) & m_instruction.imm;
    m_regs.set(m_instruction.rt, value);
}

// divu $s, $t | lo = $s / $t; hi = $s % $t
// dividend ÷ divisor = quotient
void Cpu::DIV() {
    s32 dividend = m_regs.get(m_instruction.rs);
    s32 divisor = m_regs.get(m_instruction.rt);

    if (divisor == 0) {
        m_regs.spr.hi = (u32)dividend;

        if (dividend >= 0) {
            m_regs.spr.lo = 0xffffffff;
        } else {
            m_regs.spr.lo = 1;
        }
        return;
    }

    if ((u32)dividend == 0x80000000 && divisor == -1) {
        m_regs.spr.hi = 0;
        m_regs.spr.lo = 0x80000000;
        return;
    }

    m_regs.spr.hi = static_cast<u32>(dividend % divisor);
    m_regs.spr.lo = static_cast<u32>(dividend / divisor);
}

void Cpu::DIVU() {
    u32 dividend = m_regs.get(m_instruction.rs);
    u32 divisor = m_regs.get(m_instruction.rt);

    if (divisor == 0) {
        m_regs.spr.hi = dividend;
        m_regs.spr.lo = 0xffffffff;
        return;
    }

    m_regs.spr.hi = dividend % divisor;
    m_regs.spr.lo = dividend / divisor;
}

void Cpu::MFLO() {
    if (!m_instruction.rd) return;
    m_regs.set(m_instruction.rd, m_regs.spr.lo);
}

void Cpu::MFHI() {
    if (!m_instruction.rd) return;
    m_regs.set(m_instruction.rd, m_regs.spr.hi);
}

void Cpu::MTLO() { m_regs.spr.lo = m_regs.get(m_instruction.rs); }

void Cpu::MTHI() { m_regs.spr.hi = m_regs.get(m_instruction.rs); }

// CONDITONAL/BRANCH

void Cpu::J() {
    m_branchDelay = true;
    m_regs.jumppc = (m_regs.pc & 0xf0000000) | (m_instruction.tar << 2);
}

void Cpu::JAL() {
    m_branchDelay = true;
    m_regs.jumppc = (m_regs.pc & 0xf0000000) | (m_instruction.tar << 2);
    m_regs.linkpc = m_regs.pc + 8;
}

void Cpu::JR() {
    m_branchDelay = true;
    m_regs.jumppc = m_regs.get(m_instruction.rs);
}

void Cpu::JALR() {
    m_branchDelay = true;
    m_regs.jumppc = m_regs.get(m_instruction.rs);
    m_regs.set(m_instruction.rd, m_regs.pc + 8);
}

void Cpu::BEQ() {
    u32 rs = m_regs.get(m_instruction.rs);
    u32 rt = m_regs.get(m_instruction.rt);

    if (rs == rt) {
        m_branchDelay = true;
        m_regs.jumppc = m_instruction.immse * 4 + m_regs.pc + 4;
    }
}

void Cpu::BNE() {
    u32 rs = m_regs.get(m_instruction.rs);
    u32 rt = m_regs.get(m_instruction.rt);

    if (rs != rt) {
        m_branchDelay = true;
        m_regs.jumppc = m_instruction.immse * 4 + m_regs.pc + 4;
    }
}

void Cpu::BGTZ() {
    s32 rs = m_regs.get(m_instruction.rs);
    if (rs > 0) {
        m_branchDelay = true;
        m_regs.jumppc = m_instruction.immse * 4 + m_regs.pc + 4;
    }
}

void Cpu::BLEZ() {
    s32 rs = m_regs.get(m_instruction.rs);
    if (rs <= 0) {
        m_branchDelay = true;
        m_regs.jumppc = m_instruction.immse * 4 + m_regs.pc + 4;
    }
}

void Cpu::Branch(bool link) {
    m_branchDelay = true;
    m_regs.jumppc = m_instruction.immse * 4 + m_regs.pc + 4;
    if (link) {
        m_regs.set(31, m_regs.pc + 8);
    }
}

void Cpu::SLTU() {
    if (!m_instruction.rd) return;

    u32 value = m_regs.get(m_instruction.rs) < m_regs.get(m_instruction.rt);

    m_regs.set(m_instruction.rd, value);
}

void Cpu::SLTI() {
    if (!m_instruction.rt) return;

    s32 rs = m_regs.get(m_instruction.rs);
    s32 imm = m_instruction.immse;
    u32 value = rs < imm;
    m_regs.set(m_instruction.rt, value);
}

void Cpu::SLTIU() {
    if (!m_instruction.rt) return;

    u32 rs = m_regs.get(m_instruction.rs);
    u32 value = rs < m_instruction.immse;
    m_regs.set(m_instruction.rt, value);
}

void Cpu::SLT() {
    if (!m_instruction.rd) return;
    s32 rs = m_regs.get(m_instruction.rs);
    s32 rt = m_regs.get(m_instruction.rt);

    s32 value = rs < rt;

    m_regs.set(m_instruction.rd, static_cast<u32>(value));
}

void Cpu::REGIMM() {
    s32 rs = m_regs.get(m_instruction.rs);
    if (m_instruction.bgez) {
        if (rs >= 0) {
            Branch(m_instruction.b_link);
        }
    } else {
        if (rs < 0) {
            Branch(m_instruction.b_link);
        }
    }
}

// COP

void Cpu::COP0() {
    switch (m_instruction.rs) {
        case 0:
            MFC0();
            break;
        case 4:
            MTC0();
            break;
        case 16:
            RFE();
            break;
        default:
            m_emulator.log("Unimplemented COP0 instruction {:#x}\n", m_instruction.opcode);
    }
}

void Cpu::MTC0() {
    u32 val = m_regs.get(m_instruction.rt);
    m_regs.setcopr(m_instruction.rd, val);
}

void Cpu::MFC0() {
    checkPendingLoad();
    u32 val = m_regs.getcopr(m_instruction.rd);
    m_regs.ld_target = m_instruction.rt;
    m_regs.ld_value = val;
    m_loadDelay = true;
}

// Unimplemented Instructions

void Cpu::Unknown() { panic("Unknown instruction at {:#x}, opcode {:#x}\n", m_regs.pc, m_instruction.ins); }

void Cpu::BREAK() { panic("[Unimplemented] BREAK instruction\n"); }
void Cpu::CFC2() { panic("[Unimplemented] CFC2 instruction\n"); }
void Cpu::COP2() { panic("[Unimplemented] COP2 instruction\n"); }
void Cpu::CTC2() { panic("[Unimplemented] CTC2 instruction\n"); }
void Cpu::LHU() { panic("[Unimplemented] LHU instruction\n"); }
void Cpu::LWC2() { panic("[Unimplemented] LWC2 instruction\n"); }
void Cpu::LWL() { panic("[Unimplemented] LWL instruction\n"); }
void Cpu::LWR() { panic("[Unimplemented] LWR instruction\n"); }
void Cpu::MFC2() { panic("[Unimplemented] MFC2 instruction\n"); }
void Cpu::MTC2() { panic("[Unimplemented] MTC2 instruction\n"); }
void Cpu::MULT() { panic("[Unimplemented] MULT instruction\n"); }
void Cpu::MULTU() { panic("[Unimplemented] MULTU instruction\n"); }
void Cpu::NOR() { panic("[Unimplemented] NOR instruction\n"); }
void Cpu::SLLV() { panic("[Unimplemented] SLLV instruction\n"); }
void Cpu::SRAV() { panic("[Unimplemented] SRAV instruction\n"); }
void Cpu::SRLV() { panic("[Unimplemented] SRLV instruction\n"); }
void Cpu::SUB() { panic("[Unimplemented] SUB instruction\n"); }
void Cpu::SWC2() { panic("[Unimplemented] SWC2 instruction\n"); }
void Cpu::SWL() { panic("[Unimplemented] SWL instruction\n"); }
void Cpu::SWR() { panic("[Unimplemented] SWR instruction\n"); }

void Cpu::XOR() { panic("[Unimplemented] XOR instruction\n"); }
void Cpu::XORI() { panic("[Unimplemented] XORI instruction\n"); }
