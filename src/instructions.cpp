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
    m_regs.ld_target = m_instruction.rt;
    u32 addr = m_regs.get(m_instruction.rs) + m_instruction.immse;
    m_regs.ld_value = (s8) m_emulator.m_mem.read8(addr);
    m_loadDelay = true;
}

void Cpu::LH() {
    m_regs.ld_target = m_instruction.rt;
    u32 addr = m_regs.get(m_instruction.rs) + m_instruction.immse;
    m_regs.ld_value = (s16)m_emulator.m_mem.read16(addr);
    m_loadDelay = true;
}

void Cpu::LW() {

    m_regs.ld_target = m_instruction.rt;

    u32 addr = m_regs.get(m_instruction.rs) + m_instruction.immse;

    m_regs.ld_value = m_emulator.m_mem.read32(addr);
    m_loadDelay = true;
}

void Cpu::SB() {
    if ((m_regs.copr.sr & 0x10000) != 0) {
        return;
    }

    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    u8 value = m_regs.get(m_instruction.rt);
    m_emulator.m_mem.write8(address, value);
}

void Cpu::SH() {
    u32 address = m_regs.get(m_instruction.rs) + m_instruction.immse;
    u16 value = (u16)m_regs.get(m_instruction.rt);
    m_emulator.m_mem.write16(address, value);
}

void Cpu::SW() {
    if ((m_regs.copr.sr & 0x10000) != 0) {
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
    u32 imm = m_instruction.immse;
    u32 value = rs + imm;
    m_regs.set(m_instruction.rt, value);
}

void Cpu::ADDIU() {
    u32 rs = m_regs.get(m_instruction.rs);
    u32 imm = m_instruction.immse;
    u32 value = rs + imm;
    m_regs.set(m_instruction.rt, value);
}

void Cpu::ADDU() {
    u32 rs = m_regs.get(m_instruction.rs);
    u32 rt = m_regs.get(m_instruction.rt);
    u32 value = rs + rt;
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

void Cpu::AND() {
    u32 value = m_regs.get(m_instruction.rs) & m_regs.get(m_instruction.rt);
    m_regs.set(m_instruction.rd, value);
}

void Cpu::ANDI() {
    u32 value = m_regs.get(m_instruction.rs) & m_instruction.imm;
    m_regs.set(m_instruction.rt, value);
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

void Cpu::BEQ() {
    u32 rs = m_regs.get(m_instruction.rs);
    u32 rt = m_regs.get(m_instruction.rt);

    if (rs != rt) return;

    m_branchDelay = true;
    m_regs.jumppc = m_instruction.immse * 4 + m_regs.pc + 4;
}

void Cpu::BNE() {
    u32 rs = m_regs.get(m_instruction.rs);
    u32 rt = m_regs.get(m_instruction.rt);

    if (rs == rt) return;

    m_branchDelay = true;
    m_regs.jumppc = m_instruction.immse * 4 + m_regs.pc + 4;
}

void Cpu::SLTU() {
    if (!m_instruction.rd) return;

    u32 value = m_regs.get(m_instruction.rs) < m_regs.get(m_instruction.rt);

    m_regs.set(m_instruction.rd, value);
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
void Cpu::REGIMM() { panic("[Unimplemented] RegImm instruction\n"); }
void Cpu::BGTZ() { panic("[Unimplemented] BGTZ instruction\n"); }
void Cpu::BLEZ() { panic("[Unimplemented] BLEZ instruction\n"); }
void Cpu::BREAK() { panic("[Unimplemented] BREAK instruction\n"); }
void Cpu::CFC2() { panic("[Unimplemented] CFC2 instruction\n"); }
void Cpu::COP2() { panic("[Unimplemented] COP2 instruction\n"); }
void Cpu::CTC2() { panic("[Unimplemented] CTC2 instruction\n"); }
void Cpu::DIV() { panic("[Unimplemented] DIV instruction\n"); }
void Cpu::DIVU() { panic("[Unimplemented] DIVU instruction\n"); }
void Cpu::JALR() { panic("[Unimplemented] JALR instruction\n"); }
void Cpu::LBU() { panic("[Unimplemented] LBU instruction\n"); }
void Cpu::LHU() { panic("[Unimplemented] LHU instruction\n"); }
void Cpu::LWC2() { panic("[Unimplemented] LWC2 instruction\n"); }
void Cpu::LWL() { panic("[Unimplemented] LWL instruction\n"); }
void Cpu::LWR() { panic("[Unimplemented] LWR instruction\n"); }
void Cpu::MFC2() { panic("[Unimplemented] MFC2 instruction\n"); }
void Cpu::MFHI() { panic("[Unimplemented] MFHI instruction\n"); }
void Cpu::MFLO() { panic("[Unimplemented] MFLO instruction\n"); }
void Cpu::MTC2() { panic("[Unimplemented] MTC2 instruction\n"); }
void Cpu::MTHI() { panic("[Unimplemented] MTHI instruction\n"); }
void Cpu::MTLO() { panic("[Unimplemented] MTLO instruction\n"); }
void Cpu::MULT() { panic("[Unimplemented] MULT instruction\n"); }
void Cpu::MULTU() { panic("[Unimplemented] MULTU instruction\n"); }
void Cpu::NOR() { panic("[Unimplemented] NOR instruction\n"); }
void Cpu::RFE() { panic("[Unimplemented] RFE instruction\n"); }
void Cpu::SLLV() { panic("[Unimplemented] SLLV instruction\n"); }
void Cpu::SLT() { panic("[Unimplemented] SLT instruction\n"); }
void Cpu::SLTI() { panic("[Unimplemented] SLTI instruction\n"); }
void Cpu::SLTIU() { panic("[Unimplemented] SLTIU instruction\n"); }
void Cpu::SRA() { panic("[Unimplemented] SRA instruction\n"); }
void Cpu::SRAV() { panic("[Unimplemented] SRAV instruction\n"); }
void Cpu::SRL() { panic("[Unimplemented] SRL instruction\n"); }
void Cpu::SRLV() { panic("[Unimplemented] SRLV instruction\n"); }
void Cpu::SUB() { panic("[Unimplemented] SUB instruction\n"); }
void Cpu::SUBU() { panic("[Unimplemented] SUBU instruction\n"); }
void Cpu::SWC2() { panic("[Unimplemented] SWC2 instruction\n"); }
void Cpu::SWL() { panic("[Unimplemented] SWL instruction\n"); }
void Cpu::SWR() { panic("[Unimplemented] SWR instruction\n"); }
void Cpu::SYSCALL() { panic("[Unimplemented] SYSCALL instruction\n"); }
void Cpu::XOR() { panic("[Unimplemented] XOR instruction\n"); }
void Cpu::XORI() { panic("[Unimplemented] XORI instruction\n"); }
