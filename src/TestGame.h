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
#include "render/text/text_mesh.h"
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

        input->CreateAxis("Move")
            .Map(Key_W, vec2( 0, -1))
            .Map(Key_S, vec2( 0,  1))
            .Map(Key_A, vec2(-1,  0))
            .Map(Key_D, vec2( 1,  0))
            .Normalized();
    }

    void createStaticDeviceResources(RenderDevice* device) override {
        AssetPackage package = loadPackage("./assets.bin");

        // load all descriptors
        
        DescriptorSetLayoutConfig viewLayout = {{
            { 0, ShaderStage_Vertex, DescriptorType_UniformBuffer }
        }};

        DescriptorSetLayoutConfig materialLayout = {{
            { 0, ShaderStage_Fragment, DescriptorType_ImageSampler }
        }};

        m_viewSetLayout = device->descriptorSetLayoutFactory->createDescriptorSetLayout(viewLayout);
        m_materialSetLayout = device->descriptorSetLayoutFactory->createDescriptorSetLayout(materialLayout);

        m_renderPassLoRes = device->renderPassFactory->createRenderPass();

        // Create particle shader

        ShaderProgramSource particleProgramSource;
        particleProgramSource.vertexLayout = ParticleMesh::getLayout();
        particleProgramSource.pushConstants = {{ ShaderStage_Vertex, sizeof(mat4) }};
        particleProgramSource.descriptorSetLayouts = { m_viewSetLayout, m_materialSetLayout };
        particleProgramSource.blendType = BlendType_Additive;
        particleProgramSource.shaders = {
            { ShaderStage_Vertex, package["particle.vert"].binary },
            { ShaderStage_Fragment, package["particle.frag"].binary }
        };

        m_particleShader = device->shaderFactory->createShaderRenderPass(m_renderPassLoRes, particleProgramSource);

        // Create line shader

        ShaderProgramSource lineProgramSource;
        lineProgramSource.vertexLayout = LineMesh::getLayout();
        lineProgramSource.pushConstants = {{ ShaderStage_Vertex, sizeof(LineShaderPushConstants) }};
        lineProgramSource.descriptorSetLayouts = { m_viewSetLayout, m_materialSetLayout };
        lineProgramSource.blendType = BlendType_Alpha;
        lineProgramSource.shaders = {
            { ShaderStage_Vertex, package["line.vert"].binary },
            { ShaderStage_Fragment, package["line.frag"].binary }
        };

        m_lineShader = device->shaderFactory->createShader(lineProgramSource);

        // Create sprite shader

        ShaderProgramSource spriteProgramSource;
        spriteProgramSource.vertexLayout = SpriteMesh::getLayout();
        spriteProgramSource.descriptorSetLayouts = { m_viewSetLayout, m_materialSetLayout };
        spriteProgramSource.blendType = BlendType_Alpha;
        spriteProgramSource.shaders = {
            { ShaderStage_Vertex, package["sprite.vert"].binary },
            { ShaderStage_Fragment, package["sprite.frag"].binary }
        };

        m_spriteShader = device->shaderFactory->createShader(spriteProgramSource);

        // Create circle sprite shader

        ShaderProgramSource circleSpriteProgramSource;
        circleSpriteProgramSource.vertexLayout = SpriteMesh::getLayout();
        circleSpriteProgramSource.descriptorSetLayouts = { m_viewSetLayout, m_materialSetLayout };
        circleSpriteProgramSource.blendType = BlendType_Alpha;
        circleSpriteProgramSource.shaders = {
            { ShaderStage_Vertex, package["sprite.vert"].binary },
            { ShaderStage_Fragment, package["sprite_circle.frag"].binary }
        };

        m_circleSpriteShader = device->shaderFactory->createShader(circleSpriteProgramSource);

        // Create text shader

        ShaderProgramSource textProgramSource;
        textProgramSource.vertexLayout = TextMesh::getLayout();
        textProgramSource.descriptorSetLayouts = { m_viewSetLayout, m_materialSetLayout };
        textProgramSource.blendType = BlendType_Alpha;
        textProgramSource.shaders = {
            { ShaderStage_Vertex, package["text_msdf.vert"].binary },
            { ShaderStage_Fragment, package["text_msdf.frag"].binary }
        };
        textProgramSource.pushConstants = {{ ShaderStage_Vertex, sizeof(mat4) }};

        m_textShader = device->shaderFactory->createShader(textProgramSource);

        // Create descriptor sets

        m_viewSet = device->descriptorSetFactory->createDescriptorSet(m_viewSetLayout);
        m_spriteMaterialSet = device->descriptorSetFactory->createDescriptorSet(m_materialSetLayout);
        m_textMaterialSet = device->descriptorSetFactory->createDescriptorSet(m_materialSetLayout);

        // Create particle mesh

        m_particleMesh = new ParticleMesh(device->bufferFactory, 13000);

        // Create sprite

        m_spriteMesh = new SpriteMesh(device->bufferFactory, 4);

        // Create text mesh

        m_font = new Font();
        m_font->data = package["test.font"];
        m_font->msdfAtlas = device->imageFactory->createImage2DFromAsset(package[m_font->data.atlasPath]);

        m_textMesh = new TextMesh(device->bufferFactory, m_font, 100);
        m_textMesh->setString("Hello there\n123\n456\n789");

        // Camera

        m_cameraUBO = device->bufferFactory->createUniformBufferEmptyFromType<CameraUBO>();

        // Load and write image to group

        m_image = device->imageFactory->createImage2DFromAsset(package["test.image"]);
        m_imageSampler = device->imageSamplerFactory->createImageSampler();

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

        static vec2 pos = vec2(0, 0);
        pos += input->GetAxis("Move") * tick.deltaTime * 10.f;

        SpriteMesh::Instance inst{};
        inst.pos = pos;
        inst.scale = vec2(4);
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

        // this isn't double buffered so bad        
        m_cameraUBO->setData(&cam);

        // Update descriptor sets, I think these are fine?
        m_viewSet->writeUniformBuffer(0, m_cameraUBO);
        m_spriteMaterialSet->writeImage(0, m_image, m_imageSampler);
        m_textMaterialSet->writeImage(0, m_font->msdfAtlas, m_imageSampler);

        device->beginScreenRenderPass();

        mat4 model = mat4(1.f);

        CommandBuffer& cmd = *frame.commandBuffer;

        // Draw particles

        cmd.setShader(m_particleShader);
        cmd.pushConstant(m_particleShader, 0, &model);
        cmd.bindDescriptorSet(m_textShader, 0, m_viewSet);
        cmd.bindDescriptorSet(m_textShader, 1, m_spriteMaterialSet);

        m_particleMesh->draw(cmd);

        // Draw lines

        cmd.setShader(m_lineShader);
        cmd.bindDescriptorSet(m_lineShader, 0, m_viewSet);
        cmd.bindDescriptorSet(m_lineShader, 1, m_spriteMaterialSet);

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

        // cmd.setShader(m_spriteShader);
        // cmd.bindDescriptorSet(m_spriteShader, 0, m_viewSet);
        // cmd.bindDescriptorSet(m_spriteShader, 1, m_spriteMaterialSet);

        // m_spriteMesh->draw(cmd);

        // Draw circles

        cmd.setShader(m_circleSpriteShader);
        cmd.bindDescriptorSet(m_spriteShader, 0, m_viewSet);
        cmd.bindDescriptorSet(m_spriteShader, 1, m_spriteMaterialSet);

        m_spriteMesh->draw(cmd);

        // Draw text

        cmd.setShader(m_textShader);
        cmd.pushConstant(m_textShader, 0, &model);
        cmd.bindDescriptorSet(m_textShader, 0, m_viewSet);
        cmd.bindDescriptorSet(m_textShader, 1, m_textMaterialSet);

        m_textMesh->draw(cmd);

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
        device->bufferFactory->destroyBuffer(m_cameraUBO);
        
        device->imageFactory->destroyImage(m_image);
        device->imageFactory->destroyImage(m_font->msdfAtlas);

        device->imageSamplerFactory->destroyImageSampler(m_imageSampler);

        device->descriptorSetLayoutFactory->destroyDescriptorSetLayout(m_viewSetLayout);
        device->descriptorSetLayoutFactory->destroyDescriptorSetLayout(m_materialSetLayout);
        device->descriptorSetFactory->destroyDescriptorSet(m_viewSet);
        device->descriptorSetFactory->destroyDescriptorSet(m_spriteMaterialSet);
        device->descriptorSetFactory->destroyDescriptorSet(m_textMaterialSet);

        device->shaderFactory->destroyShader(m_particleShader);
        device->shaderFactory->destroyShader(m_lineShader);
        device->shaderFactory->destroyShader(m_spriteShader);
        device->shaderFactory->destroyShader(m_circleSpriteShader);
        device->shaderFactory->destroyShader(m_textShader);

        delete m_font;

        for (Orbital* o : m_orbitals) {
            delete o;
        }

        delete m_particleMesh;
        delete m_spriteMesh;
        delete m_textMesh;
    }

private:
    ParticleSpawner m_particleSpawner;

    // Text
    Font* m_font;

    // Camera
    CameraLens m_lens;
    Buffer* m_cameraUBO;

    // Meshes
    TextMesh* m_textMesh;
    SpriteMesh* m_spriteMesh;
    ParticleMesh* m_particleMesh;
    std::vector<Orbital*> m_orbitals;

    // Images
    Image* m_image;
    Image* m_spriteImage;
    ImageSampler* m_imageSampler;

    // Shaders
    Shader* m_lineShader;
    Shader* m_particleShader;
    Shader* m_spriteShader;
    Shader* m_circleSpriteShader;
    Shader* m_textShader;

    // Everything to do with descriptors
    DescriptorSetLayout* m_viewSetLayout;
    DescriptorSetLayout* m_materialSetLayout;
    DescriptorSet* m_viewSet;
    DescriptorSet* m_spriteMaterialSet;
    DescriptorSet* m_textMaterialSet;

    // do a pixel process
    RenderPass* m_renderPassLoRes;

    // This is here because the Game interface is bad
    RenderDevice* m_device;
};