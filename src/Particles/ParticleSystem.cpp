#include "ParticleSystem.h"

#include <numbers>

ParticleSystem::ParticleSystem(sf::RenderWindow* win, const int count):
    window(win),
    rng(std::random_device{}())
{
    // Pre-allocation
    particles.reserve(count);
}

void ParticleSystem::spawnParticles(int count, sf::Vector2f origin)
{
    std::uniform_real_distribution<float> angleDist(0, 2 * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> speedDist(50, 200);
    std::uniform_int_distribution<int> colorDist(0, 255);
    std::uniform_real_distribution<float> lifeDist(5.0f, 10.0f);

    for (int i = 0; i < count; i++)
    {
        float angle = angleDist(rng);
        float speed = speedDist(rng);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);

        sf::Color color(colorDist(rng), colorDist(rng), colorDist(rng), 255);
        float lifetime = lifeDist(rng);

        particles.emplace_back(std::make_unique<Particle>(origin, velocity, color, lifetime));
    }
}

void ParticleSystem::update(float deltaTime)
{
    // Iterator
    for (std::vector<std::unique_ptr<GameObject>>::iterator it = particles.begin(); it != particles.end();)
    {
        const std::unique_ptr<GameObject>& ita = *it;
        ita->update(deltaTime);

        if (ita->isAlive())
            ++it;
        else
            it = particles.erase(it);
    }
}

void ParticleSystem::render()
{
    // Render all particles
    for (auto& particle : particles)
        particle->render(*window);
}

size_t ParticleSystem::getParticleCount() const
{
    return particles.size();
}
