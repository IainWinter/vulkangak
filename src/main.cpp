#include "simulation_loop.h"
#include "imgui.h"
#include "input.h"

#include "asset/package.h"
#include "asset/image.h"
#include "asset/shader_program.h"

#include "render/particle/particle_mesh.h"
#include "render/line/line_mesh.h"
#include "render/lens.h"

#include "math/random.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_map>
#include <functional>

struct CameraUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
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

// class SmokeParticleSystem {
//     struct SmokeParticle {
//         vec2 velocity;
//         float damping;

//         float avelocity;
//         float adamping;

//         float lifeTotal;
//         float lifeCurrent;
//         float lifeRatio;
//     };

//     SmokeParticleSystem(RenderDevice* device) {
//         m_mesh = new ParticleMesh<SmokeParticle>(device, 500);
//     }

//     ~SmokeParticleSystem() {
//         delete m_mesh;
//     }

//     void update(float dt) {

//     }

// private:
//     ParticleMesh<SmokeParticle>* m_mesh;
// };


struct LineShaderPushConstants {
    mat4 model;
    float totalLength;
};

struct Arc {
    vec2 pos = random_vec2_centered_extent(2, 2);
    vec2 vel = random_vec2_circle();
    vec2 acc = vec2(0, 0);

    vec2 lastpos = {};

    LineMesh* lineMesh;

    Arc(RenderDevice* device) {
        lineMesh = new LineMesh(device);
        lineMesh->add(pos);

        lastpos = pos;
    }

    ~Arc() {
        delete lineMesh;
    }

    void update(float dt) {
        pos += vel * dt;
        vel += acc * dt;
        vel *= damping(dt, 0.3f);

        if (distance(lastpos, pos) > .01f) {
            lastpos = pos;
            lineMesh->add(pos);
        }

        lineMesh->points().back().point = pos;
    }
};

int main() {
    AssetPackage package = loadPackage("./assets.bin");

    SimulationLoop* loop = new SimulationLoop();
    Window* window = new Window("VkTest", 1280, 720);
    RenderDevice* device = new RenderDevice(window, true);
    ImGuiLoop* imgui = device->newImGuiLoop();

    InputMap* input = new InputMap();

    CameraLens lens = CameraLens::Orthographic(4, 16.f/9.f, -1, 1);

    input->CreateAxis("Mouse Position")
        .Map(MOUSE_POS_X, vec2(1, 0))
        .Map(MOUSE_POS_Y, vec2(0, 1));

    window->addEventListener([loop](SDL_Event event) {
        if (event.type == SDL_QUIT) {
            loop->shutdown();
        }
    });

    window->addEventListener([window, input](SDL_Event event) {
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
    });

    UniformBuffer* camUBO = device->newUniformBuffer<CameraUBO>();

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

    ImageAsset img = package["test.image"];
    Image* image = device->newImage((u8*)img.pixels.data(), img.width, img.height, img.channels);
    ImageSampler* imageSampler = device->newImageSampler();

    for (u32 i = 0; i < device->getFrameSyncInfo().framesInFlight(); i++) {
        descriptor->write(i, 0, image, imageSampler);
    }

    // Create particle shader

    VulkanShaderSource particleShaderSource;
    particleShaderSource.vertex = package["particle.vert"].binary;
    particleShaderSource.fragment = package["particle.frag"].binary;
    particleShaderSource.pushConstants = {{ VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) }};
    particleShaderSource.descriptorSetLayouts = {descriptor->getLayout(0)};
    particleShaderSource.vertexInputs = ParticleMesh<SmokeParticle>::getLayout();

    Shader* particleShader = device->newShader(particleShaderSource);
    ParticleMesh<SmokeParticle>* particleMesh = new ParticleMesh<SmokeParticle>(device, 500);

    // Create line shader

    VulkanShaderSource lineShaderSource;
    lineShaderSource.vertex = package["line.vert"].binary;
    lineShaderSource.fragment = package["line.frag"].binary;
    lineShaderSource.pushConstants = {{ VK_SHADER_STAGE_VERTEX_BIT, sizeof(LineShaderPushConstants) }};
    lineShaderSource.descriptorSetLayouts = {descriptor->getLayout(0)};
    lineShaderSource.vertexInputs = LineMesh::getLayout();

    Shader* lineShader = device->newShader(lineShaderSource);

    std::vector<Arc*> arcs = {
        new Arc(device)
    };

    for (int i = 0; i < 100; i++) {
        arcs.push_back(new Arc(device));
    }

    float acc = 0.f;

    SimulationTick tick;
    while (loop->beginTick(&tick)) {
        window->pumpEvents();
        input->UpdateStates(tick.deltaTime);

        mat4 model = mat4(1.0f);
        vec2 mousePos = lens.ScreenToWorld2D(input->GetAxis("Mouse Position"));

        acc += tick.deltaTime;
        while (acc > .005f) {
            acc -= .005f;

            ParticleMesh<SmokeParticle>::Instance instance{};
            instance.color = vec4(1.f);
            instance.pos = vec3(mousePos, 0.f);

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

        for (int i = 1; i < arcs.size(); i++) {
            Arc* arc = arcs[i];

            vec2 dir = arcs[0]->pos - arc->pos;
            float dist = length(dir);

            arc->acc = dir / (dist * dist) * 10.f;

            arc->update(tick.deltaTime);
        }

        arcs[0]->pos = mousePos;
        arcs[0]->update(tick.deltaTime);

        VulkanFrameImage frame;
        if (device->waitBeginFrame(&frame)) {
            float aspect = frame.width / (float)frame.height;
            float width = 2 * aspect;

            lens.aspect = window->m_width / window->m_height;

            CameraUBO ubo{};
            ubo.view = lens.GetViewMatrix();
            ubo.proj = lens.GetProjectionMatrix();
            ubo.viewProj = ubo.proj * ubo.view;
            
            camUBO->setData(&ubo);
            descriptor->write(frame.frameIndex, 1, camUBO);
            
            device->beginScreenRenderPass();

            CommandBuffer& cmd = *frame.commandBuffer;

            cmd.setShader(particleShader);
            cmd.pushConstant(particleShader, 0, &model);
            cmd.bindDescriptorSet(particleShader, descriptor->getSet(0));

            particleMesh->sendToDevice();
            particleMesh->draw(cmd);

            cmd.setShader(lineShader);
            cmd.bindDescriptorSet(lineShader, descriptor->getSet(0));

            LineShaderPushConstants linePush{};
            linePush.model = model;

            for (Arc* arc : arcs) {
                linePush.totalLength = arc->lineMesh->totalLength();
                cmd.pushConstant(lineShader, 0, &linePush);

                arc->lineMesh->sendToDevice();
                arc->lineMesh->draw(cmd);
            }

            imgui->beginFrame();
            {
                if (ImGui::Begin("Test")) {
                    ImGui::Text("This is a string");

                    ImGui::Text("%f fps", 1.f / tick.deltaTime);

                    ImGui::Text("Input map");
                    ImGui::Text("Mouse pos: %f, %f", input->GetRawState(MOUSE_POS_X), input->GetRawState(MOUSE_POS_Y));
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

    delete particleShader;
    delete particleMesh;

    delete lineShader;

    for (Arc* arc : arcs) {
        delete arc;
    }

    delete image;
    delete imageSampler;

    delete descriptor;
    delete camUBO;
    
    delete imgui;
    delete device;
    delete window;
    delete loop;
}