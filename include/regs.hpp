#pragma once

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

    void markWbIndex(u32 index) { ld_target = index; }
    void setWbValue(u32 value) { ld_value = value; }
    void wbLoadDelay() {
        set(ld_target, ld_value);
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
