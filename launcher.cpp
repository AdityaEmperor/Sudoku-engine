
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <optional>

int main()
{
    // 1. Create window
    //sf::Window window(sf::VideoMode({800, 600}), "Application Launcher");
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),"Application Launcher");

    // 2. Load font
    sf::Font font;

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "Failed to load font!\n";
        return 1;
    }

    std::cout << "Font loaded successfully!\n";

    // 3. Create title
    sf::Text title(font, "SUDOKU LAUNCHER", 32);

    title.setPosition({230, 80});
    title.setFillColor(sf::Color::White);

    // 4. Create first button
    sf::RectangleShape button1({300, 70});

    button1.setPosition({250, 220});
    button1.setFillColor(sf::Color(40, 100, 220));

    // 5. Create second button
    sf::RectangleShape button2({300, 70});

    button2.setPosition({250, 330});
    button2.setFillColor(sf::Color(30, 160, 90));

    // 6. Create button text
    sf::Text text1(font, "Sudoku Solver", 24);
    text1.setPosition({320, 240});

    sf::Text text2(font, "Sudoku Game", 24);
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
        window.clear(sf::Color(25, 25, 35));

        window.draw(title);

        window.draw(button1);
        window.draw(button2);

        window.draw(text1);
        window.draw(text2);

        window.display();
    }

    return 0;
}
