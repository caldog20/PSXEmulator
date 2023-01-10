//
// Created by Caleb Yates on 1/4/23.
//

#include "cpu.hpp"

#include <bitset>
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
    m_instruction = m_emulator.m_mem.psxRead32(m_regs.pc);
    m_emulator.log("Current PC: {:#x} Instruction: {:#x}\n", m_regs.pc, m_instruction.code);
}

void Cpu::checkPendingLoad() {
    m_pendingLoad += 1;
    if (m_pendingLoad == 2) {
        m_regs.set(m_regs.ld_target, m_regs.ld_value);
        m_pendingLoad -= 1;
        m_loadDelay = false;
        m_inLoadDelaySlot = false;
    }
}

void Cpu::handleLoadDelay() {
    if (m_inLoadDelaySlot && testCancelLoad()) clearLoadDelay();
    if (m_loadDelay && m_inLoadDelaySlot) {
        m_regs.wbLoadDelay();
        clearLoadDelay();
    }

    if (m_loadDelay) {
        m_inLoadDelaySlot = true;
    }
}

void Cpu::handleBranchDelay() {
    if (m_branchDelay && m_inBranchDelaySlot) {
        m_regs.pc = m_regs.jumppc;
        m_regs.next_pc = m_regs.jumppc + 4;
        m_regs.jumppc = 0;
        if (m_regs.link_pc) {
            m_regs.gpr.ra = m_regs.link_pc;
            m_regs.link_pc = 0;
        }
        if (m_loadDelay) {
            m_inLoadDelaySlot = true;
        }
        m_branchDelay = false;
        m_inBranchDelaySlot = false;
        m_branching = true;
    }

    if (m_branchDelay) {
        m_inBranchDelaySlot = true;
    }
}

void Cpu::step() {
    m_branching = false;
    m_regs.backup_pc = m_regs.pc;

    if ((m_regs.pc % 4) != 0) {
        m_emulator.log("Unaligned PC {:#x}", m_regs.pc);
        ExceptionHandler(BadLoadAddress);
        return;
    }

    // Lookup instruction in basic LUT
    const auto bd = basic[m_instruction.opcode];
    // Execute the function pointed too by LUT pointer
    (this->*bd)();

    m_regs.count++;
    m_regs.cycles++;

    handleLoadDelay();
    handleBranchDelay();

    if (!m_branching) m_regs.nextpc();

    fetch();
    m_emulator.checktoBreak();

    // TEMP FIX FOR MISSING GPU
    m_emulator.m_mem.write32(m_emulator.m_mem.m_hw, 0xe8, 0);
}

void Cpu::logMnemonic() {
    if (m_emulator.m_enableLog) {
        const char* mnemonic = "";
        if (m_instruction.opcode == 0) {
            mnemonic = mnemonic_special[m_instruction.fn];
        } else {
            mnemonic = mnemonic_basic[m_instruction.opcode];
        }
        m_emulator.log("{}\n", mnemonic);
    }
}