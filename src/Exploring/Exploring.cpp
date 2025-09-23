#include "Exploring.h"

#include <algorithm>
#include <functional>
#include <ranges>
#include <string>
#include <vector>
#include <iostream>

Exploring::Exploring()
{
    // gebruik functies uit <algorithm> en <functional> om de volgende opdrachten uit te voeren:

    std::cout << std::endl;
    
    // splits de vector in 2 nieuwe vectoren: 1 met alles wat alfabetisch voor 'purple' komt, 1 met alles er na
    {
        std::vector<std::string> colours{"red", "green", "white", "blue", "orange", "green", "orange", "black", "purple"};

        std::ranges::sort(colours);

        std::vector<std::string> before;
        std::vector<std::string> after;

        // copy everything < "purple"
        std::ranges::copy(
            colours | std::views::filter([](auto const& c) { return c < "purple"; }),
            std::back_inserter(before)
        );

        // copy everything > "purple"
        std::ranges::copy(
            colours | std::views::filter([](auto const& c) { return c > "purple"; }),
            std::back_inserter(after)
        );

        std::cout << "Before: ";
        std::ranges::copy(before, std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << "\nAfter: ";
        std::ranges::copy(after, std::ostream_iterator<std::string>(std::cout, " "));
        std::cout << "\nsplit vector\n\n";
    }
    
    // maak alle elementen UPPERCASE
    {
        std::vector<std::string> colours{"red", "green", "white", "blue", "orange", "green", "orange", "black", "purple"};
        auto result = std::ranges::for_each(colours, [](std::string& colour) {
            std::ranges::transform(colour, colour.begin(), ::toupper);
            std::cout << colour << "\n";
        });

        std::cout << "\nupper case\n\n";
    }

    // verwijder alle dubbele elementen
    {
        std::vector<std::string> colours{"red", "green", "white", "blue", "orange", "green", "orange", "black", "purple"};

        std::ranges::sort(colours);

        auto last = std::ranges::unique(colours);

        colours.erase(last.begin(), last.end());
        
        for (auto const& colour : colours) {
            std::cout << colour << "\n";
        }
        
        std::cout << "\nno duplicates\n\n";
    }
return;

    // verwijder alle negatieve elementen
    {
        std::vector<double> numbers{10, 324422, 6, -23, 234.5, 654.1, 3.1242, -9.23, 635};
    }

    // bepaal voor alle elementen of ze even of oneven zijn
    {
        std::vector numbers{10, 324422, 6, -23, 234, 654, 3, -9, 635};
    }

    // bepaal de som, het gemiddelde, en het product van alle getallen te berekenen
    {
        std::vector<double> numbers{10, 324422.1, 6, -23, 234.5, 654.1, 3.1242, -9.23, 635};
    }
}

Exploring::~Exploring()
{
    
}
