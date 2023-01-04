#pragma once
#include <array>
#include <string>

#include "cpu.hpp"
#include "mem.hpp"
#include "utils.hpp"

class Emulator {
  public:
    static const int width = 160;
    static const int height = 144;

    void reset();
    void step();
    void runFrame();

    void loadBios(const std::string& path);

    bool isRunning = false;
    int framesPassed = 0;
    std::array<u8, width * height * 4> framebuffer;  // An 160x144 RGBA framebuffer

    Emulator() { framebuffer.fill(0xFF); }

  private:
    friend class Disassembly;
    Memory m_mem;
    Cpu m_cpu{*this};
};