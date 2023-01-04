//
// Created by Caleb Yates on 1/4/23.
//

#pragma once
#include <SFML/Graphics.hpp>

#include "emulator.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "utils.hpp"

class Disassembly {
  public:
    Disassembly(Emulator& emulator) : m_emulator(emulator) {}

    void draw();
    void clear() {
        m_ins.clear();
        m_history.clear();
        m_codeSize = 0;
    };
    size_t disassembleBios();

    bool m_draw = false;
    size_t m_codeSize = 0;

  private:
    Emulator& m_emulator;
    std::vector<std::string> m_ins;
    std::vector<std::string> m_history;
    ssize_t m_historyPos = -1;
    bool m_autoScroll = true;
    bool m_scrollToBottom = false;
};
