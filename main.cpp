#include "SDL.h"
#include "vulkan/vulkan.hpp"

#include "render_device.h"
#include "simulation_loop.h"
#include "asset_compiler/package.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>

struct Vertex {
    vec2 pos;
    vec2 uv;
    vec3 color;

    static VulkanVertexLayout getLayout() {
        VkVertexInputBindingDescription description{};
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputAttributeDescription> attributes(3);
        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[0].offset = offsetof(Vertex, pos);

        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[1].offset = offsetof(Vertex, uv);

        attributes[2].binding = 0;
        attributes[2].location = 2;
        attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[2].offset = offsetof(Vertex, color);

        return { description, attributes };
    }
};

static const std::vector<Vertex> vertices = {
    // 0  2    
    // 1  3
    {{-0.5f, -0.5f}, {0, 1}, {1.0f, 1.0f, 1.0f}},
    {{-0.5f,  0.5f}, {0, 0}, {1.0f, 1.0f, 1.0f}},
    {{ 0.5f, -0.5f}, {1, 1}, {1.0f, 1.0f, 1.0f}},
    {{ 0.5f,  0.5f}, {1, 0}, {1.0f, 1.0f, 1.0f}}
};

static const std::vector<u32> indices = {
    0, 2, 1,
    1, 2, 3
};

struct CameraUBO {
    mat4 view;
    mat4 proj;
};

int main() {
    AssetPackage package = loadPackage("C:/dev/vkTestCpp/assets.bin");

    SimulationLoop* loop = new SimulationLoop();
    Window* window = new Window("VkTest", 500, 500);
    RenderDevice* device = new RenderDevice(window, true);

    // Create the image

    Asset img = package["test.image"];
    Image* image = device->newImage((u8*)img.binary.data(), img.getInt("width"), img.getInt("height"), img.getInt("channels"));
    ImageSampler* imageSampler = device->newImageSampler();
    
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

    for (u32 i = 0; i < device->getFrameSyncInfo().framesInFlight(); i++) {
        descriptor->write(i, 0, image, imageSampler);
    }

    VulkanShaderSource shaderSource;
    shaderSource.vertex = package["test.vert"].binary;
    shaderSource.fragment = package["test.frag"].binary;
    shaderSource.pushConstants = {
        { VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) }
    };
    shaderSource.descriptorSetLayouts = {
        descriptor->getLayout(0)
    };

    Shader* shader = device->newShader<Vertex>(shaderSource);
    VertexBuffer* vertexBuffer = device->newVertexBuffer<Vertex>(vertices);
    IndexBuffer* indexBuffer = device->newIndexBuffer(indices);
    UniformBuffer* camUBO = device->newUniformBuffer<CameraUBO>();

    SimulationTick tick;
    while (loop->beginTick(&tick)) {
        VulkanFrameImage frame;
        if (device->waitBeginFrame(&frame)) {
            VkCommandBuffer commandBuffer = frame.commandBuffer;

            mat4 model = rotate(mat4(1.0f), tick.applicationTime * radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
            
            float aspect = frame.width / (float)frame.height;
            float width = 2 * aspect;

            auto v = vertices;
            v[0].pos.x = 1 * sin(tick.applicationTime);
            vertexBuffer->setData(v.data());

            CameraUBO ubo{};
            ubo.view = mat4(1.f);
            ubo.proj = glm::ortho(-width/2, width/2, -1.f, 1.f, -1.f, 1.f);
            
            camUBO->setData(&ubo);
            descriptor->write(frame.frameIndex, 1, camUBO);
            
            device->beginScreenRenderPass();

            shader->use(commandBuffer);
            shader->pushConstant(commandBuffer, 0, &model);
            shader->bindDescriptorSet(commandBuffer, descriptor->getSet(0));

            vertexBuffer->bind(commandBuffer);
            indexBuffer->bind(commandBuffer);
            indexBuffer->draw(commandBuffer);

            vkCmdEndRenderPass(commandBuffer);

            device->submitFrame();
        }
    }

    device->waitUntilIdle();

    delete descriptor;
    delete camUBO;
    delete image;
    delete imageSampler;
    delete indexBuffer;
    delete vertexBuffer;
    delete shader;
    delete loop;
    delete device;
    delete window;
}