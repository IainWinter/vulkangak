#pragma once

#include <chrono>

struct SimulationTick {
    // The time since the app was run
    float applicationTime;

    // The time accumulated from all ticks
    float time;

    // The time between ticks
    float deltaTime;

    // The current simulation frame
    int tick;
};

class SimulationLoop {
public:
    SimulationLoop();

    bool beginTick(SimulationTick* tick);

    void shutdown();

private:
    std::chrono::high_resolution_clock::time_point m_lastTime;
    std::chrono::high_resolution_clock::time_point m_startTime;

    int m_tick;
    float m_timeAccumulated;
    bool m_running;
};