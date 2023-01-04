#pragma once
#include <array>
#include <memory>

#include "utils.hpp"
class Emulator;

#define BIOS_SIZE (512 * 1024)
#define RAM_SIZE (2048 * 1024)

template <typename T>
struct Range {
    Range(T begin, T size) : start(begin), length(size) {
        static_assert(std::is_integral<T>::value, "Range should take an unsigned integer type");
    }

    inline bool contains(u32 addr) const { return (addr >= start && addr < start + length); }

    inline u32 offset(u32 addr) const { return addr - start; }

    T start = 0;
    T length = 0;
};

class Memory {
  public:
    Memory(Emulator& emulator) : m_emulator(emulator) { init(); }
    ~Memory() = default;

    void init();

    u8 read8(u32 address);
    u16 read16(u32 address);
    u32 read32(u32 address);

    void write8(u32 address, u8 value);
    void write16(u32 address, u16 value);
    void write32(u32 address, u32 value);

    std::unique_ptr<u8[]> m_ram = nullptr;
    std::unique_ptr<u8[]> m_bios = nullptr;

    Range<u32> BIOS = Range<u32>(0xbfc00000, BIOS_SIZE);

    Emulator& m_emulator;
};
