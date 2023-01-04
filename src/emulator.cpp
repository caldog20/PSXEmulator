#include "emulator.hpp"

#include "fmt/format.h"

void Emulator::step() { fmt::print("Step\n"); }

void Emulator::runFrame() { fmt::print("Frame {}\n", framesPassed++); }