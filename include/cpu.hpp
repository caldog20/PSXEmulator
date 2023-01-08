#pragma once
#include <map>

#include "exceptions.hpp"
#include "utils.hpp"

class Emulator;

#define START_PC (0xbfc00000)

// Registers
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
        u32 lo;
        u32 hi;
    };
    u32 r[2];
} spr_t;

typedef union {
    struct {
        u32 index;
        u32 rand;
        u32 elo0;
        u32 bpc;
        u32 context;
        u32 bda;
        u32 pmask;
        u32 dcic;
        u32 bvaddr;
        u32 bdam;
        u32 ehi;
        u32 bpcm;
        u32 sr;
        u32 cause;
        u32 epc;
        u32 prid;
        u32 config;
        u32 lladdr;
        u32 watchlo;
        u32 watchhi;
        u32 xcontent;
        u32 rsv1;
        u32 rsv2;
        u32 rsv3;
        u32 rsv4;
        u32 rsv5;
        u32 ecc;
        u32 cacheerr;
        u32 taglo;
        u32 taghi;
        u32 errepc;
        u32 rsv6;
    };
    u32 r[32];
} copr_t;

struct Regs {
    gpr_t gpr;
    spr_t spr;
    copr_t copr;
    u32 pc;
    u32 jumppc;
    u32 next_pc;
    u32 link_pc;
    u32 backup_pc;
    u32 ld_target;
    u32 ld_value;
    u32 opcode;
    u32 count;
    u32 cycles;

    bool writeBack = false;

    void markWbIndex(u32 index) { ld_target = index; }

    void cancelLoad() {
        if (writeBack) writeBack = false;
    }

    void set(u32 reg, u32 value) { gpr.r[reg] = value; }
    void setcopr(u32 reg, u32 value) { copr.r[reg] = value; }

    u32 get(u32 reg) { return gpr.r[reg]; }
    u32 getcopr(u32 reg) { return copr.r[reg]; }

    void nextpc() {
        pc += 4;
        next_pc += 4;
    }
};

// Instruction Decoder

struct Instruction {
    u32 ins;
    u32 opcode;
    u32 rs;
    u32 rt;
    u32 imm;
    u32 tar;
    u32 sa;
    u32 fn;
    u32 rd;
    u32 im;
    u32 immse;
    u32 immlui;
    u32 bgez;
    u32 b_link;

    Instruction(u32 instruction) { set(instruction); }

    void set(u32 instruction) {
        ins = instruction;
        opcode = instruction >> 26;
        rs = (instruction >> 21) & 0x1f;
        rt = (instruction >> 16) & 0x1F;
        imm = instruction & 0xffff;
        fn = instruction & 0x3f;
        sa = (instruction >> 6) & 0x1f;
        tar = instruction & 0x03ffffff;
        rd = (instruction >> 11) & 0x1F;
        im = (u16)instruction;
        immse = (s16)instruction;
        immlui = instruction << 16;
        bgez = (instruction >> 16) & 1;
        b_link = (instruction >> 20) & 1;
    }
};

using Helpers::log;

class Cpu {
  public:
    using opfn = void (Cpu::*)();

    Cpu(Emulator& emulator) : m_emulator(emulator) { init(); }

    void init();
    void step();
    void logMnemonic();
    void fetch();
    void reset();
    void checkPendingLoad();
    void handleLoadDelay();
    void handleBranchDelay();

    inline void pendingLoad(u32 rt, u32 value) {
        m_regs.ld_target = rt;
        m_regs.ld_value = value;
    }

    Regs m_regs;
    bool m_loadDelay = false;
    bool m_inLoadDelaySlot = false;
    bool m_branchDelay = false;
    bool m_inBranchDelaySlot = false;
    bool m_branching = false;
    u32 m_pendingLoad = 0;

    Instruction m_instruction{0};
    Emulator& m_emulator;

  private:
    void ExceptionHandler(Exception cause);
    void Branch(bool link = false);
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
