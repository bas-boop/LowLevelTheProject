#pragma once
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <random>
#include <vector>
#include <thread>

#include "WorkerThreadPool.h"

class GameOfLife
{
public:
    GameOfLife();

private:
    // original number, 330 FPS
    //const int rows = 600;
    //const int cols = 600;
    //const int cellSize = 2;
    
    // testing during making multithreading, 50 FPS
    const int rows = 1200;
    const int cols = 1800;
    const int cellSize = 1;
    
    // absolute limits, 1 FPS
    //const int rows = 5000;
    //const int cols = 10000;
    //const float cellSize = 0.1f;

    const sf::Color alive = {76, 58, 98};
    const sf::Color dead = {35, 35, 54};

    float fps = 0.f;
    float highestFps = 0.f;
    int frameCount = 0;

    std::vector<uint8_t> grid;
    std::vector<uint8_t> bufferGrid;
    sf::VertexArray cellVertices;

    const int neighborOffsets[8][2] = {
        {-1, -1}, { 0, -1}, { 1, -1},
        {-1,  0},           { 1,  0},
        {-1,  1}, { 0,  1}, { 1,  1}
    };

    std::random_device rd;

    // threading
    std::unique_ptr<WorkerThreadPool> threadPool;
    int poolThreadCount = 0; // set in ctor

    void updateGrid();
    inline int countNeighbors(int x, int y) const;

    void buildGrid();
    void drawGrid(sf::RenderWindow& window);
    void createVisibleGrid();

    void updateFps(sf::Time dt);
    void showUi();
};
