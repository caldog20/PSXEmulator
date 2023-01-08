//
// Created by Caleb Yates on 1/4/23.
//

#include "disassembly.hpp"

#include "capstone/capstone.h"
#include "fmt/format.h"
#include "fmt/printf.h"

void Disassembly::draw() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Disassembly View", &m_draw)) {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Disassemble BIOS")) {
        m_codeSize = disassembleBios();
        Helpers::log("Disassembling BIOS\n");
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        clear();
    }

    ImGui::SameLine();
    ImGui::Text("Code size: %.2fKB", (double)m_codeSize / 1024);

    ImGui::Separator();

    ImGui::BeginChild("DisassembledInstructions", ImVec2(0, -4), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
    for (auto& item : m_ins) {
        ImGui::TextUnformatted(item.c_str());
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::SetItemDefaultFocus();
    ImGui::End();
}

size_t Disassembly::disassembleBios() {
    csh handle;
    cs_insn* insn;
    size_t count;

    const uint8_t* buffer = m_emulator.m_mem.m_bios;
    const size_t bufferSize = 512 * 1024;

    if (buffer == nullptr) {
        Helpers::log("Invalid pointer to BIOS Memory\n");
        return 0;
    }

    if (cs_open(CS_ARCH_MIPS, CS_MODE_32, &handle) != CS_ERR_OK) {
        Helpers::log("Disassembler Error: Failed to initialize Capstone.\n");
        return 0;
    }

    cs_option(handle, CS_OPT_SKIPDATA, CS_OPT_ON);
    count = cs_disasm(handle, buffer, bufferSize, 0xbfc00000, 0, &insn);

    if (count > 0) {
        for (size_t j = 0; j < count; j++) {
            std::string s =
                fmt::sprintf("%#010" PRIx64 ":\t\t%-12s\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
            m_ins.push_back(s);
        }
    } else {
        cs_close(&handle);
        Helpers::log("Error disassembling instructions\n");
        return 0;
    }
    size_t dcount = count * sizeof(u32);
    cs_free(insn, count);
    cs_close(&handle);
    return dcount;
}