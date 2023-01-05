#include "memviewer.hpp"

#include <string>

#include "imgui.h"
#include "mem.hpp"

void MemViewer::walkBios() {
    for (int i = 0; i < BIOS_SIZE; i += 4) {
        u32 val = *(u32*)(m_emulator.m_mem.m_bios.get() + i);
        m_bios.push_back(val);
    }
}

void MemViewer::walkRam() {
    for (int i = 0; i < RAM_SIZE; i += 4) {
        u32 val = *(u32*)(m_emulator.m_mem.m_ram.get() + i);
        m_ram.push_back(val);
    }
}

void MemViewer::walkHw() {
    for (int i = 0; i < HWREG_SIZE; i += 4) {
        u32 val = *(u32*)(m_emulator.m_mem.m_hw.get() + i);
        m_hw.push_back(val);
    }
}

void MemViewer::walkPara() {
    for (int i = 0; i < PARAPORT_SIZE; i += 4) {
        u32 val = *(u32*)(m_emulator.m_mem.m_para.get() + i);
        m_para.push_back(val);
    }
}
void MemViewer::walkScratch() {
    for (int i = 0; i < SCRATCHPAD_SIZE; i += 4) {
        u32 val = *(u32*)(m_emulator.m_mem.m_scratch.get() + i);
        m_scratch.push_back(val);
    }
}

void MemViewer::draw() {
    static char search_addr[9] = "";
    static u32 indexer = 0;
    static u32 count = 0;
    static u32 addr = 0;

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Mem Viewer", &m_draw)) {
        ImGui::End();
        return;
    }

    const char* items[] = {"BIOS", "RAM", "SCRATCHPAD", "HWREG", "PARA"};
    static int prev_sel = 0;
    static int item_current = 0;
    ImGui::Combo("Memory Region", &item_current, items, IM_ARRAYSIZE(items));

    if (prev_sel != item_current) {
        clear();
        prev_sel = item_current;
        indexer = addr = count = 0;
    }

    if (ImGui::Button("Walk Memory")) {
        clear();
        count = 0x40;

        switch (item_current) {
            case 0:
                addr = 0xbfc00000;
                walkBios();
                break;
            case 1:
                addr = RAM_BASE;
                walkRam();
                break;
            case 2:
                addr = SCRATCHPAD_BASE;
                walkScratch();
                break;
            case 3:
                addr = HWREG_BASE;
                walkHw();
                break;
            case 4:
                addr = PARAPORT_BASE;
                walkPara();
                break;

            default:
                return;
        }
    }

    ImGui::InputText("Address", search_addr, sizeof(search_addr), ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Search")) {
        clear();
        indexer = 0;
        count = 1;
        if (strlen(search_addr) != 0) {
            u32 address = std::stoul(search_addr, nullptr, 16);
            addr = address;
            REGION region = static_cast<REGION>(item_current);
            search(region, address);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        indexer = addr = count = 0;
        clear();
    }

    if (ImGui::Button("Next")) {
        indexer += 0x40;
        count += 0x40;
    }
    if (ImGui::Button("Prev")) {
        indexer -= 0x40;
        count -= 0x40;
    }
    if (ImGui::Button("Reset")) {
        indexer = addr = count = 0;
    }

    std::vector<u32>& m_walkedMem = m_bios;

    switch (item_current) {
        case 0:
            m_walkedMem = m_bios;
            ImGui::Text("Bios ROM");
            break;
        case 1:
            m_walkedMem = m_ram;
            ImGui::Text("RAM");
            break;
        case 2:
            m_walkedMem = m_scratch;
            ImGui::Text("SCRATCHPAD");
            break;
        case 3:
            m_walkedMem = m_hw;
            ImGui::Text("HWREG");
            break;
        case 4:
            m_walkedMem = m_para;
            ImGui::Text("Parallel Port");
            break;
        default:
            m_emulator.log("Unhandled memory region");
    }

    ImGui::Separator();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -4), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));  // Tighten spacing

    ImGui::Text("Address\t\t\t\t\t\tValue");
    for (int i = indexer; i < count; i++) {
        if (!m_walkedMem.size()) break;
        ImGui::Text("0x%08x\t\t\t\t\t", addr + i * 4);
        ImGui::SameLine();
        ImGui::Text("0x%08x", m_walkedMem[i]);
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}

void MemViewer::search(REGION region, u32 address) {
    switch (region) {
        case REGION::BIOS:
            m_emulator.log("BIOS REGION");
            address = address & 0x1fffffff;
            if (m_emulator.m_mem.BIOS.contains(address)) {
                u32 offset = m_emulator.m_mem.BIOS.offset(address);
                u32 val = *(u32*)(m_emulator.m_mem.m_bios.get() + offset);
                m_bios.push_back(val);
            }
            break;

        case REGION::RAM:
            m_emulator.log("RAM REGION");
            address = address & 0x1fffffff;
            if (m_emulator.m_mem.RAM.contains(address)) {
                u32 offset = m_emulator.m_mem.RAM.offset(address);
                u32 val = *(u32*)(m_emulator.m_mem.m_ram.get() + offset);
                m_ram.push_back(val);
            }
            break;
        case REGION::SCRATCHPAD:
            m_emulator.log("ScratchPad REGION");
            address = address & 0x1fffffff;
            if (m_emulator.m_mem.SCRATCHPAD.contains(address)) {
                u32 offset = m_emulator.m_mem.SCRATCHPAD.offset(address);
                u32 val = *(u32*)(m_emulator.m_mem.m_scratch.get() + offset);
                m_scratch.push_back(val);
            }
            break;
        case REGION::HWREG:
            m_emulator.log("HWREG REGION");
            address = address & 0x1fffffff;
            if (m_emulator.m_mem.HWREG.contains(address)) {
                u32 offset = m_emulator.m_mem.HWREG.offset(address);
                u32 val = *(u32*)(m_emulator.m_mem.m_hw.get() + offset);
                m_hw.push_back(val);
            }
            break;
        case REGION::PARA:
            m_emulator.log("Parallel REGION");
            address = address & 0x1fffffff;
            if (m_emulator.m_mem.PARAPORT.contains(address)) {
                u32 offset = m_emulator.m_mem.PARAPORT.offset(address);
                u32 val = *(u32*)(m_emulator.m_mem.m_para.get() + offset);
                m_para.push_back(val);
            }
            break;
        default:
            return;
    }
}
