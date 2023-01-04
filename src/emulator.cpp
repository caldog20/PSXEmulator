#include "emulator.hpp"

#include "fmt/format.h"

void Emulator::step() { fmt::print("Step\n"); }

void Emulator::runFrame() { fmt::print("Frame {}\n", framesPassed++); }

void Emulator::loadBios(const std::string& path) {
    fmt::print("Loading BIOS file {}\n", path);
    auto [bios, hash] = Helpers::loadROMWithHash(path);
    fmt::print("BIOS Size: {}kb\n", bios.size() / 1024);
    fmt::print("BIOS SHA1 SUM: {}\n", hash);

    std::memcpy(m_mem.m_bios.get(), bios.data(), bios.size());
}
