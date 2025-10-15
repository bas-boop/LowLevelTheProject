#include "Crisis.h"
#include "imgui-SFML.h"
#include "imgui.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <unordered_set>

Crisis::Crisis()
{
    sf::RenderWindow window;
    window.create(sf::VideoMode(windowSize), "Our crisis window");
    window.setVerticalSyncEnabled(false);

    if (!ImGui::SFML::Init(window))
        return;

    gen = std::mt19937(rd());
    posxDist = std::uniform_real_distribution<float>(margin, windowSize.x - margin);
    posyDist = std::uniform_real_distribution<float>(margin, windowSize.y - margin);
    velDist = std::uniform_real_distribution<float>(-200.0f, 200.0f);
    colorDist = std::uniform_int_distribution<int>(0, 255);
    radiusDist = std::uniform_real_distribution<float>(2.5f, 7.5f);

    balls = std::vector<Ball>();
    balls.reserve(ballAmount);
    grid.reserve(ballAmount);
    
    for (int i = 0; i < ballAmount; ++i)
    {
        const float x = posxDist(gen);
        const float y = posyDist(gen);
        const float radius = radiusDist(gen);
        const auto color = sf::Color (colorDist(gen), colorDist(gen), colorDist(gen));
        const float vx = velDist(gen);
        const float vy = velDist(gen);

        Ball b(i, x, y, radius, color, vx, vy);
        balls.push_back(b);
    }

    sf::Clock deltaClock;
    
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();
        }

        sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        window.clear();
        
        updateBalls(window.getSize(), dt.asSeconds());
        drawBalls(window);

        if (dt.asSeconds() > 0.f)
        {
            fps = 1.f / dt.asSeconds();
            frameCount++;

            if (frameCount > 5
                && fps > highestFps)
            {
                highestFps = fps;
                std::cout << "Highest fps: " << highestFps << "\n";
            }
        }
        
        ImGui::Begin("FPS Counter");
        ImGui::Text("FPS: %.1f", fps);
        ImGui::End();
        
        ImGui::SFML::Render(window);

        window.display();
    }
}

Crisis::~Crisis() = default;

void Crisis::updateBalls(const sf::Vector2u& windowSize, const float deltaTime)
{
    moveBalls(deltaTime);
    buildSpatialGrid();
    handleCollisions(windowSize);
}

void Crisis::moveBalls(const float deltaTime)
{
    std::ranges::for_each(balls, [&](Ball& ball)
    {
        ball.shape.move(ball.velocity * deltaTime);
    });
}

void Crisis::handleCollisions(const sf::Vector2u& windowSize)
{
    for (auto& ball : balls)
        handleWallCollision(windowSize, ball);

    for (const auto& [key, indices] : grid)
    {
        const int cellX = key % gridStride;
        const int cellY = key / gridStride;

        for (const auto& [ox, oy] : neighborOffsets)
        {
            const int neighborX = cellX + ox;
            const int neighborY = cellY + oy;
            const int neighborKey = neighborY * gridStride + neighborX;

            auto it = grid.find(neighborKey);

            if (it == grid.end())
                continue;

            checkCellPairCollisions(indices, it->second);
        }
    }
}

void Crisis::buildSpatialGrid()
{
    grid.clear();
    
    for (size_t i = 0; i < balls.size(); i++)
    {
        const auto& b = balls[i];
        const int cellX = static_cast<int>(b.shape.getPosition().x / cellSize);
        const int cellY = static_cast<int>(b.shape.getPosition().y / cellSize);
        int key = cellY * gridStride + cellX;
        
        grid[key].push_back(static_cast<int>(i));
    }
}

void Crisis::drawBalls(sf::RenderWindow& window) const
{
    std::ranges::for_each(balls, [&](const Ball& ball)
    {
        window.draw(ball.shape);
    });
}

void Crisis::handleWallCollision(const sf::Vector2u& windowSize, Ball& ball)
{
    const unsigned int w = windowSize.x;
    const unsigned int h = windowSize.y;
    const sf::Vector2f pos = ball.shape.getPosition();
    const float rad = ball.shape.getRadius();
    
    if (pos.x - rad < 0
        || pos.x + rad > w)
        ball.velocity.x = -ball.velocity.x;

    if (pos.y - rad < 0
        || pos.y + rad > h)
        ball.velocity.y = -ball.velocity.y;

    float x = std::clamp(pos.x, rad, w - rad);
    float y = std::clamp(pos.y, rad, h - rad);

    ball.shape.setPosition({x, y});
}

void Crisis::handleBallsCollision(Ball& ball1, Ball& ball2)
{
    const sf::Vector2f delta = ball2.shape.getPosition() - ball1.shape.getPosition();
    const float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    const float minDist = ball1.shape.getRadius() + ball2.shape.getRadius();
                    
    if (dist >= minDist
        || dist <= 0.0f)
        return;
    
    const sf::Vector2f normal = delta / dist;
    const float overlap = (minDist - dist) * 0.5f;
    sf::Vector2f apos = ball1.shape.getPosition();
    sf::Vector2f bpos = ball2.shape.getPosition();

    apos -= normal * overlap;
    bpos += normal * overlap;

    ball1.shape.setPosition(apos);
    ball2.shape.setPosition(bpos);
        
    const sf::Vector2f relVel = ball2.velocity - ball1.velocity;
    const float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

    if (velAlongNormal > 0)
        return;

    const float impulse = -velAlongNormal;
    const sf::Vector2f impulseVec = impulse * normal;
        
    ball1.velocity -= impulseVec;
    ball2.velocity += impulseVec;
}

void Crisis::checkCellPairCollisions(const std::vector<int>& cellA, const std::vector<int>& cellB)
{
    for (const int i : cellA)
    {
        for (const int j : cellB)
        {
            if (i < j)
                handleBallsCollision(balls[i], balls[j]);
        }
    }
}
