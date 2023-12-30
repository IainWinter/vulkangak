#pragma once

#include "render/backend/render_device.h"
#include "math/random.h"

#include <functional>

struct Particle {
    struct Instance {
        vec3 pos;
        vec2 scale;
        vec3 rotation;
        vec4 color;
    };

	vec3 velocity = vec3(0.f);
	float damping = 0.f;

	vec3 aVelocity = vec3(0.f);
	float aDamping = 0.f;

	float life = 0.f;
	float lifetime = 1.f;

	bool enableScalingByLife = false;
	vec2 initialScale = vec2(1.f);
	vec2 finalScale = vec2(0.f);
	float factorScale = 1.f;

	bool enableTintByLife = false;
	vec4 initialTint = vec4(1, 1, 1, 1);
	vec4 finalTint = vec4(1, 1, 1, 0);
	float factorTint = 1.f;
};

struct ParticleSpawner {
    Particle::Instance instance;
	Particle particle;

	RandomSphere position;
	RandomFloat3 rotation;
	RandomFloat2 scale;

	RandomFloat4 tint;
	RandomInt texture;

	RandomSphere velocity;
	RandomFloat damping;

	RandomFloat3 aVelocity;
	RandomFloat aDamping;

	RandomFloat life;

	RandomBool enableScalingByLife;
	RandomFloat2 finalScale;
	RandomFloat factorScale;

	RandomBool enableTintByLife;
	RandomFloat4 finalTint;
	RandomFloat factorTint;

	RandomBool additiveBlend;

	RandomInt numberPerSpawn;
	
    float numberPerSecond;
	float timer;

    std::vector<std::pair<Particle::Instance, Particle>> spawn(float deltaTime);
};

class ParticleMesh {
public:
    struct Vertex {
        vec2 pos, uv;
    };

    using Instance = Particle::Instance;

    static std::vector<VulkanVertexLayout> getLayout();

    ParticleMesh(RenderDevice* device, size_t batchSize);
    ~ParticleMesh();

    ParticleMesh(const ParticleMesh&) = delete;
    ParticleMesh& operator=(const ParticleMesh&) = delete;

    void update(float deltaTime);

    void sendToDevice();
    void draw(CommandBuffer& cmd);

    void add(const Instance& instance, const Particle& particle);

private:
    // index is not stable so don't expose
    // could use id, but particles are large in number
    // and short in lifetime so individually addressing one
    // is not needed

    void remove(s32 index);

    std::pair<Instance&, Particle&> get(s32 index);
    s32 count() const;

private:
    // annoying that these need to be vectors
    // there is no reason for that besides the creation api

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {0, 1}}, // 0  2
        {{-0.5f,  0.5f}, {0, 0}}, // 1  3
        {{ 0.5f, -0.5f}, {1, 1}},
        {{ 0.5f,  0.5f}, {1, 0}}
    };

    const std::vector<u32> indices = {
        0, 2, 1,
        1, 2, 3
    };

    const int batchSize;

    VertexBuffer* quadBuffer;
    IndexBuffer*  indexBuffer;

    std::vector<Instance> instances; // gpu data
    std::vector<Particle> particles; // cpu data

    // For now there can only be a single batch
    VertexBuffer* instanceBuffer;
};