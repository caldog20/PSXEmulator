#pragma once
#include <array>

#include "utils.hpp"
#include "mem.hpp"
#include <string>

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

    Emulator() {
        framebuffer.fill(0xFF);
    }

  private:
    Memory m_mem;
};