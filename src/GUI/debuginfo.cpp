#include "debuginfo.hpp"

#include <string>

#include "imgui.h"
#include "mnemonics.hpp"

void DebugInfo::draw() {
    auto& curIns = m_emulator.m_cpu.m_instruction;
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Debug Info", &m_draw)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Diagnostic Info");

    if (ImGui::Button("Step")) m_emulator.step();
    ImGui::SameLine();
    if (ImGui::Button("Start")) m_emulator.isRunning = true;
    ImGui::SameLine();
    if (ImGui::Button("Stop")) m_emulator.isRunning = false;
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        m_emulator.reset();
        m_emulator.m_cpu.fetch();
    }

    ImGui::Text("Current PC: 0x%08x", m_emulator.m_cpu.m_regs.pc);
    ImGui::Text("Current Instruction: 0x%08x", curIns.ins);
    ImGui::Text("Current Opcode: 0x%08x", curIns.opcode);
    ImGui::Text("Jump PC: 0x%08x", m_emulator.m_cpu.m_regs.jumppc);
    //    ImGui::Text("Instruction Counter %lu", m_emulator.m_cpu->m_counter);
    //    ImGui::Text("Branch Delay Instruction %lu", m_emulator.m_cpu->m_bdCounter);
    //    ImGui::Text("Load Delay Instruction %lu", m_emulator.m_cpu->m_ldCounter);
    static char break_addr[9] = "";
    ImGui::InputText("Break Address", break_addr, sizeof(break_addr), ImGuiInputTextFlags_CharsHexadecimal);
    if (ImGui::Button("Set Break")) {
        u32 address = std::stoul(break_addr, nullptr, 16);
        m_emulator.m_breakPc = address;
        m_emulator.m_break = true;
    }

    ImGui::Checkbox("Break at address", &m_emulator.m_break);

    ImGui::NewLine();
    ImGui::Separator();
    ImGui::Text("Current Instruction Decoder");
    ImGui::Separator();

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable |
                                   ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                   ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_NoHostExtendX |
                                   ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;

    if (ImGui::BeginTable("Instruction", 2, flags)) {
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("mnemonic");
        ImGui::TableSetColumnIndex(1);
        const char* mnemonic;

        if (curIns.opcode == 0) {
            mnemonic = mnemonic_special[curIns.fn];
        } else {
            mnemonic = mnemonic_basic[curIns.opcode];
        }

        ImGui::Text("%s", mnemonic);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("instruction");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.ins);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("opcode");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.opcode);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("reg source");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.rs);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("reg target");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.rt);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("reg destination");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.rd);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("immediate");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.imm);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("immediate se");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.immse);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("function");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.fn);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("shift amount");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.sa);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("jump target");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("0x%08x", curIns.tar);

        ImGui::EndTable();
    }

    ImGui::End();
}