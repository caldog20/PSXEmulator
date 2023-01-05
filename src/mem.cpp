#include "mem.hpp"

#include <exception>
#include <iostream>
#include <memory>

#include "emulator.hpp"

void Memory::init() {
    try {
        m_ram = std::make_unique<u8[]>(RAM_SIZE);
        m_bios = std::make_unique<u8[]>(BIOS_SIZE);
        m_scratch = std::make_unique<u8[]>(SCRATCHPAD_SIZE);
        m_hw = std::make_unique<u8[]>(HWREG_SIZE);
        m_para = std::make_unique<u8[]>(PARAPORT_SIZE);
    } catch (...) {
        throw std::runtime_error("Error allocating memory for Emulator\n");
    }
}

void Memory::reset() {
    std::memset(m_ram.get(), 0, RAM_SIZE);
    std::memset(m_scratch.get(), 0, SCRATCHPAD_SIZE);
    std::memset(m_hw.get(), 0, HWREG_SIZE);
    std::memset(m_para.get(), 0, PARAPORT_SIZE);
}

u32 Memory::read32(u32 address) {
    if (address % 4 != 0) m_emulator.log("Unaligned read32 at address {:#x}\n", address);
    m_emulator.log("read32 {:#x}\n", address);

    u32 hw_address = address & 0x1fffffff;

    if (MEMCONTROL.contains(hw_address)) {
        m_emulator.log("MEMCONTROL Read at address: {:#x}, masked {:#x}\n", address, hw_address);
        auto offset = MEMCONTROL.offset(hw_address);
        u32 final = *(u32*)(m_hw.get() + offset);
        return final;
    }

    if (HWREG.contains(hw_address)) {
        m_emulator.log("read32 HWREG address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = HWREG.offset(hw_address);
        u32 final = *(u32*)(m_hw.get() + offset);
        return final;
    }

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("read32 CACHECONTROL address: {:#x}, hw_address {:#x}\n", address, hw_address);
        u32 final = m_cacheControl;
        return final;
    }

    if (BIOS.contains(hw_address)) {
        m_emulator.log("read32 BIOS address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = BIOS.offset(hw_address);
        u32 final = *(u32*)(m_bios.get() + offset);
        return final;
    }

    if (RAM.contains(hw_address)) {
        m_emulator.log("read32 RAM address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = RAM.offset(hw_address);
        u32 final = *(u32*)(m_ram.get() + offset);
        return final;
    }

    if (SCRATCHPAD.contains(hw_address)) {
        m_emulator.log("read32 ScratchPad address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = SCRATCHPAD.offset(hw_address);
        u32 final = *(u32*)(m_scratch.get() + offset);
        return final;
    }

    if (PARAPORT.contains(hw_address)) {
        m_emulator.log("read32 Parallel address: {:#x}, hw_address {:#x}\n", address, hw_address);
        auto offset = PARAPORT.offset(hw_address);
        u32 final = *(u32*)(m_para.get() + offset);
        return final;
    }

    m_emulator.log("read32: Unmatched memory region at address {:#x}, hw_address {:#x}\n", address, hw_address);
    return 0;
}

void Memory::write32(u32 address, u32 value) {
    u32 hw_address = address & 0x1fffffff;

    if (CACHECONTROL.contains(address)) {
        m_emulator.log("write32 CACHECONTROL address: {:#x}, hw_address {:#x}, value: {:#x}\n", address, hw_address,
                       value);
        m_cacheControl = value;
        return;
    }

    if (MEMCONTROL.contains(hw_address)) {
        u32 offset = MEMCONTROL.offset(hw_address);
        m_emulator.log("write32 MEMCONTROL address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        *(u32*)(m_hw.get() + offset) = value;
        return;
    }

    if (HWREG.contains(hw_address)) {
        auto offset = HWREG.offset(hw_address);
        *(u32*)(m_hw.get() + offset) = value;
        m_emulator.log("write32 HWREG address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (RAM.contains(hw_address)) {
        auto offset = RAM.offset(hw_address);
        *(u32*)(m_ram.get() + offset) = value;
        m_emulator.log("write32 RAM address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (SCRATCHPAD.contains(hw_address)) {
        auto offset = SCRATCHPAD.offset(hw_address);
        *(u32*)(m_scratch.get() + offset) = value;
        m_emulator.log("write32 ScratchPad address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    if (PARAPORT.contains(hw_address)) {
        auto offset = PARAPORT.offset(hw_address);
        *(u32*)(m_para.get() + offset) = value;
        m_emulator.log("write32 Parallel address: {:#x}, offset: {:#x}, value: {:#x}\n", hw_address, offset, value);
        return;
    }

    m_emulator.log("write32: Unmatched memory region at address {:#x}, hw_address {:#x}, value: {:#x}\n", address,
                   hw_address, value);
}
