#include "particle_mesh.h"

std::vector<std::pair<Particle::Instance, Particle>> ParticleSpawner::spawn(float deltaTime) {
    float dtPerSpawn = 1.f / numberPerSecond;
    timer += deltaTime;

    std::vector<std::pair<Particle::Instance, Particle>> particles;

    while (timer >= dtPerSpawn) {
        timer -= dtPerSpawn;

        int spawnCount = numberPerSpawn.get();

        for (int i = 0; i < spawnCount; i++) {
		    Particle::Instance inst = instance;
            Particle part = particle;

            inst.pos      += position.get();
            inst.rotation += rotation.get();
            inst.scale    += scale.get();
            inst.color    += tint.get();

            part.velocity  += velocity.get();
            part.damping   += damping.get();
            part.aVelocity += aVelocity.get();
            part.aDamping  += aDamping.get();
            
            particles.push_back({ inst, part });
        }
    }

    return particles;
}

std::vector<VulkanVertexLayout> ParticleMesh::getLayout() {
    return VertexLayoutBuilder()
        .buffer(sizeof(Vertex), false)
            .attribute(0, offsetof(Vertex, pos), VK_FORMAT_R32G32_SFLOAT)
            .attribute(1, offsetof(Vertex, uv),  VK_FORMAT_R32G32_SFLOAT)
        .buffer(sizeof(Instance), true)
            .attribute(2, offsetof(Instance, pos),      VK_FORMAT_R32G32B32_SFLOAT)
            .attribute(3, offsetof(Instance, scale),    VK_FORMAT_R32G32_SFLOAT)
            .attribute(4, offsetof(Instance, rotation), VK_FORMAT_R32G32B32_SFLOAT)
            .attribute(5, offsetof(Instance, color),    VK_FORMAT_R32G32B32A32_SFLOAT)
        .build();
}

ParticleMesh::ParticleMesh(RenderDevice* device, size_t batchSize) 
    : batchSize (batchSize)
{
    quadBuffer = device->newVertexBuffer<Vertex>(vertices);
    indexBuffer = device->newIndexBuffer(indices);
    instanceBuffer = device->newVertexBuffer(sizeof(Instance), batchSize, nullptr);
}

ParticleMesh::~ParticleMesh() {
    delete quadBuffer;
    delete indexBuffer;
    delete instanceBuffer;
}

void ParticleMesh::update(float deltaTime) {
    for (s32 i = 0; i < count(); i++) {
        auto [instance, particle] = get(i);

        particle.life += deltaTime;
        
        if (particle.life >= particle.lifetime) {
            remove(i);
            i--;
        }
        
        //instance.pos.z = i / (float)count();

        float ratio = particle.life / particle.lifetime;

        instance.pos += particle.velocity * deltaTime;
        particle.velocity *= damping(particle.damping, deltaTime);

        instance.rotation += particle.aVelocity * deltaTime;
        particle.aVelocity *= damping(particle.aDamping, deltaTime);

        if (particle.enableScalingByLife) {
            float w = pow(ratio, particle.factorScale);
            instance.scale = lerp(particle.initialScale, particle.finalScale, w);
        }

        if (particle.enableTintByLife) {
            float w = pow(ratio, particle.factorTint);
            instance.color = lerp(particle.initialTint, particle.finalTint, w);
        }
    }
}

void ParticleMesh::sendToDevice() {
    // If there is no data, skip
    // Nothing will get drawn anyways
    if (count() == 0) {
        return;
    }
    
    instanceBuffer->setData(0, sizeof(Instance) * count(), instances.data());
}

void ParticleMesh::draw(CommandBuffer& cmd) {
    VertexBuffer* buffers[2] = { quadBuffer, instanceBuffer };

    cmd.bindVertexBuffers(2, buffers);
    cmd.bindIndexBuffer(indexBuffer);
    cmd.drawIndexed(indices.size(), count());
}

void ParticleMesh::add(const Instance& instance, const Particle& particle) {
    // skip if the count is over the max size
    if (count() + 1 >= batchSize) {
        return;
    }

    instances.push_back(instance);
    particles.push_back(particle);
}

void ParticleMesh::remove(s32 index) {
    instances.at(index) = instances.back();
    instances.pop_back();

    particles.at(index) = particles.back();
    particles.pop_back();
}

std::pair<ParticleMesh::Instance&, Particle&> ParticleMesh::get(s32 index) {
    return { instances.at(index), particles.at(index) };
}

s32 ParticleMesh::count() const {
    return (s32)instances.size();
}