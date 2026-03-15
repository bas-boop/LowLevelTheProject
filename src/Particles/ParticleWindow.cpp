#include "ParticleWindow.h"
#include "ParticleSystem.h"
#include <imgui.h>
#include <numeric>
#include <imgui-SFML.h>

ParticleWindow::ParticleWindow()
    : window(sf::VideoMode({800, 800}), "Perfect particle system!"),
      particleSystem(&window, particleCount)
{
    ImGui::SFML::Init(window);
    particleSystem.spawnParticles(particleCount, sf::Vector2f(400, 400));
}

static float dequeAvg(const std::deque<float>& d)
{
    if (d.empty())
        return 0.f;

    return std::accumulate(d.begin(), d.end(), 0.f) / static_cast<float>(d.size());
}

static std::vector<float> toVec(const std::deque<float>& d)
{
    return { d.begin(), d.end() };
}

void ParticleWindow::drawMetricsOverlay()
{
    ImGui::SetNextWindowPos({10, 10}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({280, 0}, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.75f);
    ImGui::Begin("Performance", nullptr);

    ImGui::Text("Particles : %zu", particleSystem.getParticleCount());
    ImGui::Text("FPS       : %6.1f  (avg %5.1f)", metrics.fps, dequeAvg(metrics.fpsHistory));
    ImGui::Text("Update    : %5.2f ms  (avg %5.2f)", metrics.updateMs, dequeAvg(metrics.updateHistory));

    {
        const std::vector<float> v = toVec(metrics.fpsHistory);
        ImGui::PlotLines("##fps", v.data(), static_cast<int>(v.size()), 0, "FPS", 0.f, 120.f, {262, 40});
    }
    
    ImGui::End();
}

void ParticleWindow::run()
{
    sf::Clock frameClock; // to count the frame time
    sf::Clock phaseClock; // update & render timing

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();
        }

        const float deltaTime = frameClock.restart().asSeconds();
        metrics.frameMs = deltaTime * 1000.f;
        metrics.fps = 1.f / deltaTime;

        phaseClock.restart();
        ImGui::SFML::Update(window, sf::seconds(deltaTime));
        particleSystem.update(deltaTime);
        metrics.updateMs = phaseClock.restart().asMilliseconds();

        phaseClock.restart();
        window.clear();
        particleSystem.render();
        drawMetricsOverlay();
        ImGui::SFML::Render(window);
        window.display();
        metrics.push(metrics.fps, metrics.updateMs);
    }

    ImGui::SFML::Shutdown();
}

void ParticleWindow::runOld()
{
    sf::RenderWindow window(sf::VideoMode({800, 800}), "OLD Particle System Example");
    window.setFramerateLimit(60);
    ParticleSystem particleSystem(&window);
    particleSystem.spawnParticles(200, sf::Vector2f(400, 400));

    sf::Clock clock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        particleSystem.update(deltaTime);
        window.clear();
        particleSystem.render();
        window.display();
    }

    ImGui::SFML::Shutdown();
}
