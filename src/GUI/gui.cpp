#include "gui.hpp"

#include <filesystem>  // For std::filesystem::path

#include "fmt/format.h"       // For fmt::print
#include "tinyfiledialogs.h"  // For file explorer

GUI::GUI(Emulator& emulator) : window(sf::VideoMode(1366, 768), "PSX Emulator"), emulator(emulator) {
    // window.setFramerateLimit(60);  // cap FPS to 60
    ImGui::SFML::Init(window);  // Init Imgui-SFML
    display.create(Emulator::width, Emulator::height);

    auto& io = ImGui::GetIO();                             // Set some ImGui options
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable navigation with keyboard
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Configure viewport feature
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
}

void GUI::update() {
    sf::Event event;

    while (window.pollEvent(event)) {  // Poll events
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed) window.close();
    }

    if (emulator.isRunning) emulator.runFrame();

    ImGui::SFML::Update(window, deltaClock.restart());  // Update imgui-sfml

    showMenuBar();  // Render GUI stuff
    showDisplay();

    if (m_showDemo) {
        ImGui::ShowDemoWindow();
    }

    if (m_disassembly.m_draw) {
        m_disassembly.draw();
    }

    if (m_regviewer.m_draw) {
        m_regviewer.draw();
    }

    if (emulator.m_logger.m_draw) {
        emulator.m_logger.draw();
    }

    if (m_debuginfo.m_draw) {
        m_debuginfo.draw();
    }

    if (m_memviewer.m_draw) {
        m_memviewer.draw();
    }

    drawGUI();
}

void GUI::showMenuBar() {
    static const char* romTypes[] = {"*.bin", "*.rom"};  // Some generic filetypes for ROMs, configure as you want

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {  // Show file selection dialog if open ROM button is pressed
            if (ImGui::MenuItem("Open ROM", nullptr)) {
                auto file = tinyfd_openFileDialog("Choose a ROM",  // File explorer window title
                                                  "",              // Default directory
                                                  2,               // Amount of file types
                                                  romTypes,        // Array of file types
                                                  "ROMs",          // File type description in file explorer window
                                                  0);

                if (file != nullptr) {  // Check if file dialog was canceled
                    const auto path = std::filesystem::path(file);
                    fmt::print("Opened file {}\n", path.string());
                }
            }

            if (ImGui::MenuItem("Load BIOS", nullptr)) {
                auto file = tinyfd_openFileDialog("Choose BIOS File",  // File explorer window title
                                                  "",                  // Default directory
                                                  2,                   // Amount of file types
                                                  romTypes,            // Array of file types
                                                  "BIOS",              // File type description in file explorer window
                                                  0);

                if (file != nullptr) {  // Check if file dialog was canceled
                    const auto path = std::filesystem::path(file);
                    emulator.loadBios(path.string());
                }
            }

            if (ImGui::MenuItem("Save state", nullptr)) fmt::print("Save state");

            if (ImGui::MenuItem("Load state", nullptr)) fmt::print("Load state");

            if (ImGui::MenuItem("Show ImGui Demo", nullptr, &m_showDemo))
                ;

            if (ImGui::MenuItem("Quit", nullptr)) {
                emulator.isRunning = false;
                window.close();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Emulation")) {
            if (ImGui::MenuItem("Step", nullptr)) emulator.step();
            ImGui::MenuItem("Run", nullptr, &emulator.isRunning);
            ImGui::MenuItem("Enable Logs", nullptr, &emulator.m_enableLog);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("Debug", nullptr, &m_debuginfo.m_draw);
            ImGui::MenuItem("Logs", nullptr, &emulator.m_logger.m_draw);
            ImGui::MenuItem("Disassembly", nullptr, &m_disassembly.m_draw);
            ImGui::MenuItem("Registers", nullptr, &m_regviewer.m_draw);
            ImGui::MenuItem("Memory", nullptr, &m_memviewer.m_draw);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

// TODO: Optimize this
// This is fairly slow, so for best measure if you don't want presentation to bottleneck emulation too much
// You should have the emulator on a separate thread from the GUI
void GUI::showDisplay() {
    if (ImGui::Begin("Display")) {
        const auto size = ImGui::GetContentRegionAvail();
        const auto scale_x = size.x / Emulator::width;
        const auto scale_y = size.y / Emulator::height;
        const auto scale = scale_x < scale_y ? scale_x : scale_y;

        display.update(emulator.framebuffer.data());
        sf::Sprite sprite(display);
        sprite.setScale(scale, scale);

        ImGui::Image(sprite);
        ImGui::End();
    }
}

void GUI::drawGUI() {
    window.clear(sf::Color(69, 137, 224, 1));  // Clear window with turquoise
    ImGui::SFML::Render(window);               // Render ImGui contents
    window.display();                          // Display ImGui contents
}
