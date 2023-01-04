#pragma once
#include <SFML/Graphics.hpp>

#include "disassembly.hpp"
#include "emulator.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "logger.hpp"
#include "regviewer.hpp"

class GUI {
    sf::RenderWindow window;
    sf::Clock deltaClock;
    sf::Texture display;
    Emulator& emulator;

  public:
    GUI(Emulator& emulator);

    void update();                             // Update the GUI
    bool isOpen() { return window.isOpen(); }  // Shows if the GUI window has been closed or not

  private:
    void showMenuBar();
    void showDisplay();
    void drawGUI();

    bool m_showDemo = false;

    Disassembly m_disassembly{emulator};
    RegViewer m_regviewer{emulator};
};