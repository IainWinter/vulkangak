#pragma once

#include "SDL.h"

class Window {
public:
    Window(const char* title, int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void waitViewable();

public:
    SDL_Window* m_sdlWindow;
};