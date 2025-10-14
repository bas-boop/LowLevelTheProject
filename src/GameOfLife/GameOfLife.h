#pragma once
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <random>
#include <vector>

class GameOfLife
{
public:
    GameOfLife();

private:
    int rows;
    int cols;
    int cellSize;

    const sf::Color alive = {76, 58, 98};
    const sf::Color dead = {35, 35, 54};

    float fps;
    float highestFps;
    int frameCount;

    std::vector<std::vector<bool>> grid;
    std::vector<std::vector<bool>> bufferGrid;

    std::random_device rd;
    
    void updateGrid();
    int countNeighbors(int x, int y);
    void drawGrid(sf::RenderWindow& window);
    void updateFps(sf::Time dt);
    void showFps();
    void buildGrid();
};
