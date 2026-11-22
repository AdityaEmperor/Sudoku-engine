
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <optional>
#include <algorithm>

int main()
{
    // 1. Create window
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),"Application Launcher");

    // Load Starry Night background
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("Starry_Night.jpg"))
    {
        std::cout << "Failed to load Starry_Night.jpg!\\n";
        return 1;
    }
    sf::Sprite background(bgTexture);
    float scale = std::max(800.f / bgTexture.getSize().x,
                           600.f / bgTexture.getSize().y);
    background.setScale({scale, scale});
    background.setPosition({
        (800.f - bgTexture.getSize().x * scale) / 2.f,
        (600.f - bgTexture.getSize().y * scale) / 2.f
    });

    // 2. Load font
    sf::Font font;

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "Failed to load font!\n";
        return 1;
    }

    std::cout << "Font loaded successfully!\n";

    // 3. Create title
    sf::Text title(font, "SUDOKU LAUNCHER", 40);

    title.setPosition({200, 80});
    title.setFillColor(sf::Color(128, 81, 169, 150));
    title.setOutlineColor(sf::Color(0, 0, 0, 255));
    title.setOutlineThickness(1.8f);    

    // 4. Create first button
    sf::RectangleShape button1({300, 70});

    button1.setPosition({250, 220});
    button1.setFillColor(sf::Color(55, 30, 50, 200));
    // button1.setOutlineColor(sf::Color(0, 0, 0, 255));
    // button1.setOutlineThickness(1.2f);    

    // 5. Create second button
    sf::RectangleShape button2({300, 70});

    button2.setPosition({250, 330});
    button2.setFillColor(sf::Color(55, 30, 50, 200));
    // button2.setOutlineColor(sf::Color(0, 0, 0, 255));
    // button2.setOutlineThickness(1.2f);

    // 6. Create button text
    sf::Text text1(font, "Sudoku Solver", 24);
    text1.setFillColor(sf::Color(255, 239, 255));
    text1.setPosition({320, 240});

    sf::Text text2(font, "Sudoku Game", 24);
    text2.setFillColor(sf::Color(255, 239, 205));
    text2.setPosition({320, 350});

    // 7. Main loop
    while (window.isOpen())
    {
        // Event handling
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Mouse click detection
            if (const auto* mouse =
                event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2i mousePosition =
                        sf::Mouse::getPosition(window);

                    sf::Vector2f mousePos(mousePosition);

                    // First button
                    if (button1.getGlobalBounds().contains(mousePos))
                    {
                        std::system(
                            "start \"\" \"sudoku_solver_app.exe\""
                        );
                    }

                    // Second button
                    if (button2.getGlobalBounds().contains(mousePos))
                    {
                        std::system(
                            "start \"\" \"gameApp.exe\""
                        );
                    }
                }
            }
        }

        // Drawing
        window.clear(sf::Color::Black);
        window.draw(background);

        window.draw(title);

        window.draw(button1);
        window.draw(button2);

        window.draw(text1);
        window.draw(text2);

        window.display();
    }

    return 0;
}
