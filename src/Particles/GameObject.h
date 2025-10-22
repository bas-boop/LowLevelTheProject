#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
    virtual ~GameObject() = default;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual bool isAlive() const = 0;

protected:
    bool active;
    double creationTime;
    int id;
    float x;
    float y;
};