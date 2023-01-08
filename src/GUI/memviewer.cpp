#include "memviewer.hpp"

#include <string>

#include "imgui.h"
#include "mem.hpp"

void MemViewer::draw() {
    static char search_addr[9] = "";

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
        switch (item_current) {
            case 0:
                m_index = 0xbfc00000;
                m_count = SETCOUNT(m_index);
                break;
            case 1:
                m_index = RAM_BASE;
                m_count = SETCOUNT(m_index);
                break;
            case 2:
                m_index = SCRATCHPAD_BASE;
                m_count = SETCOUNT(m_index);
                break;
            case 3:
                m_index = HWREG_BASE;
                m_count = SETCOUNT(m_index);
                break;
            case 4:
                m_index = PARAPORT_BASE;
                m_count = SETCOUNT(m_index);
                break;
            default:
                m_emulator.log("Unhandled memory region");
        }
        prev_sel = item_current;
    }

    ImGui::Checkbox("Show Memory", &m_showMemory);

    ImGui::SameLine();
    if (ImGui::Button("Next")) {
        m_index += INCREMENT;
        m_count = SETCOUNT(m_index);
    }
    ImGui::SameLine();
    if (ImGui::Button("Prev")) {
        m_index -= INCREMENT;
        m_count = SETCOUNT(m_index);
    }

    ImGui::InputText("Address", search_addr, sizeof(search_addr), ImGuiInputTextFlags_CharsHexadecimal);

    if (ImGui::Button("Search")) {
        clear();
        m_index = 0;
        m_count = 1;
        if (strlen(search_addr) != 0) {
            m_searchAddress = std::stoul(search_addr, nullptr, 16);
            m_searchRegion = static_cast<REGION>(item_current);
            m_search = true;
            m_showMemory = false;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
        clear();
        item_current = 0;
        m_index = 0xbfc00000;
        m_count = SETCOUNT(m_index);
        std::memset(search_addr, 0, 9);
    }

    ImGui::Separator();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -4), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));  // Tighten spacing

    if (m_search) {
        search(m_searchRegion, m_searchAddress);
    } else if (m_showMemory) {
        m_search = false;
        walkRegion(static_cast<REGION>(item_current));
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}

void MemViewer::walkRegion(REGION region) {
    switch (region) {
        case REGION::BIOS: {
            for (auto i = m_index; i < m_count; i += 4) {
                if (m_psxMem.BIOS.contains(i & 0x1fffffff)) {
                    ImGui::Text("0x%08x\t\t\t\t\t", i);
                    ImGui::SameLine();
                    u32 offset = m_psxMem.BIOS.offset(i & 0x1fffffff);
                    ImGui::Text("0x%08x", m_psxMem.read32(m_psxMem.m_bios, offset));
                }
            }
            break;
        }

        case REGION::RAM: {
            for (auto i = m_index; i < m_count; i += 4) {
                if (m_psxMem.RAM.contains(i & 0x1fffffff)) {
                    ImGui::Text("0x%08x\t\t\t\t\t", i);
                    ImGui::SameLine();
                    u32 offset = m_psxMem.RAM.offset(i & 0x1fffffff);
                    ImGui::Text("0x%08x", m_psxMem.read32(m_psxMem.m_ram, offset));
                }
            }
            break;
        }

        case REGION::SCRATCHPAD: {
            for (auto i = m_index; i < m_count; i += 4) {
                if (m_psxMem.SCRATCHPAD.contains(i & 0x1fffffff)) {
                    ImGui::Text("0x%08x\t\t\t\t\t", i);
                    ImGui::SameLine();
                    u32 offset = m_psxMem.SCRATCHPAD.offset(i & 0x1fffffff);
                    ImGui::Text("0x%08x", m_psxMem.read32(m_psxMem.m_scratch, offset));
                }
            }
            break;
        }

        case REGION::HWREG: {
            for (auto i = m_index; i < m_count; i += 4) {
                if (m_psxMem.HWREG.contains(i & 0x1fffffff)) {
                    ImGui::Text("0x%08x\t\t\t\t\t", i);
                    ImGui::SameLine();
                    u32 offset = m_psxMem.HWREG.offset(i & 0x1fffffff);
                    ImGui::Text("0x%08x", m_psxMem.read32(m_psxMem.m_hw, offset));
                }
            }
            break;
        }

        case REGION::PARA: {
            for (auto i = m_index; i < m_count; i += 4) {
                if (m_psxMem.PARAPORT.contains(i & 0x1fffffff)) {
                    ImGui::Text("0x%08x\t\t\t\t\t", i);
                    ImGui::SameLine();
                    u32 offset = m_psxMem.PARAPORT.offset(i & 0x1fffffff);
                    ImGui::Text("0x%08x", m_psxMem.read32(m_psxMem.m_para, offset));
                }
            }
            break;
        }

        default:
            return;
    }
}

void MemViewer::search(REGION region, u32 address) {
    switch (region) {
        case REGION::BIOS: {
            u32 hw_address = address & 0x1fffffff;
            if (m_psxMem.BIOS.contains(hw_address)) {
                auto offset = m_psxMem.BIOS.offset(hw_address);
                auto val = m_psxMem.read32(m_psxMem.m_bios, offset);
                ImGui::Text("0x%08x\t\t\t\t\t", address);
                ImGui::SameLine();
                ImGui::Text("0x%08x", val);
            }
            break;
        }

        case REGION::RAM: {
            u32 hw_address = address & 0x1fffffff;
            if (m_psxMem.RAM.contains(hw_address)) {
                auto offset = m_psxMem.RAM.offset(hw_address);
                auto val = m_psxMem.read32(m_psxMem.m_ram, offset);
                ImGui::Text("0x%08x\t\t\t\t\t", address);
                ImGui::SameLine();
                ImGui::Text("0x%08x", val);
            }
            break;
        }

        case REGION::SCRATCHPAD: {
            u32 hw_address = address & 0x1fffffff;
            if (m_psxMem.SCRATCHPAD.contains(hw_address)) {
                auto offset = m_psxMem.SCRATCHPAD.offset(hw_address);
                auto val = m_psxMem.read32(m_psxMem.m_scratch, offset);
                ImGui::Text("0x%08x\t\t\t\t\t", address);
                ImGui::SameLine();
                ImGui::Text("0x%08x", val);
            }
            break;
        }
        case REGION::HWREG: {
            u32 hw_address = address & 0x1fffffff;
            if (m_psxMem.HWREG.contains(hw_address)) {
                auto offset = m_psxMem.HWREG.offset(hw_address);
                auto val = m_psxMem.read32(m_psxMem.m_hw, offset);
                ImGui::Text("0x%08x\t\t\t\t\t", address);
                ImGui::SameLine();
                ImGui::Text("0x%08x", val);
            }
            break;
        }
        case REGION::PARA: {
            u32 hw_address = address & 0x1fffffff;
            if (m_psxMem.PARAPORT.contains(hw_address)) {
                auto offset = m_psxMem.PARAPORT.offset(hw_address);
                auto val = m_psxMem.read32(m_psxMem.m_para, offset);
                ImGui::Text("0x%08x\t\t\t\t\t", address);
                ImGui::SameLine();
                ImGui::Text("0x%08x", val);
            }
            break;
        }
        default:
            return;
    }
}
