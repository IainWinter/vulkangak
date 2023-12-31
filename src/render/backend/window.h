#pragma once

#include "SDL.h"

#include <functional>
#include <vector>

using SDLEventListener = std::function<void(SDL_Event)>;

class Window {
public:
    Window(const char* title, int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void addEventListener(std::function<void(SDL_Event)> listener);

    // Push all events to the listeners
    void pumpEvents();

    // Spin loop until the window has a viewable size (valid for the swap chai)
    // When the window is minimized it has a size of 0, 0
    void waitViewable();

public:
    SDL_Window* m_sdlWindow;

    // These are updated by pumpEvents
    float m_width;
    float m_height;

private:
    std::vector<SDLEventListener> listeners;
};