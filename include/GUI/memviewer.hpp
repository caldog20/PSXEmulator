#pragma once

#include "emulator.hpp"
#include "mem.hpp"

#define INCREMENT (0x200)
#define SETCOUNT(index) (index + INCREMENT)

class MemViewer {
  public:
    MemViewer(Emulator& emulator) : m_emulator(emulator) {}

    enum REGION { BIOS, RAM, SCRATCHPAD, HWREG, PARA };

    void draw();
    void walkRegion(REGION region);

    void search(REGION region, u32 address);

    void clear() {
        m_showMemory = false;
        m_search = false;
    };

    bool m_draw = false;

    u32 m_index = 0xbfc00000;
    u32 m_count = SETCOUNT(m_index);

    bool m_showMemory = false;

    bool m_search = false;
    u32 m_searchAddress{};
    REGION m_searchRegion = REGION::BIOS;

  private:
    Emulator& m_emulator;
    Memory& m_psxMem{m_emulator.m_mem};
};
