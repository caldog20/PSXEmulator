#pragma once

#include "emulator.hpp"

class MemViewer {
  public:
    MemViewer(Emulator& emulator) : m_emulator(emulator) {}

    void draw();
    void walkBios();
    void walkRam();
    void walkScratch();
    void walkHw();
    void walkPara();

    enum REGION { BIOS, RAM, SCRATCHPAD, HWREG, PARA };

    void search(REGION region, u32 address);

    void clear() {
        m_bios.clear();
        m_ram.clear();
        m_hw.clear();
        m_scratch.clear();
        m_para.clear();
    };

    bool m_draw = false;

  private:
    Emulator& m_emulator;

    std::vector<u32> m_bios;
    std::vector<u32> m_ram;
    std::vector<u32> m_hw;
    std::vector<u32> m_scratch;
    std::vector<u32> m_para;
};
