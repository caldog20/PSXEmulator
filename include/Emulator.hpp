#pragma once
#include <array>

#include "utils.hpp"

class Emulator {
  public:
    static const int width = 160;
    static const int height = 144;

    void step();
    void runFrame();

    bool isRunning = false;
    int framesPassed = 0;
    std::array<u8, width * height * 4> framebuffer;  // An 160x144 RGBA framebuffer

    Emulator() { framebuffer.fill(0xFF); }
};