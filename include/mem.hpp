#pragma once
#include <array>
#include <map>
#include <memory>
#include <vector>

#include "utils.hpp"
class Emulator;

#define BIOS_BASE (0x1fc00000)
#define RAM_BASE (0x00000000)
#define SCRATCHPAD_BASE (0x1f800000)
#define HWREG_BASE (0x1f801000)
#define PARAPORT_BASE (0x1f000000)

#define BIOS_SIZE (0x80000)
#define RAM_SIZE (0x200000)
#define SCRATCHPAD_SIZE (0x400)
#define HWREG_SIZE (0x2000)
#define PARAPORT_SIZE (0x10000)
#define MEMCONTROL_SIZE (0x20)
#define CACHECONTROL_SIZE (4)

using Helpers::Range;

enum class REGION { NONE, BIOS, RAM, SCRATCHPAD, IO, CACHE_CONTROL };

template <typename T>
struct Mapper {
    Mapper() = default;

    void addRange(REGION region, Range<T>& range) { ranges.push_back({region, range}); }

    REGION contains(u32 address) {
        for (auto& range : ranges) {
            if (range.second.contains((address))) {
                return range.first;
            }
        }
        return REGION::NONE;
    }

    std::map<REGION, Range<T>> ranges;
};

class Memory {
  public:
    Memory(Emulator& emulator) : m_emulator(emulator) { init(); }
    ~Memory() = default;

    void init();
    void reset();

    u8 read8(u32 address);
    u16 read16(u32 address);
    u32 read32(u32 address);

    void write8(u32 address, u8 value);
    void write16(u32 address, u16 value);
    void write32(u32 address, u32 value);

    std::unique_ptr<u8[]> m_ram = nullptr;
    std::unique_ptr<u8[]> m_bios = nullptr;
    std::unique_ptr<u8[]> m_scratch = nullptr;
    std::unique_ptr<u8[]> m_hw = nullptr;
    std::unique_ptr<u8[]> m_para = nullptr;
    u32 m_cacheControl = 0;

    const Range<u32> BIOS = Range<u32>(BIOS_BASE, BIOS_SIZE);
    const Range<u32> RAM = Range<u32>(RAM_BASE, RAM_SIZE);
    const Range<u32> SCRATCHPAD = Range<u32>(SCRATCHPAD_BASE, SCRATCHPAD_SIZE);
    const Range<u32> PARAPORT = Range<u32>(PARAPORT_BASE, PARAPORT_SIZE);
    const Range<u32> HWREG = Range<u32>(HWREG_BASE, HWREG_SIZE);
    const Range<u32> MEMCONTROL = Range<u32>(0x1f801000, MEMCONTROL_SIZE);
    const Range<u32> CACHECONTROL = Range<u32>(0xFFFE0130, CACHECONTROL_SIZE);

    Emulator& m_emulator;
};
