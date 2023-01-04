#include "mem.hpp"
#include <iostream>
#include <exception>

void Memory::init() {
    try {
        m_ram = new u8[RAM_SIZE];
        m_bios = new u8[BIOS_SIZE];
    } catch (...) {
        std::cerr << "Error allocating memory" << std::endl;
    }
}

Memory::~Memory() {
    if (m_ram) delete[] m_ram;
    if (m_bios) delete[] m_bios;
}
