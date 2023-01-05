#pragma once

#include "emulator.hpp"

class MemViewer {
  public:
    MemViewer(Emulator& emulator) : m_emulator(emulator) {}

    void draw();
    void walkBios();
    void walkRam();

    void clear() {
        m_biosMem.clear();
        m_mainMem.clear();
    };

    bool m_draw = false;

  private:
    Emulator& m_emulator;

    std::vector<u32> m_biosMem;
    std::vector<u32> m_mainMem;
};
