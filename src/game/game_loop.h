#pragma once

#include "game_interface.h"

class GameLoop {
public:
    GameLoop(GameInterface* game);
    ~GameLoop();

    void run();

private:
    SimulationLoop* m_loop;
    InputMap* m_input;
    Window* m_window;
    RenderDevice* m_device;
    ImGuiLoop* m_imgui;

    // doesn't own
    GameInterface* m_game;
};