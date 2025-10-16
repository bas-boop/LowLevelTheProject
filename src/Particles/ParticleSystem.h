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
    ParticleSystem(sf::RenderWindow* win) : window(win), rng(std::random_device{}()) {
        // Pre-allocation
        particles.reserve(100);
    }

    void spawnParticles(int count, sf::Vector2f origin) {
        std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
        std::uniform_real_distribution<float> speedDist(50, 200);
        std::uniform_int_distribution<int> colorDist(0, 255);
        std::uniform_real_distribution<float> lifeDist(1.0f, 5.0f);

        for (int i = 0; i < count; ++i) {
            float angle = angleDist(rng);
            float speed = speedDist(rng);
            sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);

            sf::Color color(colorDist(rng), colorDist(rng), colorDist(rng), 255);
            float lifetime = lifeDist(rng);

            // Create particle
            auto particle = std::make_unique<Particle>(origin, velocity, color, lifetime);

            // Store in containers
            particles.push_back(std::move(particle));
        }
    }

    void update(float deltaTime) {
        // Iterator
        for (auto it = particles.begin(); it != particles.end();) {
            (*it)->update(deltaTime);

            if (!(*it)->isAlive()) {
                it = particles.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void render() {
        // Render all particles
        for (auto& particle : particles) {
            particle->render(*window);
        }
    }

    size_t getParticleCount() const {
        return particles.size();
    }
};