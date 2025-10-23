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

    grid.assign(rows * cols, 0);
    bufferGrid.assign(rows * cols, 0);

    poolThreadCount = static_cast<int>(std::max(1u, std::thread::hardware_concurrency())); // 32 on my pc
    //poolThreadCount = 1;
    threadPool = std::make_unique<WorkerThreadPool>(static_cast<size_t>(poolThreadCount));

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
        showUi();

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}

inline int GameOfLife::countNeighbors(const int x, const int y) const
{
    int count = 0;

    for (const auto& offs : neighborOffsets)
    {
        const int nx = x + offs[0];
        const int ny = y + offs[1];

        if (nx >= 0
            && nx < cols
            && ny >= 0
            && ny < rows)
            count += grid[ny * cols + nx];
    }

    return count;
}

void GameOfLife::updateGrid()
{
    std::ranges::fill(bufferGrid, 0);
    std::atomic<int> rowsCompleted{0};

    for (int y = 0; y < rows; ++y)
    {
        threadPool->enqueue([this, y, &rowsCompleted]()
        {
            const int rowStart = y * cols;
            
            for (int x = 0; x < cols; ++x)
            {
                const int i = rowStart + x;
                int neighbors = 0;
                
                for (const auto& offs : neighborOffsets)
                {
                    const int nx = x + offs[0];
                    const int ny = y + offs[1];
                    
                    if (nx >= 0 && nx < cols && ny >= 0 && ny < rows)
                        neighbors += grid[ny * cols + nx];
                }

                const bool aliveNow = grid[i] != 0;
                bool nextState = aliveNow;
                
                if (aliveNow)
                {
                    if (neighbors < 2 || neighbors > 3)
                        nextState = false;
                }
                else
                {
                    if (neighbors == 3)
                        nextState = true;
                }

                bufferGrid[i] = nextState ? 1 : 0;
            }
            
            rowsCompleted++;
        });
    }

    // Wait for all rows to complete.
    while (rowsCompleted.load(std::memory_order_acquire) < rows)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    grid.swap(bufferGrid);
}

void GameOfLife::drawGrid(sf::RenderWindow& window)
{
    sf::RectangleShape cellShape(sf::Vector2f(cellSize - 1, cellSize - 1));
    
    for (int i = 0; i < rows * cols; ++i)
    {
        const int x = i % cols;
        const int y = i / cols;
        
        cellShape.setPosition({(float)x * cellSize, (float)y * cellSize});
        cellShape.setFillColor(grid[i] ? alive : dead);
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

void GameOfLife::showUi()
{
    ImGui::Begin("Debug info");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Pool threads: %d", poolThreadCount);
    ImGui::Text("Rows: %d  Cols: %d", rows, cols);

    if (ImGui::Button("Restart"))
        buildGrid();
    
    ImGui::End();
}

void GameOfLife::buildGrid()
{
    grid.clear();
    bufferGrid.clear();

    grid.reserve(rows * cols);
    bufferGrid.reserve(rows * cols);
    
    //grid.assign(rows * cols, 0);
    //bufferGrid.assign(rows * cols, 0);

    std::mt19937 gen(rd());
    std::uniform_real_distribution dist(0.f, 1.f);

    for (int i = 0; i < rows * cols; ++i)
    {
        if (dist(gen) < 0.5f)
            grid[i] = 1;
    }
}
