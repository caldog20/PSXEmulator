#pragma once
#include <array>
#include <string>

#include "cpu.hpp"
#include "logger.hpp"
#include "mem.hpp"
#include "utils.hpp"

class Memory;
class Cpu;
class Logger;

class Emulator {
  public:
    static const int width = 160;
    static const int height = 144;

    void reset();
    void step();
    void runFrame();

    void loadBios(const std::string& path);

    template <typename... Args>
    void log(const char* fmt, const Args&... args) {
        Helpers::log(fmt, args...);
        if (m_enableLog) {
            m_logger.AddLog(fmt, args...);
        }
    }

    bool isRunning = false;
    int framesPassed = 0;
    std::array<u8, width * height * 4> framebuffer;  // An 160x144 RGBA framebuffer

    Emulator() { framebuffer.fill(0xFF); }

    Memory m_mem{*this};
    Cpu m_cpu{*this};
    Logger m_logger;

    bool m_enableLog = true;
};