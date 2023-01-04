#pragma once
#include "utils.hpp"


#define BIOS_SIZE (512 * 1024)
#define RAM_SIZE (2048 * 1024)

class Memory {
  public:
    Memory() = default;

    ~Memory();

    void init();

    u8 read8(u32 address);
    u16 read16(u32 address);
    u32 read32(u32 address);

    void write8(u32 address, u8 value);
    void write16(u32 address, u16 value);
    void write32(u32 address, u32 value);

  private:
    u8 *m_ram = nullptr;
    u8 *m_bios = nullptr;
};
