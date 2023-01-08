#include "mem.hpp"

#include <memory>

#include "emulator.hpp"

void Memory::init() {
    try {
        m_ram = new u8[RAM_SIZE];
        m_bios = new u8[BIOS_SIZE];
        m_scratch = new u8[SCRATCHPAD_SIZE];
        m_hw = new u8[HWREG_SIZE];
        m_para = new u8[PARAPORT_SIZE];
    } catch (...) {
        throw std::runtime_error("Error allocating memory for Emulator\n");
    }
}

Memory::~Memory() {
    delete[] m_ram;
    delete[] m_bios;
    delete[] m_scratch;
    delete[] m_hw;
    delete[] m_para;
}

void Memory::reset() {
    std::fill(m_ram, m_ram + (RAM_SIZE - 1), 0);
    std::fill(m_scratch, m_scratch + (SCRATCHPAD_SIZE - 1), 0);
    std::fill(m_hw, m_hw + (HWREG_SIZE - 1), 0);
    std::fill(m_para, m_para + (PARAPORT_SIZE - 1), 0);
}

u8 Memory::psxRead8(u32 address) {
    u32 hw_address = address & 0x1fffffff;

    // Placeholder for expansion slot
    if (EXP1.contains(address)) {
        return 0xff;
    }

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("psxRead8 CACHECONTROL address: {:#x}, hw_address {:#x}\n", address, hw_address);
        u8 final = m_cacheControl;
        return final;
    }

    if (BIOS.contains(hw_address)) {
        m_emulator.log("psxRead8 BIOS address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = BIOS.offset(hw_address);
        return read8(m_bios, offset);
    }

    if (RAM.contains(hw_address)) {
        m_emulator.log("psxRead8 RAM address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = RAM.offset(hw_address);
        return read8(m_ram, offset);
    }

    if (SCRATCHPAD.contains(hw_address)) {
        m_emulator.log("psxRead8 ScratchPad address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = SCRATCHPAD.offset(hw_address);
        return read16(m_scratch, offset);
    }

    if (MEMCONTROL.contains(hw_address)) {
        m_emulator.log("MEMCONTROL Read at address: {:#x}, masked {:#x}\n", address, hw_address);
        auto offset = MEMCONTROL.offset(hw_address);
        return read16(m_hw, offset);
    }

    if (HWREG.contains(hw_address)) {
        m_emulator.log("psxRead8 HWREG address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = HWREG.offset(hw_address);
        return read16(m_hw, offset);
    }

    if (PARAPORT.contains(hw_address)) {
        m_emulator.log("psxRead8 Parallel address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = PARAPORT.offset(hw_address);
        return read16(m_para, offset);
    }

    m_emulator.log("psxRead8: Unmatched memory region at address {:#x}, hw_address {:#x}\n", address, hw_address);
    return 0;
}

u16 Memory::psxRead16(u32 address) {
    if (address % 2 != 0) {
        m_emulator.log("Unaligned psxRead32 at address {:#x}\n", address);
        return 0;
    }
    u32 hw_address = address & 0x1fffffff;

    // Temporary fix
    if (address == 0x1F801074) return 0;

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("psxRead16 CACHECONTROL address: {:#x}, hw_address {:#x}\n", address, hw_address);
        u16 final = m_cacheControl;
        return final;
    }

    if (BIOS.contains(hw_address)) {
        m_emulator.log("psxRead16 BIOS address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = BIOS.offset(hw_address);
        return read16(m_bios, offset);
    }

    if (RAM.contains(hw_address)) {
        m_emulator.log("psxRead16 RAM address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = RAM.offset(hw_address);
        return read16(m_ram, offset);
    }

    if (SCRATCHPAD.contains(hw_address)) {
        m_emulator.log("psxRead16 ScratchPad address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = SCRATCHPAD.offset(hw_address);
        return read16(m_scratch, offset);
    }

    if (MEMCONTROL.contains(hw_address)) {
        m_emulator.log("MEMCONTROL Read at address: {:#x}, masked {:#x}\n", address, hw_address);
        auto offset = MEMCONTROL.offset(hw_address);
        return read16(m_hw, offset);
    }

    if (HWREG.contains(hw_address)) {
        m_emulator.log("psxRead16 HWREG address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = HWREG.offset(hw_address);
        return read16(m_hw, offset);
    }

    if (PARAPORT.contains(hw_address)) {
        m_emulator.log("psxRead16 Parallel address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = PARAPORT.offset(hw_address);
        return read16(m_para, offset);
    }

    m_emulator.log("psxRead16: Unmatched memory region at address {:#x}, hw_address {:#x}\n", address, hw_address);
    return 0;
}

u32 Memory::psxRead32(u32 address) {
    if (address % 4 != 0) {
        m_emulator.log("Unaligned psxRead32 at address {:#x}\n", address);
        return 0;
    }

    u32 hw_address = address & 0x1fffffff;

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("psxRead32 CACHECONTROL address: {:#x}, hw_address {:#x}\n", address, hw_address);
        u32 final = m_cacheControl;
        return final;
    }

    if (BIOS.contains(hw_address)) {
        m_emulator.log("psxRead32 BIOS address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = BIOS.offset(hw_address);
        return read32(m_bios, offset);
    }

    if (RAM.contains(hw_address)) {
        m_emulator.log("psxRead32 RAM address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = RAM.offset(hw_address);
        return read32(m_ram, offset);
    }

    if (SCRATCHPAD.contains(hw_address)) {
        m_emulator.log("psxRead32 ScratchPad address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = SCRATCHPAD.offset(hw_address);
        return read32(m_scratch, offset);
    }

    if (MEMCONTROL.contains(hw_address)) {
        m_emulator.log("MEMCONTROL Read at address: {:#x}, masked {:#x}\n", address, hw_address);
        auto offset = MEMCONTROL.offset(hw_address);
        return read32(m_hw, offset);
    }

    if (HWREG.contains(hw_address)) {
        m_emulator.log("psxRead32 HWREG address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = HWREG.offset(hw_address);
        return read32(m_hw, offset);
    }

    if (PARAPORT.contains(hw_address)) {
        m_emulator.log("psxRead32 Parallel address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = PARAPORT.offset(hw_address);
        return read32(m_para, offset);
    }

    m_emulator.log("psxRead32: Unmatched memory region at address {:#x}, hw_address {:#x}\n", address, hw_address);
    return 0;
}

void Memory::psxWrite8(u32 address, u8 value) {
    u32 hw_address = address & 0x1fffffff;

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("psxWrite8 CACHECONTROL address: {:#x}, hw_address {:#x}, value: {:#x}\n", address, hw_address,
                       value);
        m_cacheControl = value;
        return;
    }

    if (RAM.contains(hw_address)) {
        auto offset = RAM.offset(hw_address);
        write8(m_ram, offset, value);
        m_emulator.log("psxWrite8 RAM address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (SCRATCHPAD.contains(hw_address)) {
        auto offset = SCRATCHPAD.offset(hw_address);
        write8(m_scratch, offset, value);
        m_emulator.log("psxWrite8 ScratchPad address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (MEMCONTROL.contains(hw_address)) {
        u32 offset = MEMCONTROL.offset(hw_address);
        m_emulator.log("psxWrite8 MEMCONTROL address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        write8(m_hw, offset, value);
        return;
    }

    if (HWREG.contains(hw_address)) {
        auto offset = HWREG.offset(hw_address);
        write8(m_hw, offset, value);
        m_emulator.log("psxWrite8 HWREG address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (PARAPORT.contains(hw_address)) {
        auto offset = PARAPORT.offset(hw_address);
        write8(m_para, offset, value);
        m_emulator.log("psxWrite8 Parallel address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    m_emulator.log("psxWrite8: Unmatched memory region at address {:#x}, hw_address {:#x}, value: {:#x}\n", address,
                   hw_address, value);
}

void Memory::psxWrite16(u32 address, u16 value) {
    if (address % 2 != 0) {
        m_emulator.log("Unaligned psxWrite16 at address {:#x}\n", address);
        return;
    }

    u32 hw_address = address & 0x1fffffff;

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("psxWrite16 CACHECONTROL address: {:#x}, hw_address {:#x}, value: {:#x}\n", address, hw_address,
                       value);
        m_cacheControl = value;
        return;
    }

    if (RAM.contains(hw_address)) {
        auto offset = RAM.offset(hw_address);
        write16(m_ram, offset, value);
        m_emulator.log("psxWrite16 RAM address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (SCRATCHPAD.contains(hw_address)) {
        auto offset = SCRATCHPAD.offset(hw_address);
        write16(m_scratch, offset, value);
        m_emulator.log("psxWrite16 ScratchPad address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset,
                       value);
        return;
    }

    if (MEMCONTROL.contains(hw_address)) {
        u32 offset = MEMCONTROL.offset(hw_address);
        m_emulator.log("psxWrite16 MEMCONTROL address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset,
                       value);
        write16(m_hw, offset, value);
        return;
    }

    if (HWREG.contains(hw_address)) {
        auto offset = HWREG.offset(hw_address);
        write16(m_hw, offset, value);
        m_emulator.log("psxWrite16 HWREG address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (PARAPORT.contains(hw_address)) {
        auto offset = PARAPORT.offset(hw_address);
        write16(m_para, offset, value);
        m_emulator.log("psxWrite16 Parallel address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    m_emulator.log("psxWrite16: Unmatched memory region at address {:#x}, hw_address {:#x}, value: {:#x}\n", address,
                   hw_address, value);
}

void Memory::psxWrite32(u32 address, u32 value) {
    if (address % 4 != 0) {
        m_emulator.log("Unaligned psxWrite32 at address {:#x}\n", address);
        return;
    }

    u32 hw_address = address & 0x1fffffff;

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("psxWrite32 CACHECONTROL address: {:#x}, hw_address {:#x}, value: {:#x}\n", address, hw_address,
                       value);
        m_cacheControl = value;
        return;
    }

    if (RAM.contains(hw_address)) {
        auto offset = RAM.offset(hw_address);
        write32(m_ram, offset, value);
        m_emulator.log("psxWrite32 RAM address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (SCRATCHPAD.contains(hw_address)) {
        auto offset = SCRATCHPAD.offset(hw_address);
        write32(m_scratch, offset, value);
        m_emulator.log("psxWrite32 ScratchPad address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset,
                       value);
        return;
    }

    if (MEMCONTROL.contains(hw_address)) {
        u32 offset = MEMCONTROL.offset(hw_address);
        m_emulator.log("psxWrite32 MEMCONTROL address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset,
                       value);
        write32(m_hw, offset, value);
        return;
    }

    if (HWREG.contains(hw_address)) {
        auto offset = HWREG.offset(hw_address);
        write32(m_hw, offset, value);
        m_emulator.log("psxWrite32 HWREG address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (PARAPORT.contains(hw_address)) {
        auto offset = PARAPORT.offset(hw_address);
        write32(m_para, offset, value);
        m_emulator.log("psxWrite32 Parallel address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    m_emulator.log("psxWrite32: Unmatched memory region at address {:#x}, hw_address {:#x}, value: {:#x}\n", address,
                   hw_address, value);
}

u8 Memory::read8(u8* region, u32 offset) { return *(region + offset); }

u16 Memory::read16(u8* region, u32 offset) { return *(u16*)(region + offset); }

u32 Memory::read32(u8* region, u32 offset) { return *(u32*)(region + offset); }

void Memory::write8(u8* region, u32 offset, u8 value) { *(region + offset) = value; }

void Memory::write16(u8* region, u32 offset, u16 value) { *(u16*)(region + offset) = value; }

void Memory::write32(u8* region, u32 offset, u32 value) { *(u32*)(region + offset) = value; }