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
    window.create(sf::VideoMode({ 2000, 1200 }), "Our crisis window");
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(window))
        return;

    gen = std::mt19937(rd());
    posDist = std::uniform_real_distribution<float>(5.0f, 1195.0f);
    velDist = std::uniform_real_distribution<float>(-200.0f, 200.0f);
    colorDist = std::uniform_int_distribution<int>(0, 255);
    radiusDist = std::uniform_real_distribution<float>(2.5f, 7.5f);

    balls = std::vector<Ball>();
    balls.reserve(ballAmount);
    grid.reserve(ballAmount);
    
    for (int i = 0; i < ballAmount; ++i)
    {
        const float x = posDist(gen);
        const float y = posDist(gen);
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

        if (dt.asSeconds() > 0.f)
        {
            fps = 1.f / dt.asSeconds();
            frameCount++;

            // Skip the first few frames to avoid artificially high FPS spikes
            if (frameCount > 5 && fps > highestFps)
            {
                highestFps = fps;
                std::cout << "Highest fps: " << highestFps << "\n";
            }
        }
        
        updateBalls(window.getSize(), dt.asSeconds());
        drawBalls(window);
        
        ImGui::Begin("FPS Counter");
        ImGui::Text("FPS: %.1f", fps);
        ImGui::End();
        
        ImGui::SFML::Render(window);

        window.display();
    }
}

Crisis::~Crisis()
{
    
}

void Crisis::updateBalls(const sf::Vector2u& windowSize, float deltaTime)
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
    const float W = windowSize.x, H = windowSize.y;

    // Wall collisions
    for (auto& b : balls)
    {
        auto pos = b.shape.getPosition();
        auto rad = b.shape.getRadius();
        
        if (pos.x - rad < 0 || pos.x + rad > W)
            b.velocity.x = -b.velocity.x;
        if (pos.y - rad < 0 || pos.y + rad > H)
            b.velocity.y = -b.velocity.y;

        float x = std::clamp(pos.x, rad, W - rad);
        float y = std::clamp(pos.y, rad, H - rad);

        pos = {x, y};
        b.shape.setPosition(pos);
    }

    for (auto& [key, indices] : grid)
    {
        const int cellX = key % 10000;
        const int cellY = key / 10000;

        for (auto [ox, oy] : neighborOffsets)
        {
            const int nx = cellX + ox;
            const int ny = cellY + oy;
            int neighbor_key = ny * 10000 + nx;
            
            if (!grid.contains(neighbor_key))
                continue;

            auto& other = grid[neighbor_key];
            
            for (const int i : indices)
            {
                for (const int j : other)
                {
                    if (i >= j)
                        continue;
                    
                    auto& a = balls[i];
                    auto& b = balls[j];

                    const sf::Vector2f delta = b.shape.getPosition() - a.shape.getPosition();
                    const float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

                    if (const float minDist = a.shape.getRadius() + b.shape.getRadius(); dist < minDist && dist > 0.f)
                    {
                        const sf::Vector2f normal = delta / dist;
                        const float overlap = (minDist - dist) * 0.5f;
                        sf::Vector2f apos = a.shape.getPosition();
                        sf::Vector2f bpos = b.shape.getPosition();

                        apos -= normal * overlap;
                        bpos += normal * overlap;

                        a.shape.setPosition(apos);
                        b.shape.setPosition(bpos);


                        const sf::Vector2f relVel = b.velocity - a.velocity;
                        const float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

                        if (velAlongNormal > 0)
                            continue;

                        const float impulse = -velAlongNormal;
                        const sf::Vector2f impulseVec = impulse * normal;
                        a.velocity -= impulseVec;
                        b.velocity += impulseVec;
                    }
                }
            }
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
        int key = cellY * 10000 + cellX;
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