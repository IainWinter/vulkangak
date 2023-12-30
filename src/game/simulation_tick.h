#pragma once

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