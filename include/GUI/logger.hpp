//
// Created by Caleb Yates on 1/4/23.
//

#pragma once

#include <fmt/format.h>

#include <string>

#include "imgui.h"

class Logger {
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets;  // Index to lines offset. We maintain this with AddLog() calls.

  public:
    bool AutoScroll;  // Keep scrolling if already at the bottom.
    bool m_draw = false;

    Logger() {
        AutoScroll = true;
        Clear();
    }

    void Clear() {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    template <typename... Args>
    void AddLog(const char* fmt, const Args&... args) {
        int old_size = Buf.size();
        std::string log = fmt::format(fmt::runtime(fmt), args...);
        Buf.append(log.c_str());
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n') LineOffsets.push_back(old_size + 1);
    }

    void draw();
};
