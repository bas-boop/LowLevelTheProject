#pragma once
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <random>
#include <unordered_set>

class GameOfLife
{
public:
    GameOfLife();

private:
    const int rows = 600;
    const int cols = 600;
    const int cellSize = 2;

    const sf::Color alive = {76, 58, 98};
    const sf::Color dead = {35, 35, 54};

    float fps;
    float highestFps;
    int frameCount;

    std::unordered_set<int> grid;
    std::unordered_set<int> bufferGrid;

    const int neighborOffsets[8][2] = {
        {-1, -1}, { 0, -1}, { 1, -1},
        {-1,  0},           { 1,  0},
        {-1,  1}, { 0,  1}, { 1,  1}
    };
    
    std::random_device rd;
    
    void updateGrid();
    int countNeighbors(int x, int y);

    void buildGrid();
    void drawGrid(sf::RenderWindow& window);
    
    void updateFps(sf::Time dt);
    void showUi();
};