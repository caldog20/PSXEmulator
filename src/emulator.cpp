#include "emulator.hpp"

#include "fmt/format.h"

void Emulator::step() {
    if (!m_biosLoaded) return;
    log("Step\n");
    m_cpu.step();
    log("\n");
}

void Emulator::runFrame() {
    if (!m_biosLoaded) return;
    log("Frame {}\n", framesPassed++);
    m_cpu.step();
}

void Emulator::loadBios(const std::string& path) {
    log("Loading BIOS file {}\n", path);

    auto [bios, hash] = Helpers::loadROMWithHash(path);


    if (!bios.size()) {
        log("Invalid BIOS File\n");
        m_biosLoaded = false;
        return;
    }

    log("BIOS Size: {}kb\n", bios.size() / 1024);
    log("BIOS SHA1 Checksum: {}\n", hash);

    std::memcpy(m_mem.m_bios, bios.data(), bios.size());
    m_cpu.fetch();
    m_biosLoaded = true;
}

void Emulator::reset() {
    isRunning = false;
    m_mem.reset();
    m_cpu.reset();
}
