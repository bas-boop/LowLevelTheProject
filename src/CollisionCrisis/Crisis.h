#pragma once

#include <SFML/Graphics.hpp>
#include <random>

struct Ball;

class Crisis
{
public:
    Crisis();
    ~Crisis();

private:
    void updateBalls(const sf::Vector2u& windowSize, float deltaTime);
    void moveBalls(const float deltaTime);
    void handleCollisions(const sf::Vector2u& windowSize);
    void buildSpatialGrid();
    void drawBalls( sf::RenderWindow& window ) const;
    void handleWallCollision(const sf::Vector2u& windowSize, Ball& ball);
    void handleBallsCollision(Ball& ball1, Ball& ball2);
    void checkCellPairCollisions(const std::vector<int>& cellA, const std::vector<int>& cellB);

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    float fps = 0;
    float highestFps = 0;
    int frameCount = 0;

    const int ballAmount = 7500;
    const int gridStride = 30000;
    const float cellSize = 32.0f;
    std::vector<Ball> balls;
    std::unordered_map<int, std::vector<int>> grid;
    const std::array<std::array<int, 2>, 9> neighborOffsets =
    {{
        {{-1, -1}}, {{0, -1}}, {{1, -1}},
        {{-1,  0}}, {{0,  0}}, {{1,  0}},
        {{-1,  1}}, {{0,  1}}, {{1,  1}}
    }};

    const sf::Vector2u windowSize = {1800, 1200};
    const float margin = 5.0f;
    
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> posxDist;
    std::uniform_real_distribution<float> posyDist;
    std::uniform_real_distribution<float> velDist;
    std::uniform_real_distribution<float> radiusDist;
    std::uniform_int_distribution<> colorDist;
};

struct Ball
{
    int id;
    float radius;
    sf::CircleShape shape;
    sf::Vector2f velocity;
    sf::Vector2f position;
    
    Ball(int id, float x, float y, float radius, sf::Color color, float vx, float vy)
    {
        this->id = id;
        shape.setRadius(radius);
        shape.setPosition(sf::Vector2f(x, y));
        shape.setFillColor(color);
        shape.setOrigin(sf::Vector2f(radius, radius));
        velocity = sf::Vector2f(vx, vy);
    }

    bool operator<(const Ball& other) const
    {
        const auto pos1 = shape.getPosition();
        const auto pos2 = other.shape.getPosition();

        if (pos1.x == pos2.x) 
            return pos1.y < pos2.y;

        return pos1.x < pos2.x;
    }

    bool operator==(const Ball& other) const {
        return id == other.id;
    }
};