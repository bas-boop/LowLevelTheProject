#include "ParticleWindow.h"
#include "ParticleSystem.h"

ParticleWindow::ParticleWindow()
    : window(sf::VideoMode({800, 800}), "Particle System Example"),
      particleSystem(&window, particleCount)
{
    ImGui::SFML::Init(window);
    particleSystem.spawnParticles(particleCount, sf::Vector2f(400, 400));
}

void ParticleWindow::run()
{
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
