#pragma once

#include "log.h"

#include "game/simulation_tick.h"
#include "game/game_interface.h"
#include "game/input.h"

#include "render/backend/imgui_loop.h"
#include "imgui.h"

#include "render/line/line_mesh.h"
#include "render/particle/particle_mesh.h"
#include "render/lens.h"

#include "asset/package.h"
#include "asset/image.h"

#include "math/random.h"
#include "glm/mat4x4.hpp"

#include "containers/pop_back.h"
#include "containers/defer_delete.h"

struct CameraUBO {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
};

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

struct Orbital {
    vec2 center;
    vec2 vel;
    float mass;

    float lifetime;
    float life;

    std::vector<Arc*> arcs;

    Orbital(RenderDevice* device, vec2 center, vec2 velocity, float mass, float lifetime, int numArcs) {
        this->center = center;
        this->vel = velocity;
        this->mass = mass;
        this->lifetime = lifetime;
        this->life = 0.f;

        for (int i = 0; i < numArcs; i++) {
            arcs.push_back(new Arc(device));
        }
    }

    ~Orbital() {
        for (Arc* arc : arcs) {
            delete arc;
        }
    }

    void update(float dt) {
        life += dt;
        center += vel * dt;

        for (Arc* arc : arcs) {
            arc->acc = (center - arc->pos) * mass / (length(center - arc->pos) * length(center - arc->pos));
            arc->update(dt);
        }
    }
};

class TestGame : public GameInterface {
public:
    void mapInput(InputMap* input) override {
        input->CreateAxis("Mouse Position")
            .Map(MOUSE_POS_X, vec2(1, 0))
            .Map(MOUSE_POS_Y, vec2(0, 1));

        input->CreateAxis("Mouse Click")
            .MapButton(MOUSE_LEFT);
    }

    void createStaticDeviceResources(RenderDevice* device) override {
        AssetPackage package = loadPackage("./assets.bin");

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

        m_descriptor = device->newDescriptorGroup({ textureLayout, uboLayout });

        // Create particle shader

        VulkanShaderSource particleShaderSource;
        particleShaderSource.vertex = package["particle.vert"].binary;
        particleShaderSource.fragment = package["particle.frag"].binary;
        particleShaderSource.pushConstants = {{ VK_SHADER_STAGE_VERTEX_BIT, sizeof(mat4) }};
        particleShaderSource.descriptorSetLayouts = { m_descriptor->getLayout(0) };
        particleShaderSource.vertexInputs = ParticleMesh::getLayout();

        m_particleShader = device->newShader(particleShaderSource);
        m_particleMesh = new ParticleMesh(device, 13000);

        // Create line shader

        VulkanShaderSource lineShaderSource;
        lineShaderSource.vertex = package["line.vert"].binary;
        lineShaderSource.fragment = package["line.frag"].binary;
        lineShaderSource.pushConstants = {{ VK_SHADER_STAGE_VERTEX_BIT, sizeof(LineShaderPushConstants) }};
        lineShaderSource.descriptorSetLayouts = { m_descriptor->getLayout(0) };
        lineShaderSource.vertexInputs = LineMesh::getLayout();

        m_lineShader = device->newShader(lineShaderSource);

        // Camera

        m_cameraUBO = device->newUniformBuffer<CameraUBO>();

        // Load and write image to group

        ImageAsset img = package["test.image"];
        m_image = device->newImage((u8*)img.pixels.data(), img.width, img.height, img.channels);
        m_imageSampler = device->newImageSampler();

        for (u32 i = 0; i < device->getFrameSyncInfo().framesInFlight(); i++) {
            m_descriptor->writeImage(i, 0, m_image, m_imageSampler);
        }

        m_device = device;
    }

    void createGameState() override {
        m_lens = CameraLens::Orthographic(32, 16.f/9.f, -10, 10);

        ParticleSpawner spawner{};
        spawner.particle.lifetime = 1.8f / 4.f;
        
        spawner.instance.rotation.z = PI / 2.f;

        spawner.particle.enableScalingByLife = true;
        spawner.particle.initialScale = vec2(0.12f);
        spawner.particle.finalScale = vec2(0);
        spawner.particle.factorScale = 4.056f;
        
        spawner.particle.enableTintByLife = true;
        spawner.particle.initialTint = vec4(1, .6, 0, 1);
        spawner.particle.finalTint = vec4(0, 0, 1, .1);
        spawner.particle.factorTint = 3.648f;

        spawner.position = { vec3(.4, .4, 0) };
        spawner.rotation = { vec3(0, 0, 0), vec3(0, 0, 7.8) };
        spawner.velocity = { vec3(10, 10, 0) };
        spawner.damping = { 7, 28 };
        spawner.aVelocity = { vec3(-11, -7, -3), vec3(7, 14, 4) };
        spawner.numberPerSpawn = {20, 20};
        spawner.numberPerSecond = 357;

        m_particleSpawner = spawner;
    }

    void simulationTick(SimulationTick tick, InputMap* input) override {
        vec2 mousePos = m_lens.ScreenToWorld2D(input->GetAxis("Mouse Position"));

        if (input->GetButton("Mouse Click")) {
            m_particleSpawner.instance.pos = vec3(mousePos, 0);
            for (auto [i, p] : m_particleSpawner.spawn(tick.deltaTime)) {
                m_particleMesh->add(i, p);
            }
        }

        if (input->GetOnce("Mouse Click")) {
            vec2 vel = normalize_safe(mousePos) * 10.f;
            m_orbitals.push_back(new Orbital(m_device, vec2(0, 0), vel, 150.f, 5, 0));
        }

        m_particleMesh->update(tick.deltaTime);

        for (int i = 0; i < m_orbitals.size(); i++) {
            Orbital* o = m_orbitals[i];
            o->update(tick.deltaTime);

            ParticleMesh::Instance inst{};
            inst.pos = vec3(o->center, 0);
            inst.scale = vec2(0.1f);
            inst.color = vec4(1, 0, .1, 1);

            Particle p{};
            p.lifetime = .4f;

            m_particleMesh->add(inst, p);

            if (o->life > o->lifetime) {
                delete_defer(o);
                pop_back(m_orbitals, i);
            }
        }
    }

    void render(SimulationTick tick, VulkanFrameImage frame, RenderDevice* device, ImGuiLoop* imgui) override {
        float aspect = frame.width / (float)frame.height;
        float width = 2 * aspect;

        m_lens.aspect = frame.width / (float)frame.height;

        CameraUBO cam{};
        cam.view = m_lens.GetViewMatrix();
        cam.proj = m_lens.GetProjectionMatrix();
        cam.viewProj = cam.proj * cam.view;
        
        m_cameraUBO->setData(&cam);
        m_descriptor->writeUniformBuffer(frame.frameIndex, 1, m_cameraUBO);
        
        device->beginScreenRenderPass();

        CommandBuffer& cmd = *frame.commandBuffer;

        mat4 model = mat4(1.f);

        cmd.setShader(m_particleShader);
        cmd.pushConstant(m_particleShader, 0, &model);
        cmd.bindDescriptorSet(m_particleShader, m_descriptor->getSet(0));

        m_particleMesh->sendToDevice();
        m_particleMesh->draw(cmd);

        cmd.setShader(m_lineShader);
        cmd.bindDescriptorSet(m_lineShader, m_descriptor->getSet(0));

        LineShaderPushConstants linePush{};
        linePush.model = model;

        for (Orbital* o : m_orbitals) {
            cmd.pushConstant(m_lineShader, 0, &linePush);

            for (Arc* arc : o->arcs) {
                linePush.totalLength = arc->lineMesh->totalLength();
                cmd.pushConstant(m_lineShader, 0, &linePush);

                arc->lineMesh->sendToDevice();
                arc->lineMesh->draw(cmd);
            }
        }

        imgui->beginFrame();
        {
            if (ImGui::Begin("Test")) {
                ImGui::Text("This is a string");

                ImGui::Text("%f fps", 1.f / tick.deltaTime);
            }
            ImGui::End();
        }
        imgui->submitFrame(cmd.m_commandBuffer);

        vkCmdEndRenderPass(cmd.m_commandBuffer);
        vkEndCommandBuffer(cmd.m_commandBuffer);

        device->submitFrame();
    }

    void destroyGameState() {
    }

    void destroyStaticDeviceResources(RenderDevice* device) {
        for (Orbital* o : m_orbitals) {
            delete o;
        }

        delete m_cameraUBO;
        delete m_image;
        delete m_imageSampler;
        delete m_lineShader;
        delete m_particleMesh;
        delete m_particleShader;
        delete m_descriptor;
    }

private:
    CameraLens m_lens;
    UniformBuffer* m_cameraUBO;

    ParticleSpawner m_particleSpawner;
    ParticleMesh* m_particleMesh;
    Shader* m_particleShader;

    //std::vector<Arc*> m_arcs;
    std::vector<Orbital*> m_orbitals;
    Shader* m_lineShader;

    DescriptorGroup* m_descriptor;

    Image* m_image;
    ImageSampler* m_imageSampler;

    // either give full access to this or put it in the update
    RenderDevice* m_device;
};