#include "GameOfLife.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <random>

GameOfLife::GameOfLife()
{
    sf::RenderWindow window(sf::VideoMode({1200, 1200}), "Game of Life");

    if (!ImGui::SFML::Init(window))
        return;

    buildGrid();
    createVisibleGrid();

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
        showUi();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

void GameOfLife::updateGrid()
{
    bufferGrid.clear();
    
    for (int i = 0; i < rows * cols; ++i)
    {
        const int x = i % cols;
        const int y = i / cols;

        const int neighbors = countNeighbors(x, y);
        const bool aliveNow = grid.contains(i);
        bool nextState = aliveNow;

        if (aliveNow)
        {
            if (neighbors < 2
                || neighbors > 3)
                nextState = false;
        }
        else if (neighbors == 3)
            nextState = true;

        if (nextState)
            bufferGrid.insert(i);
    }

    grid.swap(bufferGrid);
}

int GameOfLife::countNeighbors(const int x, const int y)
{
    int count = 0;

    for (const int* offset : neighborOffsets)
    {
        const int nx = x + offset[0];
        const int ny = y + offset[1];

        if (nx >= 0
            && nx < cols
            && ny >= 0
            && ny < rows
            && grid.contains(ny * cols + nx))
            ++count;
    }

    return count;
}


void GameOfLife::drawGrid(sf::RenderWindow& window)
{
    for (int i = 0; i < rows * cols; ++i)
    {
        sf::Color c = grid.contains(i) ? alive : dead;
        const int v = i * 4;

        // Assign color to all 4 unique vertices, for the 2 triangles
        cellVertices[v + 0].color = c;
        cellVertices[v + 1].color = c;
        cellVertices[v + 2].color = c;
        cellVertices[v + 3].color = c;
    }

    window.draw(cellVertices);
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

void GameOfLife::showUi()
{
    ImGui::Begin("Debug info");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Highest FPS: %.1f", highestFps);
    ImGui::Text("Rows: %d  Cols: %d", rows, cols);
    ImGui::Text("Cells: %d", rows * cols);
    ImGui::Text("Triangles to draw: %d", rows * cols * 2);

    if (ImGui::Button("Restart Game of Life"))
        buildGrid();
    
    ImGui::End();
}

void GameOfLife::buildGrid()
{
    grid.clear();
    bufferGrid.clear();

    grid.reserve(rows * cols);
    bufferGrid.reserve(rows * cols);

    std::mt19937 gen(rd());
    std::uniform_real_distribution dist(0.f, 1.f);

    for (int i = 0; i < rows * cols; ++i)
    {
        if (dist(gen) < 0.5f)
            grid.insert(i);
    }
}

void GameOfLife::createVisibleGrid()
{
    cellVertices.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
    cellVertices.resize(rows * cols * 4);

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            const float px = x * cellSize;
            const float py = y * cellSize;
            const int index = (y * cols + x) * 4;

            // 4 vertices in a strip
            cellVertices[index + 0].position = {px, py};
            cellVertices[index + 1].position = {px + cellSize, py};
            cellVertices[index + 2].position = {px, py + cellSize};
            cellVertices[index + 3].position = {px + cellSize, py + cellSize};
        }
    }
}
