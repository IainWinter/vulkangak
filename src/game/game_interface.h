#pragma once

class SimulationLoop;
class InputMap;
class Window;
class RenderDevice;
class ImGuiLoop;
struct SimulationTick;
struct VulkanFrameImage;

class GameInterface {
public:
    virtual ~GameInterface() {}

    virtual void mapInput(InputMap* input) = 0;
    virtual void createStaticDeviceResources(RenderDevice* device) = 0;
    virtual void createGameState() = 0;

    virtual void simulationTick(SimulationTick tick, InputMap* input) = 0;
    virtual void render(SimulationTick tick, VulkanFrameImage frame, RenderDevice* device, ImGuiLoop* imgui) = 0;

    virtual void destroyGameState() = 0;
    virtual void destroyStaticDeviceResources(RenderDevice* device) = 0;
};