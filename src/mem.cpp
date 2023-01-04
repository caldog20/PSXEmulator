#include "mem.hpp"

#include <exception>
#include <iostream>

#include "emulator.hpp"

void Memory::init() {
    try {
        m_ram = std::make_unique<u8[]>(RAM_SIZE);
        m_bios = std::make_unique<u8[]>(BIOS_SIZE);
    } catch (...) {
        std::cerr << "Error allocating memory" << std::endl;
    }
}
u32 Memory::read32(u32 address) {
    if (address % 4 != 0) m_emulator.log("Unaligned read32 at address {:X}", address);

    if (BIOS.contains(address)) {
        auto offset = BIOS.offset(address);

        auto b3 = m_bios[offset + 3];
        auto b2 = m_bios[offset + 2];
        auto b1 = m_bios[offset + 1];
        auto b0 = m_bios[offset + 0];

        u32 final = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
        return final;
    }
    m_emulator.log("Unmatched memory region at address {:X}", address);
    return 0;
}

void Memory::write32(u32 address, u32 value) {
    u32 page = address >> 29;
    m_emulator.log("write32 Address: {:X} Page: {:X} Value: {:X}\n", address, page, value);

    if (MEMCONTROL.contains(address)) {
        u32 offset = MEMCONTROL.offset(address);
        m_emulator.log("Write to MEMCONTROL address: {:X}, offset: {:X}, value: {:X}\n", address, offset, value);
        m_ram[offset] = value;
        return;
    }

    u32 real_address = address << 3;
    m_emulator.log("Real Address {:X}\n", real_address);

}
