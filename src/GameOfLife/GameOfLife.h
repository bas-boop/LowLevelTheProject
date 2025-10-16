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
    int rows;
    int cols;
    int cellSize;

    const sf::Color alive = {76, 58, 98};
    const sf::Color dead = {35, 35, 54};

    float fps;
    float highestFps;
    int frameCount;

    std::unordered_set<int> grider;
    std::unordered_set<int> bufferGrider;

    std::random_device rd;
    
    void updateGrid();
    int countNeighbors(int x, int y);

    void buildGrid();
    void drawGrid(sf::RenderWindow& window);
    
    void updateFps(sf::Time dt);
    void showUi();
};