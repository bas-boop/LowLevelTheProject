#include "GameOfLife.h"

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <random>

GameOfLife::GameOfLife()
{
    sf::RenderWindow window(sf::VideoMode({1800, 1200}), "Game of Life");

    if (!ImGui::SFML::Init(window))
        return;

    grid.assign(rows * cols, 0);
    bufferGrid.assign(rows * cols, 0);

    poolThreadCount = static_cast<int>(std::max(1u, std::thread::hardware_concurrency())); // 32 on my pc
    //poolThreadCount = 1;
    threadPool = std::make_unique<WorkerThreadPool>(static_cast<size_t>(poolThreadCount));

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

    //const int rowsPerTask = 5;
    const int rowsPerTask = poolThreadCount;
    const int taskCount = (rows + rowsPerTask - 1) / rowsPerTask;
    std::atomic<int> tasksCompleted{0};

    for (int taskIndex = 0; taskIndex < taskCount; ++taskIndex)
    {
        threadPool->enqueue([this, taskIndex, rowsPerTask, &tasksCompleted]()
        {
            const int yStart = taskIndex * rowsPerTask;
            const int yEnd   = std::min(rows, yStart + rowsPerTask);

            for (int y = yStart; y < yEnd; ++y)
            {
                const int rowStart = y * cols;
                
                for (int x = 0; x < cols; ++x)
                {
                    int neighbors = 0;
                    
                    for (auto& offs : neighborOffsets)
                    {
                        const int nx = x + offs[0];
                        const int ny = y + offs[1];
                        
                        if(nx >= 0 && nx < cols && ny >= 0 && ny < rows)
                            neighbors += grid[ny * cols + nx];
                    }

                    const bool alive = grid[rowStart + x] != 0;
                    bufferGrid[rowStart + x] = (neighbors == 3 || (alive && neighbors == 2)) ? 1 : 0;
                }
            }
            
            tasksCompleted.fetch_add(1, std::memory_order_release);
        });
    }

    // Wait for all tasks to finish
    while (tasksCompleted.load(std::memory_order_acquire) < taskCount)
    {
        std::this_thread::yield();
    }
    
    grid.swap(bufferGrid);
}


void GameOfLife::drawGrid(sf::RenderWindow& window)
{
    for (int i = 0; i < rows * cols; ++i)
    {
        sf::Color c = grid[i] ? alive : dead;
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
            highestFps = fps;
    }
}

void GameOfLife::showUi()
{
    ImGui::Begin("Debug info");
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Highest FPS: %.1f", highestFps);
    ImGui::Text("Pool threads: %d", poolThreadCount);
    ImGui::Text("Rows: %d  Cols: %d", rows, cols);
    ImGui::Text("Cells: %d", rows * cols);
    ImGui::Text("Triangles to draw: %d", rows * cols * 2);

    if (ImGui::Button("Restart Game of Life"))
        buildGrid();
    
    ImGui::End();
}

void GameOfLife::buildGrid()
{
    grid.assign(rows * cols, 0);
    bufferGrid.assign(rows * cols, 0);

    std::mt19937 gen(rd());
    std::uniform_real_distribution dist(0.f, 1.f);

    for (int i = 0; i < rows * cols; ++i)
    {
        if (dist(gen) < 0.5f)
            grid[i] = 1;
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
