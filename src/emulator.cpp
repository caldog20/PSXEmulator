#include "emulator.hpp"

#include "fmt/format.h"

void Emulator::step() {
    log("Step\n");
    m_cpu.step();
    log("\n");
}

void Emulator::runFrame() { log("Frame {}\n", framesPassed++); }

void Emulator::loadBios(const std::string& path) {
    log("Loading BIOS file {}\n", path);
    auto [bios, hash] = Helpers::loadROMWithHash(path);
    log("BIOS Size: {}kb\n", bios.size() / 1024);
    log("BIOS SHA1 SUM: {}\n", hash);

    std::memcpy(m_mem.m_bios.get(), bios.data(), bios.size());
}
