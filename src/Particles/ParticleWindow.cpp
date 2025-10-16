#include "ParticleWindow.h"
#include "ParticleSystem.h"

ParticleWindow::ParticleWindow()
{
    // 1. Create a window
    sf::RenderWindow window(sf::VideoMode({800, 800}), "Particle System Example");
    window.setFramerateLimit(60);

    // 2. Create the particle system and pass the window pointer
    ParticleSystem particleSystem(&window);

    // 3. Spawn some particles
    particleSystem.spawnParticles(200, sf::Vector2f(400, 400));

    // 4. Main game loop
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

        // Update
        particleSystem.update(deltaTime);

        // Render
        window.clear();
        particleSystem.render();
        window.display();
    }
}
