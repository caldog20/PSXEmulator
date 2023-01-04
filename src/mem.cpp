#include "mem.hpp"
#include <iostream>
#include <exception>

void Memory::init() {
    try {
        m_ram = std::make_unique<u8[]>(RAM_SIZE);
        m_bios = std::make_unique<u8[]>(BIOS_SIZE);
    } catch (...) {
        std::cerr << "Error allocating memory" << std::endl;
    }
}
