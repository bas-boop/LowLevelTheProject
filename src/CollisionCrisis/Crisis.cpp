#include "Crisis.h"

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

Crisis::Crisis()
{
    sf::RenderWindow window;
    window.create(sf::VideoMode({ 900, 900 }), "Our crisis window");
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(window))
        return;

    gen = std::mt19937(rd());
    posDist = std::uniform_real_distribution<float>(5.0f, 895.0f);
    velDist = std::uniform_real_distribution<float>(-200.0f, 200.0f);
    colorDist = std::uniform_int_distribution<int>(0, 255);
    radiusDist = std::uniform_real_distribution<float>(2.5f, 7.5f);

    betterBalls = std::set<Ball>();
    
    // Generate random balls
    for (int i = 0; i < 2500; ++i)
    {
        sf::Color randomColor(colorDist(gen), colorDist(gen), colorDist(gen));

        if (i == 0)
            randomColor = sf::Color::Red;

        betterBalls.emplace(
            i, // id
            posDist(gen), posDist(gen), // position
            radiusDist(gen), // radius
            randomColor, // color
            velDist(gen), velDist(gen) // velocity
        );
        
        /*balls.emplace_back(
            posDist(gen), posDist(gen), // position
            radiusDist(gen), // radius
            randomColor, // color
            velDist(gen), velDist(gen) // velocity
        );*/
    }

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        // Event Polling
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        // Render
        window.clear();

        fps = 1.0f / dt.asSeconds();

        updateBalls(window.getSize(), dt.asSeconds());
        drawBalls(window);
        ImGui::Begin("FPS Counter"); ImGui::Text("FPS: %.1f", fps); ImGui::End(); // fps is 100.000, something is going wrong
        ImGui::SFML::Render(window);

        window.display();
    }
}

Crisis::~Crisis()
{
    
}

void Crisis::updateBalls(const sf::Vector2u& windowSize, float deltaTime)
{
    // Update positions
    /*for (auto& ball : balls)
    {
        ball.shape.move(ball.velocity * deltaTime);
    }*/

    std::set<Ball> newSet;

    std::ranges::for_each(betterBalls, [&](const Ball& ball) {
        Ball newBall = ball;
        newBall.shape.move(newBall.velocity * deltaTime);
        newSet.insert(std::move(newBall));
    });

    betterBalls = std::move(newSet);

    std::vector<Ball> tempBalls(betterBalls.begin(), betterBalls.end());

    // collision detection
    std::ranges::for_each(tempBalls, [&](Ball& ball1) {
        std::ranges::for_each(tempBalls, [&](Ball& ball2) {
            if (ball1.id == ball2.id)
                return;

            const sf::Vector2f pos1 = ball1.shape.getPosition();
            const sf::Vector2f pos2 = ball2.shape.getPosition();
            const float radius1 = ball1.shape.getRadius();
            const float radius2 = ball2.shape.getRadius();

            sf::Vector2f delta = pos2 - pos1;
            const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            const float minDistance = radius1 + radius2;

            if (distance < minDistance && distance > 0) {
                // Normalize collision vector
                sf::Vector2f normal = delta / distance;

                // Separate balls
                float overlap = minDistance - distance;
                sf::Vector2f separation = normal * (overlap * 0.5f);
                ball1.shape.setPosition(pos1 - separation);
                ball2.shape.setPosition(pos2 + separation);

                // Relative velocity
                sf::Vector2f relativeVel = ball2.velocity - ball1.velocity;
                float velAlongNormal = relativeVel.x * normal.x + relativeVel.y * normal.y;

                if (velAlongNormal > 0)
                    return;

                float restitution = 0.0f; // bounce factor
                float impulse = -(1 + restitution) * velAlongNormal;

                sf::Vector2f impulseVector = impulse * normal;
                ball1.velocity -= impulseVector;
                ball2.velocity += impulseVector;
            }
        });
    });

    // rebuild the set with updated balls
    betterBalls.clear();
    betterBalls.insert(tempBalls.begin(), tempBalls.end());
    
    // Handle ball-to-ball collisions
    /*for (size_t i = 0; i < balls.size(); ++i)
    {
        for (size_t j = i + 1; j < balls.size(); ++j)
        {
            Ball& ball1 = balls[i];
            Ball& ball2 = balls[j];
            sf::Vector2f pos1 = ball1.shape.getPosition();
            sf::Vector2f pos2 = ball2.shape.getPosition();
            float radius1 = ball1.shape.getRadius();
            float radius2 = ball2.shape.getRadius();
            // Calculate distance between centers
            sf::Vector2f delta = pos2 - pos1;
            float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            float minDistance = radius1 + radius2;
            
            if (distance < minDistance && distance > 0)
            {
                // Normalize collision vector
                sf::Vector2f normal = delta / distance;
                // Separate balls to prevent overlap
                float overlap = minDistance - distance;
                sf::Vector2f separation = normal * (overlap * 0.5f);
                ball1.shape.setPosition(pos1 - separation);
                ball2.shape.setPosition(pos2 + separation);
                // Calculate relative velocity
                sf::Vector2f relativeVel = ball2.velocity - ball1.velocity;
                float velAlongNormal = relativeVel.x * normal.x + relativeVel.y * normal.y;
                // Don't resolve if velocities are separating
                if (velAlongNormal > 0) continue;
                // Apply collision response (elastic collision)
                float restitution = 0.0f; // Bounce factor (0 = no bounce, 1 = perfect bounce)
                float impulse = -(1 + restitution) * velAlongNormal;
                // Assume equal mass for simplicity
                sf::Vector2f impulseVector = impulse * normal;
                ball1.velocity -= impulseVector;
                ball2.velocity += impulseVector;
            }
        }
    }*/
    
    // Handle wall collisions

    std::vector<Ball> wallBalls(betterBalls.begin(), betterBalls.end());

    std::ranges::for_each(wallBalls, [&](Ball& ball)
    {
        sf::Vector2f pos = ball.shape.getPosition();
        float radius = ball.shape.getRadius();

        // Bounce off vertical walls
        if (pos.x - radius <= 0 || pos.x + radius >= windowSize.x) {
            ball.velocity.x = -ball.velocity.x;

            if (pos.x - radius <= 0) {
                ball.shape.setPosition(sf::Vector2f(radius, pos.y));
            } else {
                ball.shape.setPosition(sf::Vector2f(windowSize.x - radius, pos.y));
            }
        }

        // Bounce off horizontal walls
        if (pos.y - radius <= 0 || pos.y + radius >= windowSize.y) {
            ball.velocity.y = -ball.velocity.y;

            if (pos.y - radius <= 0) {
                ball.shape.setPosition(sf::Vector2f(pos.x, radius));
            } else {
                ball.shape.setPosition(sf::Vector2f(pos.x, windowSize.y - radius));
            }
        }
    });

    // rebuild the set
    betterBalls.clear();
    betterBalls.insert(wallBalls.begin(), wallBalls.end());
    
    /*for (auto& ball : balls)
    {
        sf::Vector2f pos = ball.shape.getPosition();
        float radius = ball.shape.getRadius();
        // Bounce off walls
        if (pos.x - radius <= 0 || pos.x + radius >= windowSize.x)
        {
            ball.velocity.x = -ball.velocity.x;
            // Clamp position to prevent sticking
            if (pos.x - radius <= 0)
            {
                ball.shape.setPosition(sf::Vector2f(radius, pos.y));
            }
            else
            {
                ball.shape.setPosition(sf::Vector2f(windowSize.x - radius,
                pos.y));
            }
        }
        if (pos.y - radius <= 0 || pos.y + radius >= windowSize.y)
        {
            ball.velocity.y = -ball.velocity.y;
            
            // Clamp position to prevent sticking
            if (pos.y - radius <= 0)
            {
                ball.shape.setPosition(sf::Vector2f(pos.x, radius));
            }
            else
            {
                ball.shape.setPosition(sf::Vector2f(pos.x, windowSize.y - radius));
            }
        }
    }*/
}

void Crisis::drawBalls(sf::RenderWindow& window) const
{
    std::ranges::for_each(betterBalls, [&](const Ball& ball)
        {
            window.draw(ball.shape);

        return;
        
            if (ball.shape.getFillColor() == sf::Color::Red)
                std::cout << ball.shape.getPosition().x << " " << ball.shape.getPosition().y << '\n';

        });
    
    return;
    
    for (const auto& ball : balls) {
        window.draw(ball.shape);
    }
}
