#include "mem.hpp"

#include <exception>
#include <iostream>
#include <memory>

#include "emulator.hpp"

void Memory::init() {
    try {
        m_ram = std::make_unique<u8[]>(RAM_SIZE);
        m_bios = std::make_unique<u8[]>(BIOS_SIZE);
    } catch (...) {
        std::cerr << "Error allocating memory" << std::endl;
    }
}

void Memory::reset() { std::memset(m_ram.get(), 0, RAM_SIZE); }

u32 Memory::read32(u32 address) {
    if (address % 4 != 0) m_emulator.log("Unaligned read32 at address {:#x}", address);

    if (BIOS.contains(address)) {
        auto offset = BIOS.offset(address);

        auto b3 = m_bios[offset + 3];
        auto b2 = m_bios[offset + 2];
        auto b1 = m_bios[offset + 1];
        auto b0 = m_bios[offset + 0];

        u32 final = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
        return final;
    }

    if (RAM.contains(address << 3)) {
        auto offset = RAM.offset(address << 3);
        auto b3 = m_ram[offset + 3];
        auto b2 = m_ram[offset + 2];
        auto b1 = m_ram[offset + 1];
        auto b0 = m_ram[offset + 0];
        u32 final = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
        return final;
    }

    m_emulator.log("Unmatched memory region at address {:#x}", address);
    return 0;
}

void Memory::write32(u32 address, u32 value) {
    u32 page = address >> 29;
    m_emulator.log("write32 Address: {:#x} Page: {:#x} Value: {:#x}\n", address, page, value);

    if (MEMCONTROL.contains(address)) {
        u32 offset = MEMCONTROL.offset(address);
        m_emulator.log("Write to MEMCONTROL address: {:#x}, offset: {:#x}, value: {:#x}\n", address, offset, value);
        m_ram[offset] = value;
        return;
    }

    if (RAM.contains(address << 3)) {
        auto offset = RAM.offset(address << 3);
        *(u32*)(m_ram.get() + offset) = value;
    }
}
