#pragma once

#include <random>
#include <SFML/Graphics.hpp>

#include "GameObject.h"

class Particle : public GameObject {
private:
    bool isDying;
    sf::Vector2f position;
    double lifetime;
    sf::Vector2f velocity;
    bool hasGravity;
    sf::Color color;
    char effectType;
    sf::Vector2f acceleration;
    double maxLifetime;
    sf::CircleShape shape;
    bool isVisible;
    float mass;
    sf::Vector2f lastPosition;
    std::string debugName;
    double lastUpdateTime;
    bool collisionEnabled;
    float radius;

public:
    Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float life);

    ~Particle() override { }

    void update(float deltaTime) override;

    void render(sf::RenderWindow& window) override;

    bool isAlive() const override;
};
