#include "ParticleSystem.h"
#include <numbers>
#include <algorithm>
#include <random>

ParticleSystem::ParticleSystem(sf::RenderWindow* win, const int reserveCount) :
    window(win),
    rng(std::random_device{}())
{
    if (reserveCount > 0)
        ps.reserve(static_cast<size_t>(reserveCount));
}

// Public wrappers for SoA
void ParticleSystem::spawnParticles(int count, sf::Vector2f origin)
{
    spawnParticles(ps, count, origin);
}

void ParticleSystem::update(float deltaTime)
{
    updateParticles(ps, deltaTime);
}

void ParticleSystem::render()
{
    renderParticles(ps, *window);
}

void ParticleSystem::spawnParticles(ParticleSystemSoA& psRef, int count, sf::Vector2f origin)
{
    std::uniform_real_distribution<float> angleDist(0, 2 * std::numbers::pi_v<float>);
    std::uniform_real_distribution<float> speedDist(50, 200);
    std::uniform_int_distribution<int> colorDist(0, 255);
    std::uniform_real_distribution<float> lifeDist(5, 10);

    static std::mt19937 localRng(std::random_device{}());

    for (int i = 0; i < count; i++)
    {
        const float angle = angleDist(localRng);
        const float speed = speedDist(localRng);

        sf::Vector2f vel(std::cos(angle) * speed, std::sin(angle) * speed);
        sf::Color col(static_cast<uint8_t>(colorDist(localRng)),
                      static_cast<uint8_t>(colorDist(localRng)),
                      static_cast<uint8_t>(colorDist(localRng)),
                      255);
        
        float lifetime = lifeDist(localRng);

        psRef.positions.push_back(origin);
        psRef.velocities.push_back(vel);
        psRef.colors.push_back(col);
        psRef.lifetimes.push_back(lifetime);
        psRef.maxLifetimes.push_back(lifetime);
        psRef.active.push_back(true);
        psRef.count++;
    }
}

void ParticleSystem::updateParticles(ParticleSystemSoA& psRef, float dt)
{
    for (size_t i = 0; i < psRef.count; )
    {
        if (!psRef.active[i])
        {
            i++;
            continue;
        }

        // Gravity
        psRef.velocities[i].y += psRef.gravity * dt;
        
        // Movement
        psRef.positions[i] += psRef.velocities[i] * dt;

        // Lifetime
        psRef.lifetimes[i] -= dt;
        const float alpha = std::clamp(psRef.lifetimes[i] / psRef.maxLifetimes[i], 0.0f, 1.0f);
        psRef.colors[i].a = static_cast<uint8_t>(alpha * 255);

        // Window collision
        auto& pos = psRef.positions[i];
        if (pos.x < 0
            || pos.x > 800.0f)
        {
            psRef.velocities[i].x *= -0.8f;
            pos.x = std::clamp(pos.x, 0.0f, 800.0f);
        }
        
        if (pos.y < 0
            || pos.y > 800.0f)
        {
            psRef.velocities[i].y *= -0.8f;
            pos.y = std::clamp(pos.y, 0.0f, 800.0f);
        }

        // Dead: swap-remove
        if (psRef.lifetimes[i] <= 0)
        {
            size_t last = psRef.count - 1;
            
            if (i != last)
            {
                psRef.positions[i] = psRef.positions[last];
                psRef.velocities[i] = psRef.velocities[last];
                psRef.colors[i] = psRef.colors[last];
                psRef.lifetimes[i] = psRef.lifetimes[last];
                psRef.maxLifetimes[i] = psRef.maxLifetimes[last];
                psRef.active[i] = psRef.active[last];
            }
            
            psRef.positions.pop_back();
            psRef.velocities.pop_back();
            psRef.colors.pop_back();
            psRef.lifetimes.pop_back();
            psRef.maxLifetimes.pop_back();
            psRef.active.pop_back();
            --psRef.count;
        }
        else
            i++;
    }
}

void ParticleSystem::renderParticles(const ParticleSystemSoA& psRef, sf::RenderWindow& windowRef) const
{
    if (psRef.count == 0)
        return;
    
    sf::CircleShape shape(particleSize);
    shape.setOrigin({particleSize, particleSize});

    for (size_t i = 0; i < psRef.count; i++)
    {
        shape.setPosition(psRef.positions[i]);
        shape.setFillColor(psRef.colors[i]);
        windowRef.draw(shape);
    }
}

size_t ParticleSystem::getParticleCount() const
{
    return ps.count;
}
