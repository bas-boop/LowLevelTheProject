#pragma once

#include <random>
#include <SFML/Graphics.hpp>

#include "GameObject.h"
#include "Particle.h"

// Inefficient particle system manager
class ParticleSystem {
private:
    // Particle Container
    std::vector<std::unique_ptr<GameObject>> particles;

    // Separate vectors per property
    std::vector<sf::Vector2f> positions;
    std::vector<sf::Vector2f> velocities;
    std::vector<sf::Color> colors;
    std::vector<bool> aliveFlags;

    sf::RenderWindow* window;
    std::mt19937 rng;

public:
    ParticleSystem(sf::RenderWindow* win, const int count);

    void spawnParticles(int count, sf::Vector2f origin);

    void update(float deltaTime);

    void render();

    size_t getParticleCount() const;
};