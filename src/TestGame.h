#pragma once

#include "log.h"

#include "game/simulation_tick.h"
#include "game/game_interface.h"
#include "game/input.h"

#include "render/backend/render_device.h"
#include "render/backend/imgui_loop.h"
#include "imgui.h"

#include "render/particle/particle_mesh.h"
#include "render/sprite/sprite_mesh.h"
#include "render/line/line_mesh.h"
#include "render/lens.h"

#include "asset/package.h"
#include "asset/image.h"

#include "math/random.h"
#include "glm/mat4x4.hpp"

#include "containers/pop_erase.h"
#include "containers/defer_delete.h"

#include "render/backend/type/image.h"

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

    Arc(BufferFactory* bufferFactory) {
        lineMesh = new LineMesh(bufferFactory);
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

    Orbital(BufferFactory* bufferFactory, vec2 center, vec2 velocity, float mass, float lifetime, int numArcs) {
        this->center = center;
        this->vel = velocity;
        this->mass = mass;
        this->lifetime = lifetime;
        this->life = 0.f;

        for (int i = 0; i < numArcs; i++) {
            arcs.push_back(new Arc(bufferFactory));
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

        DescriptorSetLayout setLayout = {
            .bindings = {
                {
                    .stageBits = ShaderStage_Fragment,
                    .type = DescriptorType_ImageSampler,
                    .elementCount = 1,
                    .location = 0,
                },
                {
                    .stageBits = ShaderStage_Vertex,
                    .type = DescriptorType_UniformBuffer,
                    .elementCount = 1,
                    .location = 1,
                }
            }
        };

        m_descriptor = device->descriptorSetFactory->createDescriptorSet(setLayout);

        // Create particle shader

        ShaderProgramSource particleProgramSource;
        particleProgramSource.vertexLayout = ParticleMesh::getLayout();
        particleProgramSource.pushConstants = {{ ShaderStage_Vertex, sizeof(mat4) }};
        particleProgramSource.descriptorSets = { m_descriptor };
        particleProgramSource.shaders = {
            { ShaderStage_Vertex, package["particle.vert"].binary },
            { ShaderStage_Fragment, package["particle.frag"].binary }
        };

        m_particleShader = device->shaderFactory->createShader(particleProgramSource);

        // Create line shader

        ShaderProgramSource lineProgramSource;
        lineProgramSource.vertexLayout = LineMesh::getLayout();
        lineProgramSource.pushConstants = {{ ShaderStage_Vertex, sizeof(LineShaderPushConstants) }};
        lineProgramSource.descriptorSets = { m_descriptor };
        lineProgramSource.shaders = {
            { ShaderStage_Vertex, package["line.vert"].binary },
            { ShaderStage_Fragment, package["line.frag"].binary }
        };

        m_lineShader = device->shaderFactory->createShader(lineProgramSource);

        // Create sprite shader

        ShaderProgramSource textProgramSource;
        textProgramSource.vertexLayout = SpriteMesh::getLayout();
        textProgramSource.descriptorSets = { m_descriptor };
        textProgramSource.shaders = {
            { ShaderStage_Vertex, package["sprite.vert"].binary },
            { ShaderStage_Fragment, package["sprite.frag"].binary }
        };

        m_spriteShader = device->shaderFactory->createShader(textProgramSource);

        // Create particle mesh

        m_particleMesh = new ParticleMesh(device->bufferFactory, 13000);

        // Create sprite

        m_spriteMesh = new SpriteMesh(device->bufferFactory, 4);

        // Camera

        m_cameraUBO = device->bufferFactory->createUniformBufferEmptyFromType<CameraUBO>();

        // Load and write image to group

        m_image = device->imageFactory->createImage2DFromAsset(package["test.font.atlas"]);
        m_imageSampler = device->imageSamplerFactory->createImageSampler();

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
            m_orbitals.push_back(new Orbital(m_device->bufferFactory, vec2(0, 0), vel, 150.f, 5, 10));
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
                pop_erase(m_orbitals, i);
            }
        }

        // Add some sprite

        SpriteMesh::Instance inst{};
        inst.pos = vec2(20, 0);
        inst.scale = vec2(16);
        m_spriteMesh->add(inst);
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

        mat4 model = mat4(1.f);

        CommandBuffer& cmd = *frame.commandBuffer;

        // Draw particles

        cmd.setShader(m_particleShader);
        cmd.pushConstant(m_particleShader, 0, &model);
        cmd.bindDescriptorSet(m_particleShader, m_descriptor, frame.frameIndex);

        m_particleMesh->draw(cmd);

        // Draw lines

        cmd.setShader(m_lineShader);
        cmd.bindDescriptorSet(m_lineShader, m_descriptor, frame.frameIndex);

        for (Orbital* o : m_orbitals) {
            for (Arc* arc : o->arcs) {
                LineShaderPushConstants linePush{};
                linePush.model = model;
                linePush.totalLength = arc->lineMesh->totalLength();
                cmd.pushConstant(m_lineShader, 0, &linePush);

                arc->lineMesh->sendToDevice();
                arc->lineMesh->draw(cmd);
            }
        }

        // Draw sprites

        cmd.setShader(m_spriteShader);
        cmd.bindDescriptorSet(m_spriteShader, m_descriptor, frame.frameIndex);

        m_spriteMesh->draw(cmd);

        // cmd.setShader(m_textShader);
        // cmd.bindDescriptorSet(m_textShader, m_descriptor, frame.frameIndex);

        // m_textMesh->sendToDevice();
        // m_textMesh->draw(cmd);

        // Draw imgui

        imgui->beginFrame();
        {
            if (ImGui::Begin("Test")) {
                ImGui::Text("%f fps", 1.f / tick.deltaTime);
            }
            ImGui::End();
        }
        imgui->submitFrame(frame.commandBuffer);

        cmd.endRenderPass();
        cmd.end();

        device->submitFrame();
    }

    void destroyGameState() {
    }

    void destroyStaticDeviceResources(RenderDevice* device) {
        for (Orbital* o : m_orbitals) {
            delete o;
        }

        delete m_particleMesh;
        delete m_spriteMesh;

        device->bufferFactory->destroyBuffer(m_cameraUBO);
        device->imageFactory->destroyImage(m_image);
        device->imageSamplerFactory->destroyImageSampler(m_imageSampler);
        device->descriptorSetFactory->destroyDescriptorSet(m_descriptor);
        device->shaderFactory->destroyShader(m_particleShader);
        device->shaderFactory->destroyShader(m_lineShader);
        device->shaderFactory->destroyShader(m_spriteShader);
    }

private:
    CameraLens m_lens;

    ParticleSpawner m_particleSpawner;
    ParticleMesh* m_particleMesh;
    Shader* m_particleShader;

    std::vector<Orbital*> m_orbitals;
    Shader* m_lineShader;

    SpriteMesh* m_spriteMesh;
    Shader* m_spriteShader;
    Image* m_spriteImage;

    // TextMesh* m_textMesh;
    // Image* m_textImage;

    Image* m_image;
    ImageSampler* m_imageSampler;
    Buffer* m_cameraUBO;
    DescriptorSet* m_descriptor;

    RenderDevice* m_device;
};