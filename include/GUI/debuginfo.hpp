//
// Created by Caleb Yates on 1/4/23.
//

#pragma once
#include "emulator.hpp"

class DebugInfo {
  public:
    DebugInfo(Emulator& emulator) : m_emulator(emulator) {}

    void draw();
    bool m_draw = false;

  private:
    Emulator& m_emulator;
};
