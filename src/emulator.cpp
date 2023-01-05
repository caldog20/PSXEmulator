#include "emulator.hpp"

#include "fmt/format.h"

void Emulator::step() {
    log("Step\n");
    m_cpu.step();
    log("End Step\n\n");
}

void Emulator::runFrame() {
    log("Frame {}\n", framesPassed++);
    step();
}

void Emulator::loadBios(const std::string& path) {
    log("Loading BIOS file {}\n", path);
    auto [bios, hash] = Helpers::loadROMWithHash(path);
    log("BIOS Size: {}kb\n", bios.size() / 1024);
    log("BIOS SHA1 SUM: {}\n", hash);
    if (!bios.size()) {
        log("Invalid BIOS File\n");
        return;
    }
    std::memcpy(m_mem.m_bios.get(), bios.data(), bios.size());
    m_cpu.fetch();
}

void Emulator::reset() {
    isRunning = false;
    m_mem.reset();
    m_cpu.reset();
}
