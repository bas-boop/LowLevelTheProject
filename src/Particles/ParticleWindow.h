#pragma once

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include "ParticleSystem.h"

class ParticleWindow {
public:
    ParticleWindow();
    void run();
private:
    sf::RenderWindow window;
    ParticleSystem particleSystem;
    const int particleCount = 200;
};

