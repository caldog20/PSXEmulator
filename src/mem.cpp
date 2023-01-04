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
