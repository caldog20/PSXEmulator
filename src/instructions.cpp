#include "cpu.hpp"

using Helpers::log;
using Helpers::panic;

void Cpu::NOP() { log("NOP\n"); }

void Cpu::LUI() { m_regs.set(m_instruction.rt, m_instruction.immlui); }

void Cpu::ORI() {
    u32 value = m_regs.get(m_instruction.rs) | m_instruction.imm;
    m_regs.set(m_instruction.rt, value);
}

// Unimplemented Instructions

void Cpu::Unknown() { panic("Unknown instruction"); }
void Cpu::REGIMM() { panic("[Unimplemented] RegImm instruction\n"); }
void Cpu::J() { panic("[Unimplemented] J instruction\n"); }
void Cpu::JAL() { panic("[Unimplemented] Jal instruction\n"); }
void Cpu::BEQ() { panic("[Unimplemented] Beq instruction\n"); }
void Cpu::SW() { panic("[Unimplemented] SW instruction\n"); }
void Cpu::Special() { panic("[Unimplemented] SPECIAL instruction\n"); }
void Cpu::SLL() { panic("[Unimplemented] SLL instruction\n"); }
void Cpu::ADDIU() { panic("[Unimplemented] ADDIU instruction\n"); }
void Cpu::OR() { panic("[Unimplemented] OR instruction\n"); }
void Cpu::ADD() { panic("[Unimplemented] ADD instruction\n"); }
void Cpu::ADDU() { panic("[Unimplemented] ADDU instruction\n"); }
void Cpu::AND() { panic("[Unimplemented] AND instruction\n"); }
void Cpu::ANDI() { panic("[Unimplemented] ANDI instruction\n"); }
void Cpu::BGTZ() { panic("[Unimplemented] BGTZ instruction\n"); }
void Cpu::BLEZ() { panic("[Unimplemented] BLEZ instruction\n"); }
// TODO: Implement overflow exceptions for ADDI
void Cpu::ADDI() { ADDIU(); }
void Cpu::BNE() { panic("[Unimplemented] BNE instruction\n"); }
void Cpu::BREAK() { panic("[Unimplemented] BREAK instruction\n"); }
void Cpu::CFC2() { panic("[Unimplemented] CFC2 instruction\n"); }
void Cpu::COP0() { panic("[Unimplemented] COP0 instruction\n"); }
void Cpu::COP2() { panic("[Unimplemented] COP2 instruction\n"); }
void Cpu::CTC2() { panic("[Unimplemented] CTC2 instruction\n"); }
void Cpu::DIV() { panic("[Unimplemented] DIV instruction\n"); }
void Cpu::DIVU() { panic("[Unimplemented] DIVU instruction\n"); }
void Cpu::JALR() { panic("[Unimplemented] JALR instruction\n"); }
void Cpu::JR() { panic("[Unimplemented] JR instruction\n"); }
void Cpu::LB() { panic("[Unimplemented] LB instruction\n"); }
void Cpu::LBU() { panic("[Unimplemented] LBU instruction\n"); }
void Cpu::LH() { panic("[Unimplemented] LH instruction\n"); }
void Cpu::LHU() { panic("[Unimplemented] LHU instruction\n"); }
void Cpu::LW() { panic("[Unimplemented] LW instruction\n"); }
void Cpu::LWC2() { panic("[Unimplemented] LWC2 instruction\n"); }
void Cpu::LWL() { panic("[Unimplemented] LWL instruction\n"); }
void Cpu::LWR() { panic("[Unimplemented] LWR instruction\n"); }
void Cpu::MFC0() { panic("[Unimplemented] MFC0 instruction\n"); }
void Cpu::MFC2() { panic("[Unimplemented] MFC2 instruction\n"); }
void Cpu::MFHI() { panic("[Unimplemented] MFHI instruction\n"); }
void Cpu::MFLO() { panic("[Unimplemented] MFLO instruction\n"); }
void Cpu::MTC0() { panic("[Unimplemented] MTC0 instruction\n"); }
void Cpu::MTC2() { panic("[Unimplemented] MTC2 instruction\n"); }
void Cpu::MTHI() { panic("[Unimplemented] MTHI instruction\n"); }
void Cpu::MTLO() { panic("[Unimplemented] MTLO instruction\n"); }
void Cpu::MULT() { panic("[Unimplemented] MULT instruction\n"); }
void Cpu::MULTU() { panic("[Unimplemented] MULTU instruction\n"); }
void Cpu::NOR() { panic("[Unimplemented] NOR instruction\n"); }
void Cpu::RFE() { panic("[Unimplemented] RFE instruction\n"); }
void Cpu::SB() { panic("[Unimplemented] SB instruction\n"); }
void Cpu::SH() { panic("[Unimplemented] SH instruction\n"); }
void Cpu::SLLV() { panic("[Unimplemented] SLLV instruction\n"); }
void Cpu::SLT() { panic("[Unimplemented] SLT instruction\n"); }
void Cpu::SLTI() { panic("[Unimplemented] SLTI instruction\n"); }
void Cpu::SLTIU() { panic("[Unimplemented] SLTIU instruction\n"); }
void Cpu::SLTU() { panic("[Unimplemented] SLTU instruction\n"); }
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
