#include "simulation_loop.h"
#include "SDL.h"

SimulationLoop::SimulationLoop() {
    m_startTime = std::chrono::high_resolution_clock::now();
    m_lastTime = m_startTime;
    m_timeAccumulated = 0.f;
}

bool SimulationLoop::beginTick(SimulationTick* tick) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    bool running = true;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                running = false;
            }
        }
    }

    float appTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_startTime).count();
    float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_lastTime).count();
    
    m_timeAccumulated += dt;
    m_lastTime = currentTime;

    tick->applicationTime = appTime;
    tick->deltaTime = dt;
    tick->time = m_timeAccumulated;

    return running;
}