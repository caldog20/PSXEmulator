#include "cpu.hpp"
#include "emulator.hpp"

using Helpers::log;
using Helpers::panic;

void Cpu::Special() {
    const auto f = special[m_instruction.fn];
    (this->*f)();
}

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
    checkPendingLoad();
    m_regs.ld_target = m_instruction.rt;
    u32 addr = m_regs.get(m_instruction.rs) + m_instruction.immse;
    m_regs.ld_value = static_cast<s16>(m_emulator.m_mem.read16(addr));
    m_loadDelay = true;
}

void Cpu::LW() {
    checkPendingLoad();
    m_regs.ld_target = m_instruction.rt;

    u32 addr = m_regs.get(m_instruction.rs) + m_instruction.immse;
    m_regs.ld_value = m_emulator.m_mem.read32(addr);
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
    if ((m_regs.copr.sr & 0x10000) != 0) {
        m_emulator.log("Cache Isolated, ignoring write\n");
        return;
    }

    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    u16 value = (u16)m_regs.get(m_instruction.rt);
    m_emulator.m_mem.write16(address, value);
}

void Cpu::SW() {
    if ((m_regs.copr.sr & 0x10000) != 0) {
        m_emulator.log("Cache Isolated, ignoring write\n");
        return;
    }

    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    u32 value = m_regs.get(m_instruction.rt);
    m_emulator.m_mem.write32(address, value);
}

// ALU

// TODO: overflow for ADDI
void Cpu::ADD() { ADDU(); }

void Cpu::ADDI() {
    u32 rs = m_regs.get(m_instruction.rs);
    s32 imm = m_instruction.immse;
    u32 value = rs + imm;
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
            m_emulator.log("MFC0\n");
            MFC0();
            break;
        case 4:
            m_emulator.log("MTC0\n");
            MTC0();
            break;
        case 16:
            m_emulator.log("RFE\n");
            RFE();
            break;
        default:
            m_emulator.log("Unimplemented COP0 instruction {:#x}\n", m_instruction.opcode);
    }
}

void Cpu::MTC0() {
    switch (m_instruction.rd) {
        case 12: {
            u32 value = m_regs.gpr.r[m_instruction.rt];
            m_regs.copr.sr = value;
            break;
        }
        default:
            m_emulator.log("MTC0 unmatched cop0 register\n");
    }
}

void Cpu::MFC0() {
    if (!m_instruction.rt) return;
    int val = m_regs.getcopr(m_instruction.rd);
    m_regs.ld_target = m_instruction.rt;
    m_regs.ld_value = val;
    m_loadDelay = true;
}

// Unimplemented Instructions

void Cpu::Unknown() { panic("Unknown instruction"); }

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
void Cpu::MTHI() { panic("[Unimplemented] MTHI instruction\n"); }
void Cpu::MTLO() { panic("[Unimplemented] MTLO instruction\n"); }
void Cpu::MULT() { panic("[Unimplemented] MULT instruction\n"); }
void Cpu::MULTU() { panic("[Unimplemented] MULTU instruction\n"); }
void Cpu::NOR() { panic("[Unimplemented] NOR instruction\n"); }
void Cpu::RFE() { panic("[Unimplemented] RFE instruction\n"); }
void Cpu::SLLV() { panic("[Unimplemented] SLLV instruction\n"); }
void Cpu::SRAV() { panic("[Unimplemented] SRAV instruction\n"); }
void Cpu::SRLV() { panic("[Unimplemented] SRLV instruction\n"); }
void Cpu::SUB() { panic("[Unimplemented] SUB instruction\n"); }
void Cpu::SWC2() { panic("[Unimplemented] SWC2 instruction\n"); }
void Cpu::SWL() { panic("[Unimplemented] SWL instruction\n"); }
void Cpu::SWR() { panic("[Unimplemented] SWR instruction\n"); }
void Cpu::SYSCALL() { panic("[Unimplemented] SYSCALL instruction\n"); }
void Cpu::XOR() { panic("[Unimplemented] XOR instruction\n"); }
void Cpu::XORI() { panic("[Unimplemented] XORI instruction\n"); }
