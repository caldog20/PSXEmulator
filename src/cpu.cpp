//
// Created by Caleb Yates on 1/4/23.
//

#include "cpu.hpp"

#include <memory>

#include "emulator.hpp"
#include "mnemonics.hpp"

void Cpu::init() {
    std::memset(&m_regs, 0, sizeof(m_regs));
    m_regs.gpr.zero = 0;
    m_regs.pc = START_PC;
    m_branchDelay = false;
    m_loadDelay = false;
}

void Cpu::step() {
    m_regs.gpr.zero = 0;
    m_instruction.set(m_emulator.m_mem.read32(m_regs.pc));
    m_emulator.log("Instruction: {:X}\n", m_instruction.ins);
    logMnemonic();

    // Lookup instruction in basic LUT
    const auto bd = basic[m_instruction.opcode];
    // Execute the function pointed too by LUT pointer
    (this->*bd)();

    m_regs.pc += 4;
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