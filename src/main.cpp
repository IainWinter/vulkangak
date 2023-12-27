#include "SDL.h"
#include "vulkan/vulkan.hpp"

#include "simulation_loop.h"

#include "asset/package.h"
#include "asset/image.h"
#include "asset/shader_program.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <functional>

#include "render/particle/particle_mesh.h"
#include "render/backend/imgui_loop.h"

#include "math/random.h"

#include "imgui.h"

struct CameraUBO {
    mat4 view;
    mat4 proj;
};

struct SmokeParticle {
    vec2 velocity;
    float damping;

    float avelocity;
    float adamping;

    float lifeTotal;
    float lifeCurrent;
    float lifeRatio;
};

int main() {
    AssetPackage package = loadPackage("./assets.bin");

    SimulationLoop* loop = new SimulationLoop();
    Window* window = new Window("VkTest", 1280, 720);
    RenderDevice* device = new RenderDevice(window, true);
    ImGuiLoop* imgui = device->newImGuiLoop();

    window->addEventListener([loop](SDL_Event event) {
        if (event.type == SDL_QUIT) {
            loop->shutdown();
        }
    });

    // each AssetPackage will have a list of all the sets needed for the shaders contained in it
    // this will allow the device to allocate what it needs to create the pools and set layouts
    // Store separate pools for each frame in flight so they can be reallocated when needed, this should only really
    // happen on a scene change, so its ok pause for stall for a few frames

    DescriptorBinding textureLayout;
    textureLayout.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureLayout.stages = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureLayout.arrayElementCount = 1;
    textureLayout.bindingLocation = 0;
    textureLayout.bindingSetLocation = 0;

    DescriptorBinding uboLayout;
    uboLayout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayout.stages = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayout.arrayElementCount = 1;
    uboLayout.bindingLocation = 1;
    uboLayout.bindingSetLocation = 0;

    DescriptorGroup* descriptor = device->newDescriptorGroup({ textureLayout, uboLayout });

    VulkanShaderSource shaderSource;
    shaderSource.vertex = package["test.vert"].binary;
    shaderSource.fragment = package["test.frag"].binary;
    shaderSource.pushConstants = {
        { VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) }
    };
    shaderSource.descriptorSetLayouts = {
        descriptor->getLayout(0)
    };
    shaderSource.vertexInputs = ParticleMesh<SmokeParticle>::getLayout();

    Shader* shader = device->newShader(shaderSource);
    UniformBuffer* camUBO = device->newUniformBuffer<CameraUBO>();

    ParticleMesh<SmokeParticle>* particleMesh = new ParticleMesh<SmokeParticle>(device, 500);

    ImageAsset img = package["test.image"];
    Image* image = device->newImage((u8*)img.pixels.data(), img.width, img.height, img.channels);
    ImageSampler* imageSampler = device->newImageSampler();

    for (u32 i = 0; i < device->getFrameSyncInfo().framesInFlight(); i++) {
        descriptor->write(i, 0, image, imageSampler);
    }

    float acc = 0.f;

    SimulationTick tick;
    while (loop->beginTick(&tick)) {
        window->pumpEvents();

        mat4 model = mat4(1.0f);//rotate(mat4(1.0f), tick.applicationTime * radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
        
        acc += tick.deltaTime;
        while (acc > .005f) {
            acc -= .005f;

            ParticleMesh<SmokeParticle>::Instance instance{};
            instance.color = vec4(1.f);
            instance.pos = vec3(-1.5f, 0.f, 0.f);

            SmokeParticle particle{};
            particle.velocity = random_vec2_min_max(3, -4, 10, 4);
            particle.damping = random_float_min_add(3, 3);
            particle.lifeTotal = random_float_min_add(1, 1);
            particle.avelocity = random_float_centered_extent(5);

            particleMesh->add(instance, particle);
        }

        particleMesh->update([&](u32 index, ParticleMesh<SmokeParticle>::Instance& instance, SmokeParticle& particle) {
            particle.lifeCurrent += tick.deltaTime;
            particle.lifeRatio = particle.lifeCurrent / particle.lifeTotal;

            if (particle.lifeRatio >= 1.f) {
                particleMesh->remove(index);
            }

            instance.pos += vec3(particle.velocity * tick.deltaTime, 0.f);
            particle.velocity *= damping(tick.deltaTime, particle.damping);

            instance.scale   = lerp(vec2(.01f), vec2(.4f), particle.lifeRatio);
            instance.color.a = lerp(0.4f, 0.f, particle.lifeRatio);
        });

        particleMesh->commit();

        VulkanFrameImage frame;
        if (device->waitBeginFrame(&frame)) {
            particleMesh->sendToDevice();

            float aspect = frame.width / (float)frame.height;
            float width = 2 * aspect;

            CameraUBO ubo{};
            ubo.view = mat4(1.f);
            ubo.proj = glm::ortho(-width/2, width/2, -1.f, 1.f, -1.f, 1.f);

            camUBO->setData(&ubo);
            descriptor->write(frame.frameIndex, 1, camUBO);
            
            device->beginScreenRenderPass();

            CommandBuffer& cmd = *frame.commandBuffer;

            cmd.setShader(shader);
            cmd.pushConstant(shader, 0, &model);
            cmd.bindDescriptorSet(shader, descriptor->getSet(0));

            particleMesh->draw(cmd);

            imgui->beginFrame();
            {
                if (ImGui::Begin("Test")) {
                    ImGui::Text("This is a string");
                    static vec4 c;
                    ImGui::ColorPicker4("Color", (float*)&c);
                }
                ImGui::End();
            }
            imgui->submitFrame(cmd.m_commandBuffer);
            
            vkCmdEndRenderPass(cmd.m_commandBuffer);
            vkEndCommandBuffer(cmd.m_commandBuffer);

            device->submitFrame();
        }
    }

    device->waitUntilIdle();

    delete particleMesh;
    delete descriptor;
    delete camUBO;
    delete image;
    delete imageSampler;
    delete shader;
    delete imgui;
    delete device;
    delete window;
    delete loop;
}