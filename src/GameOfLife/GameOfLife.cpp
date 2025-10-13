#include "GameOfLife.h"

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <random>

GameOfLife::GameOfLife() :
    rows(60),
    cols(60),
    cellSize(15),
    running(true),
    fps(0),
    highestFps(0),
    frameCount(0)
{
    sf::RenderWindow window(sf::VideoMode({900, 900}), "Game of Life");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(window))
        return;

    auto gen = std::mt19937(rd());
    
    bool a = false;
    auto b = std::uniform_real_distribution<float>(0.f, 1.f);

    if (b(gen) < 0.5f)
        a = true;
    
    grid.resize(rows, std::vector<bool>(cols, a));
    bufferGrid = grid;

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();
            
            /*if (auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    int x = mouse->position.x / cellSize;
                    int y = mouse->position.y / cellSize;
                    if (x >= 0 && x < cols && y >= 0 && y < rows)
                        grid[y][x] = !grid[y][x];
                }
            }*/
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        updateGrid();
        updateFps(dt);
        
        window.clear();
        drawGrid(window);
        showFps();
        //showControls();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

void GameOfLife::updateGrid()
{
    bufferGrid = grid;

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            int neighbors = countNeighbors(x, y);
            
            if (grid[y][x])
            {
                // Alive
                if (neighbors < 2
                    || neighbors > 3)
                    bufferGrid[y][x] = false;
            }
            else
            {
                // Dead
                if (neighbors == 3)
                    bufferGrid[y][x] = true;
            }
        }
    }
    
    grid.swap(bufferGrid);
}

int GameOfLife::countNeighbors(int x, int y)
{
    int count = 0;
    
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;
            
            if (nx >= 0 && nx < cols
                && ny >= 0 && ny < rows
                && grid[ny][nx])
                count++;
        }
    }
    
    return count;
}

void GameOfLife::drawGrid(sf::RenderWindow& window)
{
    sf::RectangleShape cellShape(sf::Vector2f(cellSize - 1, cellSize - 1));
    
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            cellShape.setPosition({(float) x * cellSize, (float) y * cellSize});
            cellShape.setFillColor(grid[y][x] ? sf::Color::White : sf::Color(40, 40, 40));
            window.draw(cellShape);
        }
    }
}

void GameOfLife::updateFps(sf::Time dt)
{
    if (dt.asSeconds() > 0.f)
    {
        fps = 1.f / dt.asSeconds();
        frameCount++;
        if (frameCount > 5 && fps > highestFps)
        {
            highestFps = fps;
            std::cout << "Highest fps: " << highestFps << "\n";
        }
    }
}

void GameOfLife::showFps()
{
    ImGui::Begin("FPS Counter");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::End();
}

void GameOfLife::showControls()
{
    ImGui::Begin("Controls");
    if (ImGui::Button(running ? "Pause" : "Play"))
        running = !running;
    ImGui::SameLine();
    if (ImGui::Button("Step"))
        updateGrid();
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        for (auto& row : grid)
            std::fill(row.begin(), row.end(), false);
    }
    ImGui::End();
}
