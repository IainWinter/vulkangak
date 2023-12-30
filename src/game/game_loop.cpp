#include "game_loop.h"

#include "simulation_loop.h"
#include "input.h"

#include "render/backend/render_device.h"
#include "render/backend/imgui_loop.h"

#include "containers/defer_delete.h"

static void onQuitEvent(SimulationLoop* loop, SDL_Event event);
static void onInputEvent(Window* window, InputMap* input, SDL_Event event);

GameLoop::GameLoop(GameInterface* game) 
    : m_game (game)
{
    m_loop = new SimulationLoop();
    m_input = new InputMap();

    m_window = new Window("VkTest", 1280, 720);
    m_device = new RenderDevice(m_window, true);
    m_imgui = m_device->newImGuiLoop();

    m_window->addEventListener([this](SDL_Event event) { onQuitEvent(m_loop, event); });
    m_window->addEventListener([this](SDL_Event event) { onInputEvent(m_window, m_input, event); });

    m_game->mapInput(m_input);
    m_game->createStaticDeviceResources(m_device);
    m_game->createGameState();
}

GameLoop::~GameLoop() {
    delete m_imgui;
    delete m_device;
    delete m_window;
    delete m_input;
    delete m_loop;
}

void GameLoop::run() {
    SimulationTick tick;
    while (m_loop->beginTick(&tick)) {
        m_window->pumpEvents();
        m_input->UpdateStates(tick.deltaTime);

        m_game->simulationTick(m_input, tick);

        VulkanFrameImage frame;
        if (m_device->waitBeginFrame(&frame)) {
            m_game->render(frame, m_device);
        }

        // wait till after render
        delete_defer_flush();
    }

    m_device->waitUntilIdle();

    delete_defer_flush_now();

    m_game->destroyGameState();
    m_game->destroyStaticDeviceResources(m_device);
}

static void onQuitEvent(SimulationLoop* loop, SDL_Event event) {
    if (event.type == SDL_QUIT) {
        loop->shutdown();
    }
}

static void onInputEvent(Window* window, InputMap* input, SDL_Event event) {
    switch (event.type) {
        // Major issue with sdl2 is that the mouse is given by int

        case SDL_MOUSEMOTION: {
            float x = (float)event.motion.x / window->m_width;
            float y = (float)event.motion.y / window->m_height;

            float vel_x = (float) event.motion.xrel;
            float vel_y = (float)-event.motion.yrel;

            input->SetState(MOUSE_POS_X, x);
            input->SetState(MOUSE_POS_Y, y);

            input->SetState(MOUSE_VEL_X, vel_x);
            input->SetState(MOUSE_VEL_Y, vel_y);

            break;
        }

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN: {
            float x = (float)event.button.x / window->m_width;
            float y = (float)event.button.y / window->m_height;

            bool state = event.button.state == SDL_PRESSED;

            MouseInput mouseInput;

            switch (event.button.button) {
                case SDL_BUTTON_LEFT:   mouseInput = MOUSE_LEFT; break;
                case SDL_BUTTON_MIDDLE: mouseInput = MOUSE_MIDDLE; break;
                case SDL_BUTTON_RIGHT:  mouseInput = MOUSE_RIGHT; break;
                case SDL_BUTTON_X1:     mouseInput = MOUSE_X1; break;
                case SDL_BUTTON_X2:     mouseInput = MOUSE_X2; break;
                default: throw nullptr;
            }
            
            input->SetState(mouseInput, state);

            input->SetState(MOUSE_POS_X, x);
            input->SetState(MOUSE_POS_Y, y);

            input->SetState(MOUSE_VEL_X, 0);
            input->SetState(MOUSE_VEL_Y, 0);

            break;
        }

        case SDL_MOUSEWHEEL: {
            float flip = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1.f : -1.f;
            float vx = event.wheel.preciseX * flip;
            float vy = event.wheel.preciseY * flip;

            input->SetState(MOUSE_VEL_WHEEL_X, vx);
            input->SetState(MOUSE_VEL_WHEEL_X, vy);

            break;
        }

        case SDL_KEYUP:
        case SDL_KEYDOWN: {
            KeyboardInput scancode = (KeyboardInput)event.key.keysym.scancode;
            bool state = (bool)event.key.state;

            input->SetState(scancode, state);
            break;
        }
    }
}