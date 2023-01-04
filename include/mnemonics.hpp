#pragma once

static const char* mnemonic_basic[64] = {
    "Special", "REGIMM",  "J",       "JAL",     "BEQ",     "BNE",     "BLEZ",    "BGTZ",    "ADDI",    "ADDIU",
    "SLTI",    "SLTIU",   "ANDI",    "ORI",     "XORI",    "LUI",     "COP0",    "Unknown", "COP2",    "Unknown",
    "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
    "Unknown", "Unknown", "LB",      "LH",      "LWL",     "LW",      "LBU",     "LHU",     "LWR",     "Unknown",
    "SB",      "SH",      "SWL",     "SW",      "Unknown", "Unknown", "SWR",     "Unknown", "Unknown", "Unknown",
    "LWC2",    "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "SWC2",    "Unknown",
    "Unknown", "Unknown", "Unknown", "Unknown"};

static const char* mnemonic_special[64] = {
    "SLL",     "Unknown", "SRL",     "SRA",     "SLLV",    "Unknown", "SRLV",    "SRAV",    "JR",      "JALR",
    "Unknown", "Unknown", "SYSCALL", "BREAK",   "Unknown", "Unknown", "MFHI",    "MTHI",    "MFLO",    "MTLO",
    "Unknown", "Unknown", "Unknown", "Unknown", "MULT",    "MULTU",   "DIV",     "DIVU",    "Unknown", "Unknown",
    "Unknown", "Unknown", "ADD",     "ADDU",    "SUB",     "SUBU",    "AND",     "OR",      "XOR",     "NOR",
    "Unknown", "Unknown", "SLT",     "SLTU",    "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
    "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
    "Unknown", "Unknown", "Unknown", "Unknown"};

static const char* mnemonic_gte[64] = {
    "GTEMove", "RTPS",    "Unknown", "Unknown", "Unknown", "Unknown", "NCLIP",   "Unknown", "Unknown", "Unknown",
    "Unknown", "Unknown", "OP",      "Unknown", "Unknown", "Unknown", "DPCS",    "INTPL",   "MVMVA",   "NCDS",
    "CDP",     "Unknown", "NCDT",    "Unknown", "Unknown", "Unknown", "Unknown", "NCCS",    "CC",      "Unknown",
    "NCS",     "Unknown", "NCT",     "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
    "SQR",     "DCPL",    "DPCT",    "Unknown", "Unknown", "AVSZ3",   "AVSZ4",   "Unknown", "RTPT",    "Unknown",
    "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
    "Unknown", "GPF",     "GPL",     "NCCT",
};
