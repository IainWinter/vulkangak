#pragma once

#include <chrono>

struct SimulationTick {
    // The time since the app was run
    float applicationTime;

    // The time accumulated from all ticks
    float time;

    // The time between ticks
    float deltaTime;
};

class SimulationLoop {
public:
    SimulationLoop();

    bool beginTick(SimulationTick* tick);

private:
    std::chrono::high_resolution_clock::time_point m_lastTime;
    std::chrono::high_resolution_clock::time_point m_startTime;

    float m_timeAccumulated;
};