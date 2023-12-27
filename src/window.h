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

private:
    void sendEventToListeners(SDL_Event event);

public:
    SDL_Window* m_sdlWindow;

private:
    std::vector<SDLEventListener> listeners;
};