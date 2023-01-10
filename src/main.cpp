#include "emulator.hpp"
#include "gui.hpp"
#include "BitField.hpp"
#include "fmt/format.h"

int main() {
    fmt::print("Start of Main\n");

    auto emulator = Emulator();  // Initialize emulator object
    auto gui = GUI(emulator);    // Initialize GUI

    while (gui.isOpen())  // Main loop: This is run while the window hasn't been closed
        gui.update();     // GUI update handles rendering the GUI, and also running a frame if the emulator is running

    ImGui::SFML::Shutdown();  // Shut down ImGui SFML if we're down
}