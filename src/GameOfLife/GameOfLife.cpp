#include "GameOfLife.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <random>

GameOfLife::GameOfLife() :
    rows(600),
    cols(600),
    cellSize(2)
{
    sf::RenderWindow window(sf::VideoMode({1200, 1200}), "Game of Life");

    if (!ImGui::SFML::Init(window))
        return;

    buildGrid();

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

        updateFps(dt);
        updateGrid();
        
        window.clear();
        drawGrid(window);
        showFps();

        if (ImGui::Button("Restart"))
            buildGrid();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

void GameOfLife::updateGrid()
{    
    for (int i = 0; i < rows * cols; ++i)
    {
        const int x = i % cols;
        const int y = i / cols;

        const int neighbors = countNeighbors(x, y);
        const bool aliveNow = grid[i];
        bool nextState = aliveNow;

        if (aliveNow)
        {
            if (neighbors < 2
                || neighbors > 3)
                nextState = false;
        }
        else if (neighbors == 3)
            nextState = true;

        bufferGrid[i] = nextState;
    }

    grid.swap(bufferGrid);
}

int GameOfLife::countNeighbors(const int x, const int y)
{
    int count = 0;
    
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0
                && dy == 0)
                continue;

            const int nx = x + dx;
            const int ny = y + dy;

            if (nx >= 0
                && nx < cols
                && ny >= 0
                && ny < rows
                && grid[ny * cols + nx])
                count++;
        }
    }
    
    return count;
}


void GameOfLife::drawGrid(sf::RenderWindow& window)
{
    sf::RectangleShape cellShape(sf::Vector2f(cellSize - 1, cellSize - 1));
    
    for (int i = 0; i < rows * cols; ++i)
    {
        const int x = i % cols;
        const int y = i / cols;
        
        cellShape.setPosition({(float) x * cellSize, (float) y * cellSize});
        cellShape.setFillColor(grid[y * cols + x] ? alive : dead);
        window.draw(cellShape);
    }
}

void GameOfLife::updateFps(sf::Time dt)
{
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
}

void GameOfLife::showFps()
{
    ImGui::Begin("FPS Counter");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::End();
}

void GameOfLife::buildGrid()
{
    grid = std::make_unique<bool[]>(rows * cols);
    bufferGrid = std::make_unique<bool[]>(rows * cols);

    std::mt19937 gen(rd());
    std::uniform_real_distribution dist(0.f, 1.f);

    for (int i = 0; i < rows * cols; ++i)
        grid[i] = dist(gen) < 0.5f;
}

