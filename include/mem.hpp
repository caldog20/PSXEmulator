#pragma once
#include <array>
#include <memory>

#include "utils.hpp"
class Emulator;

#define BIOS_SIZE (0x80000)
#define RAM_SIZE (0x80000000)
#define KUSEG_SIZE (0x80000000)
#define KSEG0_SIZE (0x20000000)
#define KSEG1_SIZE (0x20000000)
#define KSEG2_SIZE (0x40000000)

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

    const Range<u32> BIOS = Range<u32>(0xbfc00000, BIOS_SIZE);
    const Range<u32> RAM = Range<u32>(0x00000000, RAM_SIZE);
    const Range<u32> SCRATCHPAD = Range<u32>(0x1F800000, 1024);
    const Range<u32> MEMCONTROL = Range<u32>(0x1f801000, 0x20);
    const Range<u32> RAMSIZE = Range<u32>(0x1F801060, 4);
    const Range<u32> CACHECONTROL = Range<u32>(0xFFFE0130, 4);
    const Range<u32> IO = Range<u32>(0x1F801000, 8 * 1024);

    u32 m_mask[3] = {
        0x0,
        0x80,
        0xA0
    };

    Emulator& m_emulator;
};
