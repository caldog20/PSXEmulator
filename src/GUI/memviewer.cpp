#include "memviewer.hpp"

#include "imgui.h"
#include "mem.hpp"

void MemViewer::walkBios() {
    for (int i = 0; i < BIOS_SIZE; i += 4) {
        auto b0 = m_emulator.m_mem.m_bios[i];
        auto b1 = m_emulator.m_mem.m_bios[i + 1];
        auto b2 = m_emulator.m_mem.m_bios[i + 2];
        auto b3 = m_emulator.m_mem.m_bios[i + 3];

        u32 val = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
        m_biosMem.push_back(val);
    }
}

void MemViewer::walkRam() {
    for (int i = 0; i < RAM_SIZE; i += 4) {
        auto b0 = m_emulator.m_mem.m_ram[i];
        auto b1 = m_emulator.m_mem.m_ram[i + 1];
        auto b2 = m_emulator.m_mem.m_ram[i + 2];
        auto b3 = m_emulator.m_mem.m_ram[i + 3];

        u32 val = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
        m_mainMem.push_back(val);
    }
}

void MemViewer::draw() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Mem Viewer", &m_draw)) {
        ImGui::End();
        return;
    }

    const char* items[] = {"BIOS", "RAM"};
    static int prev_sel = 0;
    static int item_current = 0;
    ImGui::Combo("Memory Region", &item_current, items, IM_ARRAYSIZE(items));

    if (prev_sel != item_current) {
        clear();
        prev_sel = item_current;
    }

    if (ImGui::Button("Walk Memory")) {
        clear();

        switch (item_current) {
            case 0:
                walkBios();
                break;
            case 1:
                walkRam();
                break;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear")) clear();

    std::vector<u32>& m_walkedMem = m_mainMem;
    u32 addr{};
    switch (item_current) {
        case 0:
            addr = 0xbfc00000;
            m_walkedMem = m_biosMem;
            ImGui::Text("Bios ROM");
            break;
        case 1:
            addr = 0x00000000;
            m_walkedMem = m_mainMem;
            ImGui::Text("RAM");
            break;
        default:
            m_emulator.log("Unhandled memory region");
    }

    ImGui::Separator();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -4), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));  // Tighten spacing

    ImGui::Text("Address\t\t\t\t\t\tValue");
    for (auto& item : m_walkedMem) {
        ImGui::Text("0x%08x\t\t\t\t\t", addr);
        ImGui::SameLine();
        ImGui::Text("0x%08x", item);
        addr += 4;
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}
