//
// Created by Caleb Yates on 1/4/23.
//

#include "cpu.hpp"

#include <memory>

#include "emulator.hpp"
#include "mnemonics.hpp"

void Cpu::init() { reset(); }

void Cpu::reset() {
    std::memset(&m_regs, 0, sizeof(m_regs));
    m_regs.gpr.zero = 0;
    m_regs.pc = START_PC;
    m_regs.next_pc = START_PC + 4;
    m_instruction = 0;
    m_branchDelay = false;
    m_loadDelay = false;
}

void Cpu::fetch() {
    m_regs.gpr.zero = 0;
    m_instruction.set(m_emulator.m_mem.read32(m_regs.pc));
    m_emulator.log("Current PC: {:#x} Instruction: {:#x}\n", m_regs.pc, m_instruction.ins);
    logMnemonic();
}

void Cpu::step() {
    // Lookup instruction in basic LUT
    const auto bd = basic[m_instruction.opcode];
    // Execute the function pointed too by LUT pointer
    (this->*bd)();

    if (m_loadDelay && m_inLoadDelaySlot) {
        if (m_instruction.rt == m_regs.ld_target) {
            return;
        }

        m_regs.set(m_regs.ld_target, m_regs.ld_value);
        m_loadDelay = false;
        m_inLoadDelaySlot = false;
    }

    if (m_branchDelay && m_inBranchDelaySlot) {
        m_regs.pc = m_regs.jumppc;
        m_regs.next_pc = m_regs.jumppc + 4;
        m_regs.jumppc = 0;
        m_branchDelay = false;
        m_inBranchDelaySlot = false;
        fetch();
        return;
    }

    if (m_branchDelay) {
        m_inBranchDelaySlot = true;
    }

    if (m_loadDelay) {
        m_inLoadDelaySlot = true;
    }

    m_regs.nextpc();
    fetch();
}

void Cpu::logMnemonic() {
    const char* mnemonic = "";
    if (m_instruction.opcode == 0) {
        mnemonic = mnemonic_special[m_instruction.fn];
    } else {
        mnemonic = mnemonic_basic[m_instruction.opcode];
    }
    m_emulator.log("{}\n", mnemonic);
}