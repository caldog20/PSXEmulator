#include "regviewer.hpp"

#include "imgui.h"

void RegViewer::draw() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Reg Viewer", &m_draw)) {
        ImGui::End();
        return;
    }

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable |
                                   ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                   ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_NoHostExtendX;

    ImGui::Text("Registers");

    if (ImGui::BeginTable("GPR", 2, flags)) {
        ImGui::TableSetupColumn("GPR");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();
        for (int i = 0; i < 32; i++) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(std::to_string(i).c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("0x%08x", m_emulator.m_cpu.m_regs.gpr.r[i]);
        }

        ImGui::EndTable();
    }

    ImGui::SameLine();

    if (ImGui::BeginTable("SPR", 2, flags)) {
        ImGui::TableSetupColumn("SPR");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("hi");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", m_emulator.m_cpu.m_regs.spr.hi);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("lo");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", m_emulator.m_cpu.m_regs.spr.lo);

        ImGui::EndTable();
    }

    ImGui::SameLine();
    if (ImGui::BeginTable("COPR", 2, flags)) {
        ImGui::TableSetupColumn("COPR");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (int i = 0; i < 32; i++) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(std::to_string(i).c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("0x%08x", m_emulator.m_cpu.m_regs.copr.r[i]);
        }

        ImGui::EndTable();
    }

    ImGui::End();
}
