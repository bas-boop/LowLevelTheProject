#pragma once

#include <random>
#include <SFML/Graphics.hpp>

struct ParticleSystemSoA
{
    std::vector<sf::Vector2f> positions;
    std::vector<sf::Vector2f> velocities;
    std::vector<sf::Color> colors;
    std::vector<float> lifetimes;
    std::vector<float> maxLifetimes;
    std::vector<bool> active;

    size_t count = 0;
    float gravity = 98.1f; // m/s²

    void reserve(const size_t n) {
        positions.reserve(n);
        velocities.reserve(n);
        colors.reserve(n);
        lifetimes.reserve(n);
        maxLifetimes.reserve(n);
        active.reserve(n);
    }
};

class ParticleSystem
{
public:
    ParticleSystem(sf::RenderWindow* win, const int reserveCount = 0);

    void spawnParticles(int count, sf::Vector2f origin);
    void update(float deltaTime);
    void render();

    size_t getParticleCount() const;
    
private:
    ParticleSystemSoA ps;
    sf::RenderWindow* window;
    std::mt19937 rng;
    const float particleSize = 3.0f;

    void spawnParticles(ParticleSystemSoA& psRef, int count, sf::Vector2f origin);
    void updateParticles(ParticleSystemSoA& psRef, float dt);
    void renderParticles(const ParticleSystemSoA& psRef, sf::RenderWindow& windowRef) const;
};
