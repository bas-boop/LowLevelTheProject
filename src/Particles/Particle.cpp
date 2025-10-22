#include "Particle.h"

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float life):
    isDying(false),
    position(pos),
    lifetime(life),
    velocity(vel),
    hasGravity(true),
    color(col),
    effectType('n'),
    maxLifetime(life),
    isVisible(true),
    mass(1.0f),
    collisionEnabled(true),
    radius(2.0f)
{
    // Shape
    shape = sf::CircleShape(radius);
    shape.setFillColor(color);
    shape.setPosition(position);

    // Other data
    active = true;
    creationTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
    
    static int counter = 0;
    id = counter++;
    debugName = "Particle_" + std::to_string(id);
    lastPosition = position;
    lastUpdateTime = creationTime;
    acceleration = sf::Vector2f(0, 98.1f); // Gravity
}

void Particle::update(float deltaTime)
{
    if (!active)
        return;

    // Store last position
    lastPosition = position;

    // Physics calculations
    if (hasGravity)
        velocity += acceleration * deltaTime;

    position += velocity * deltaTime;

    // Lifetime management
    lifetime -= deltaTime;

    // Color calculations — clamp alpha to [0,1]
    const float alpha = std::clamp( static_cast<float>(lifetime / maxLifetime), 0.0f, 1.0f);
    color.a = static_cast<uint8_t>(alpha * 255.0f);

    // Update shape properties every frame
    shape.setPosition(position);
    shape.setFillColor(color);

    if (lifetime <= 0.0f)
    {
        isDying = true;
        active = false;
    }

    // Bounds checking
    if (collisionEnabled &&
        (position.x < 0.0f
        || position.x > 800.0f
        || position.y < 0.0f
        || position.y > 800.0f))
    {
        velocity *= -0.8f;
        position.x = std::max(0.0f, std::min(800.0f, position.x));
        position.y = std::max(0.0f, std::min(800.0f, position.y));
    }
}

void Particle::render(sf::RenderWindow& window)
{
    if (isVisible
        && active)
        window.draw(shape);
}

bool Particle::isAlive() const
{
    return active && !isDying;
}
