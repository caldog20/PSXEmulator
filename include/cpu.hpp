//
// Created by Caleb Yates on 1/4/23.
//

#pragma once
#include <map>

#include "utils.hpp"

class Emulator;

typedef union {
    struct {
        u32 zero, at, v0, v1, a0, a1, a2, a3;
        u32 t0, t1, t2, t3, t4, t5, t6, t7;
        u32 s0, s1, s2, s3, s4, s5, s6, s7;
        u32 t8, t9, k0, k1, gp, sp, fp, ra;
    };
    u32 r[32];
} gpr_t;

typedef union {
    struct {
        u32 hi;
        u32 lo;
    };
    u32 r[2];
} spr_t;

struct Regs {
    gpr_t gpt;
    spr_t spr;
    u32 pc;
    u32 next_pc;
    u32 ld_target;
    u32 ld_value;
    u32 instruction;

    void handlePendingLoad();
};

class Cpu {
  public:
    using opfn = void (Cpu::*)();

    Cpu(Emulator& emulator) : m_emulator(emulator) { init(); }
    void init();
    void step();

  private:
    Emulator& m_emulator;
    Regs m_regs;
    bool m_loadDelay = false;
    bool m_branchDelay = false;

    void Unknown();
    void Special();
    void REGIMM();
    void J();
    void JAL();
    void BEQ();
    void NOP();
    void ORI();
    void SW();
    void LUI();
    void SLL();
    void ADDIU();
    void ADDI();

    void ADD();
    void ADDU();
    void AND();
    void ANDI();
    void BGTZ();
    void BLEZ();
    void BNE();
    void BREAK();
    void CFC2();
    void COP0();
    void COP2();
    void CTC2();
    void DIV();
    void DIVU();
    void JALR();
    void JR();
    void LB();
    void LBU();
    void LH();
    void LHU();
    void LW();
    void LWC2();
    void LWL();
    void LWR();
    void MFC0();
    void MFC2();
    void MFHI();
    void MFLO();
    void MTC0();
    void MTC2();
    void MTHI();
    void MTLO();
    void MULT();
    void MULTU();
    void NOR();
    void OR();
    void RFE();
    void SB();
    void SH();
    void SLLV();
    void SLT();
    void SLTI();
    void SLTIU();
    void SLTU();
    void SRA();
    void SRAV();
    void SRL();
    void SRLV();
    void SUB();
    void SUBU();
    void SWC2();
    void SWL();
    void SWR();
    void SYSCALL();
    void XOR();
    void XORI();

    void GTEMove();
    void AVSZ3();
    void AVSZ4();
    void CC();
    void CDP();
    void DCPL();
    void DPCS();
    void DPCT();
    void GPF();
    void GPL();
    void INTPL();
    void MVMVA();
    void NCCS();
    void NCCT();
    void NCDS();
    void NCDT();
    void NCLIP();
    void NCS();
    void NCT();
    void OP();
    void RTPS();
    void RTPT();
    void SQR();

    const opfn basic[64] = {
        &Cpu::Special, &Cpu::REGIMM,  &Cpu::J,       &Cpu::JAL,      // 00
        &Cpu::BEQ,     &Cpu::BNE,     &Cpu::BLEZ,    &Cpu::BGTZ,     // 04
        &Cpu::ADDI,    &Cpu::ADDIU,   &Cpu::SLTI,    &Cpu::SLTIU,    // 08
        &Cpu::ANDI,    &Cpu::ORI,     &Cpu::XORI,    &Cpu::LUI,      // 0c
        &Cpu::COP0,    &Cpu::Unknown, &Cpu::COP2,    &Cpu::Unknown,  // 10
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 14
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 18
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 1c
        &Cpu::LB,      &Cpu::LH,      &Cpu::LWL,     &Cpu::LW,       // 20
        &Cpu::LBU,     &Cpu::LHU,     &Cpu::LWR,     &Cpu::Unknown,  // 24
        &Cpu::SB,      &Cpu::SH,      &Cpu::SWL,     &Cpu::SW,       // 28
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::SWR,     &Cpu::Unknown,  // 2c
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::LWC2,    &Cpu::Unknown,  // 30
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 34
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::SWC2,    &Cpu::Unknown,  // 38
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 3c

    };

    const opfn special[64] = {
        &Cpu::SLL,     &Cpu::Unknown, &Cpu::SRL,     &Cpu::SRA,      // 00
        &Cpu::SLLV,    &Cpu::Unknown, &Cpu::SRLV,    &Cpu::SRAV,     // 04
        &Cpu::JR,      &Cpu::JALR,    &Cpu::Unknown, &Cpu::Unknown,  // 08
        &Cpu::SYSCALL, &Cpu::BREAK,   &Cpu::Unknown, &Cpu::Unknown,  // 0c
        &Cpu::MFHI,    &Cpu::MTHI,    &Cpu::MFLO,    &Cpu::MTLO,     // 10
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 14
        &Cpu::MULT,    &Cpu::MULTU,   &Cpu::DIV,     &Cpu::DIVU,     // 18
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 1c
        &Cpu::ADD,     &Cpu::ADDU,    &Cpu::SUB,     &Cpu::SUBU,     // 20
        &Cpu::AND,     &Cpu::OR,      &Cpu::XOR,     &Cpu::NOR,      // 24
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::SLT,     &Cpu::SLTU,     // 28
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 2c
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 30
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 34
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 38
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 3c
    };

    const opfn gte[64] = {
        &Cpu::GTEMove, &Cpu::RTPS,    &Cpu::Unknown, &Cpu::Unknown,  // 00
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::NCLIP,   &Cpu::Unknown,  // 04
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 08
        &Cpu::OP,      &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 0c
        &Cpu::DPCS,    &Cpu::INTPL,   &Cpu::MVMVA,   &Cpu::NCDS,     // 10
        &Cpu::CDP,     &Cpu::Unknown, &Cpu::NCDT,    &Cpu::Unknown,  // 14
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::NCCS,     // 18
        &Cpu::CC,      &Cpu::Unknown, &Cpu::NCS,     &Cpu::Unknown,  // 1c
        &Cpu::NCT,     &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 20
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 24
        &Cpu::SQR,     &Cpu::DCPL,    &Cpu::DPCT,    &Cpu::Unknown,  // 28
        &Cpu::Unknown, &Cpu::AVSZ3,   &Cpu::AVSZ4,   &Cpu::Unknown,  // 2c
        &Cpu::RTPT,    &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 30
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 34
        &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown, &Cpu::Unknown,  // 38
        &Cpu::Unknown, &Cpu::GPF,     &Cpu::GPL,     &Cpu::NCCT,     // 3c
    };
};
