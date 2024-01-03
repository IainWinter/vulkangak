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

VertexLayout ParticleMesh::getLayout() {
    VertexLayout vertexLayout = {
        .buffers = {
            {
                .binding = 0,
                .stride = sizeof(Vertex),
                .instanced = false,
                .attributes = {
                    {
                        .location = 0,
                        .offset = offsetof(Vertex, pos),
                        .format = AttributeFormat_Float2
                    },
                    {
                        .location = 1,
                        .offset = offsetof(Vertex, uv),
                        .format = AttributeFormat_Float2
                    }
                }
            },
            {
                .binding = 1,
                .stride = sizeof(Instance),
                .instanced = true,
                .attributes = {
                    {
                        .location = 2,
                        .offset = offsetof(Instance, pos),
                        .format = AttributeFormat_Float3
                    },
                    {
                        .location = 3,
                        .offset = offsetof(Instance, scale),
                        .format = AttributeFormat_Float2
                    },
                    {
                        .location = 4,
                        .offset = offsetof(Instance, rotation),
                        .format = AttributeFormat_Float3
                    },
                    {
                        .location = 5,
                        .offset = offsetof(Instance, color),
                        .format = AttributeFormat_Float4
                    }
                }
            }
        }
    };

    return vertexLayout;
}

ParticleMesh::ParticleMesh(BufferFactory* bufferFactory, u32 batchSize) 
    : bufferFactory (bufferFactory)
    , batchSize     (batchSize)
{
    quadBuffer = bufferFactory->createVertexBufferFromVector<Vertex>(vertices);
    indexBuffer = bufferFactory->createIndexBufferFromVector(indices);
    instanceBuffer = bufferFactory->createVertexBufferEmpty(sizeof(Instance), batchSize);
}

ParticleMesh::~ParticleMesh() {
    bufferFactory->destroyBuffer(quadBuffer);
    bufferFactory->destroyBuffer(indexBuffer);
    bufferFactory->destroyBuffer(instanceBuffer);
}

void ParticleMesh::add(const Instance& instance, const Particle& particle) {
    // skip if the count is over the max size
    if (count() + 1 >= batchSize) {
        return;
    }

    instances.push_back(instance);
    particles.push_back(particle);
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

void ParticleMesh::draw(CommandBuffer& cmd) {
    if (count() == 0) {
        return;
    }
    
    instanceBuffer->setDataVector(instances);

    Buffer* buffers[2] = { quadBuffer, instanceBuffer };

    cmd.bindVertexBuffers(2, buffers);
    cmd.bindIndexBuffer(indexBuffer);
    cmd.drawIndexed(indices.size(), count());
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