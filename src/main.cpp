#include "SDL.h"
#include "vulkan/vulkan.hpp"

#include "simulation_loop.h"
#include "asset_compiler/package.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <functional>

#include "render/particle_mesh.h"

struct CameraUBO {
    mat4 view;
    mat4 proj;
};

struct SmokeParticle {
    vec2 velocity;
    float life;
};

int main() {
    AssetPackage package = loadPackage("C:/dev/vkTestCpp/assets.bin");

    SimulationLoop* loop = new SimulationLoop();
    Window* window = new Window("VkTest", 500, 500);
    RenderDevice* device = new RenderDevice(window, true);
    
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

    ParticleMesh<SmokeParticle>* particleMesh = new ParticleMesh<SmokeParticle>(device, 100);

    // Create the image

    Asset img = package["test.image"];
    Image* image = device->newImage((u8*)img.binary.data(), img.getInt("width"), img.getInt("height"), img.getInt("channels"));
    ImageSampler* imageSampler = device->newImageSampler();

    for (u32 i = 0; i < device->getFrameSyncInfo().framesInFlight(); i++) {
        descriptor->write(i, 0, image, imageSampler);
    }

    float acc = 0.f;

    SimulationTick tick;
    while (loop->beginTick(&tick)) {
        VulkanFrameImage frame;
        if (device->waitBeginFrame(&frame)) {
            mat4 model = mat4(1.0f);//rotate(mat4(1.0f), tick.applicationTime * radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
            
            float aspect = frame.width / (float)frame.height;
            float width = 2 * aspect;

            CameraUBO ubo{};
            ubo.view = mat4(1.f);
            ubo.proj = glm::ortho(-width/2, width/2, -1.f, 1.f, -1.f, 1.f);
            
            acc += tick.deltaTime;
            if (acc > .1) {
                acc = 0;

                ParticleMesh<SmokeParticle>::Instance instance{};
                instance.pos = vec3(0.f);
                instance.scale = vec2(1.f);
                instance.rotation = 0.f;

                SmokeParticle particle;
                particle.velocity = vec2(random() * 2.f - 1.f, random()) / vec2(3, -1);
                particle.life = random() + 1.f;

                particleMesh->add(instance, particle);
            }

            particleMesh->update([&](u32 index, ParticleMesh<SmokeParticle>::Instance& instance, SmokeParticle& particle) {
                instance.pos += vec3(particle.velocity * tick.deltaTime, 0.f);
                instance.scale = lerp(instance.scale, vec2(1.f), tick.deltaTime);
                instance.color = lerp(instance.color, vec4(1, 1, 1, .4), tick.deltaTime); 

                particle.life -= tick.deltaTime;

                if (particle.life < 0) {
                    particleMesh->remove(index);
                }
            });

            particleMesh->commit();
            particleMesh->sendToDevice();

            camUBO->setData(&ubo);
            descriptor->write(frame.frameIndex, 1, camUBO);
            
            device->beginScreenRenderPass();

            CommandBuffer& cmd = *frame.commandBuffer;

            cmd.setShader(shader);
            cmd.pushConstant(shader, 0, &model);
            cmd.bindDescriptorSet(shader, descriptor->getSet(0));

            particleMesh->draw(cmd);

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
    delete loop;
    delete device;
    delete window;
}