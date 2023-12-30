#pragma once

#include "simulation_tick.h"
#include <chrono>

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