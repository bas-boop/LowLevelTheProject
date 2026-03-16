#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include "ParticleSystem.h"

struct PerformanceMetrics {
    float updateMs = 0.f;
    float renderMs = 0.f;
    float frameMs = 0.f;
    float fps = 0.f;

    static constexpr size_t HISTORY = 120;
    std::deque<float> fpsHistory;
    std::deque<float> updateHistory;
    std::deque<float> renderHistory;

    void push(const float newFps, const float newUpdate, const float newRender)
    {
        auto pushDeque = [](std::deque<float>& d, const float v)
        {
            if (d.size() >= HISTORY) d.pop_front();
            d.push_back(v);
        };
        
        pushDeque(fpsHistory,    newFps);
        pushDeque(updateHistory, newUpdate);
        pushDeque(renderHistory, newRender);
    }
};

class ParticleWindow {
public:
    ParticleWindow();
    void run();
    void runOld();
    
private:
    sf::RenderWindow window;
    ParticleSystem particleSystem;
    const int particleCount = 200;
    
    PerformanceMetrics metrics;

    void drawMetricsOverlay();
};