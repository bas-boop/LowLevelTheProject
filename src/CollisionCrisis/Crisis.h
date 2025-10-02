#pragma once

#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>

#include <vector>
#include <random>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string_view>
#include <set>
#include <unordered_set>

struct Ball;

class Crisis
{
public:
    Crisis();
    ~Crisis();

private:
    void updateBalls(const sf::Vector2u& windowSize, float deltaTime);
    void drawBalls( sf::RenderWindow& window ) const;

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    float fps;
    
    std::vector<Ball> balls;
    std::set<Ball> betterBalls; // gives Error C2676
    //std::unordered_set<Ball> betsBalls;
    
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> posDist;
    std::uniform_real_distribution<float> velDist;
    std::uniform_int_distribution<int> colorDist;
    std::uniform_real_distribution<float> radiusDist;
};

struct Ball
{
    int id;    
    sf::CircleShape shape;
    sf::Vector2f velocity;
    
    Ball(int id, float x, float y, float radius, sf::Color color, float vx, float vy)
    {
        this->id = id;
        shape.setRadius(radius);
        shape.setPosition(sf::Vector2f(x, y));
        shape.setFillColor(color);
        shape.setOrigin(sf::Vector2f(radius, radius)); // Center origin
        velocity = sf::Vector2f(vx, vy);
    }

    bool operator<(const Ball& other) const {
        auto pos1 = shape.getPosition();
        auto pos2 = other.shape.getPosition();
        if (pos1.x == pos2.x) 
            return pos1.y < pos2.y;
        return pos1.x < pos2.x;
    }

    bool operator==(const Ball& other)
    {
        return id == other.id;
    }
};
