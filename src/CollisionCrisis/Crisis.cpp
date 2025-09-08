#include "Crisis.h"

Crisis::Crisis()
{
    sf::RenderWindow window;
    window.create(sf::VideoMode({ 1920, 1080 }), "Our crisis window");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(window))
        return;

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        // Event Polling
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::ShowDemoWindow();

        // Render
        window.clear();

        ImGui::SFML::Render(window);

        window.display();
    }
}

Crisis::~Crisis()
{
    
}
