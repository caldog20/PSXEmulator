//
// Created by Caleb Yates on 1/4/23.
//

#pragma once

#include "emulator.hpp"

static const char* regs_gpr[32] = {"r0", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2",
                                   "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5",
                                   "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

class RegViewer {
  public:
    RegViewer(Emulator& emulator) : m_emulator(emulator) {}

    void draw();
    bool m_draw = false;

  private:
    Emulator& m_emulator;
};
