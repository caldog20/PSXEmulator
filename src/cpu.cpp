//
// Created by Caleb Yates on 1/4/23.
//

#include "cpu.hpp"

#include <memory>

void Cpu::init() {
    std::memset(&m_regs, 0, sizeof(m_regs));
    m_branchDelay = false;
    m_loadDelay = false;
}

void Cpu::step() {}
