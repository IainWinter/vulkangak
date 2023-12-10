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

// getLayout functions make no sense as they are tied to the shader
// not the vertex

struct Vertex {
    vec2 pos;
    vec2 uv;
    vec4 color;

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
        attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[2].offset = offsetof(Vertex, color);

        return { description, attributes };
    }
};

struct ModelMatrixVertex {
    vec3 position;
    vec2 scale;
    float rotation;

    static VulkanVertexLayout getLayout() {
        VkVertexInputBindingDescription description{};
        description.binding = 1;
        description.stride = sizeof(ModelMatrixVertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        std::vector<VkVertexInputAttributeDescription> attributes(3);
        attributes[0].binding = 1;
        attributes[0].location = 3;
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].offset = offsetof(ModelMatrixVertex, position);

        attributes[1].binding = 1;
        attributes[1].location = 4;
        attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[1].offset = offsetof(ModelMatrixVertex, scale);

        attributes[2].binding = 1;
        attributes[2].location = 5;
        attributes[2].format = VK_FORMAT_R32_SFLOAT;
        attributes[2].offset = offsetof(ModelMatrixVertex, rotation);

        return { description, attributes };
    }
};

static const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {0, 1}, {1.0f, 1.0f, 1.0f, .1f}}, // 0  2
    {{-0.5f,  0.5f}, {0, 0}, {1.0f, 1.0f, 1.0f, .1f}}, // 1  3
    {{ 0.5f, -0.5f}, {1, 1}, {1.0f, 1.0f, 1.0f, .1f}},
    {{ 0.5f,  0.5f}, {1, 0}, {1.0f, 1.0f, 1.0f, .1f}}
};

static const std::vector<u32> indices = {
    0, 2, 1,
    1, 2, 3
};

static std::vector<ModelMatrixVertex> insts = {};

struct CameraUBO {
    mat4 view;
    mat4 proj;
};

struct SmokeParticle {
    vec2 velocity;
    float life;
};

float random() {
    return rand() / (float)RAND_MAX;
}

vec2 lerp(vec2 a, vec2 b, float w) {
    return a * (1.f - w) + b * w;
}

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
    shaderSource.vertexInputs = {
        Vertex::getLayout(),
        ModelMatrixVertex::getLayout()
    };

    Shader* shader = device->newShader(shaderSource);
    UniformBuffer* camUBO = device->newUniformBuffer<CameraUBO>();

    VertexBuffer* vertexBuffer = device->newVertexBuffer<Vertex>(vertices);

    insts.resize(100);
    VertexBuffer* instanceBuffer = device->newVertexBuffer<ModelMatrixVertex>(insts);
    IndexBuffer* indexBuffer = device->newIndexBuffer(indices);

    // Create the image

    Asset img = package["test.image"];
    Image* image = device->newImage((u8*)img.binary.data(), img.getInt("width"), img.getInt("height"), img.getInt("channels"));
    ImageSampler* imageSampler = device->newImageSampler();

    for (u32 i = 0; i < device->getFrameSyncInfo().framesInFlight(); i++) {
        descriptor->write(i, 0, image, imageSampler);
    }

    std::vector<ModelMatrixVertex> particles;
    std::vector<SmokeParticle> particleInfo;

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
                particles.push_back({vec3(0.f), vec2(1.f), 0.f});

                SmokeParticle p;
                p.velocity = vec2(random() * 2.f - 1.f, random()) / vec2(3, -1);
                p.life = random() + 1.f;

                particleInfo.push_back(p);
            }

            for (int i = 0; i < particles.size(); i++) {
                ModelMatrixVertex& v = particles[i];
                v.position += vec3(particleInfo[i].velocity * tick.deltaTime, 0.f);
                v.scale = lerp(v.scale, vec2(0.f), tick.deltaTime);

                particleInfo[i].life -= tick.deltaTime;

                if (particleInfo[i].life < 0) {
                    particles.at(i) = particles.back();
                    particles.pop_back();

                    particleInfo.at(i) = particleInfo.back();
                    particleInfo.pop_back();

                    i--;
                }
            }

            if (particleInfo.size() > 0) {
                instanceBuffer->setData(0, sizeof(ModelMatrixVertex) * particles.size(), particles.data());
            }

            camUBO->setData(&ubo);
            descriptor->write(frame.frameIndex, 1, camUBO);
            
            device->beginScreenRenderPass();

            CommandBuffer& cmd = *frame.commandBuffer;

            cmd.setShader(shader);
            cmd.pushConstant(shader, 0, &model);
            cmd.bindDescriptorSet(shader, descriptor->getSet(0));

            cmd.drawInstanced(particleInfo.size(), indexBuffer, {vertexBuffer, instanceBuffer});

            vkCmdEndRenderPass(cmd.m_commandBuffer);
            vkEndCommandBuffer(cmd.m_commandBuffer);

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
    delete instanceBuffer;
    delete shader;
    delete loop;
    delete device;
    delete window;
}