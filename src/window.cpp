#include "window.h"
#include "typedef.h"

Window::Window(const char* title, int width, int height) {
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    m_sdlWindow = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        width, height, 
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
    );
}

Window::~Window() {
    SDL_DestroyWindow(m_sdlWindow);
}

void Window::addEventListener(std::function<void(SDL_Event)> listener) {
    listeners.push_back(listener);
}

void Window::pumpEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        for (auto& listener : listeners) {
            listener(event);
        }
    }
}

void Window::waitViewable() {
    // This code is so gross

    SDL_Event event;
    bool pause;

    do {
        int w, h;
        SDL_GetWindowSize(m_sdlWindow, &w, &h);
        u32 flags = SDL_GetWindowFlags(m_sdlWindow);
        pause = w < 10 || h < 10 || (flags & SDL_WINDOW_MINIMIZED); // under 10 for odd resize values, should be 0
    }
    while (pause && SDL_WaitEvent(&event));
}