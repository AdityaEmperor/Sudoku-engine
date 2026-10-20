#include "sudokuGame.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace
{
    constexpr unsigned int WINDOW_WIDTH = 1000;
    constexpr unsigned int WINDOW_HEIGHT = 960;
    constexpr float GRID_X = 70.f;
    constexpr float GRID_Y = 155.f;
    constexpr float GRID_SIZE = 750.f;

    const sf::Color BACKGROUND(245, 247, 250);
    const sf::Color GRID_LINE(145, 153, 165);
    const sf::Color BOX_LINE(45, 55, 70);
    const sf::Color GIVEN_COLOR(28, 75, 150);      // Original puzzle clues
    const sf::Color PLAYER_COLOR(35, 42, 52);      // Player-entered numbers
    const sf::Color SELECTED_COLOR(255, 229, 150);
    const sf::Color FIXED_CELL_COLOR(231, 238, 249);
    const sf::Color BUTTON_COLOR(225, 231, 239);
    const sf::Color BUTTON_SELECTED(190, 211, 242);
    const sf::Color TEXT_COLOR(35, 42, 52);
    const sf::Color STATUS_COLOR(65, 75, 90);
    const sf::Color ERROR_COLOR(170, 45, 45);
    const sf::Color SUCCESS_COLOR(25, 120, 70);

    struct Button
    {
        sf::FloatRect bounds;
        std::string label;
        bool selected = false;
    };

    bool contains(const sf::FloatRect& bounds, sf::Vector2f point)
    {
        return bounds.contains(point);
    }

    void drawText(sf::RenderWindow& window, const sf::Font& font,
                  const std::string& value, unsigned int size,
                  sf::Vector2f position, sf::Color color = TEXT_COLOR)
    {
        sf::Text text(font, value, size);
        text.setFillColor(color);
        text.setPosition(position);
        window.draw(text);
    }

    void drawButton(sf::RenderWindow& window, const sf::Font& font,
                    const Button& button)
    {
        sf::RectangleShape shape({
            button.bounds.size.x,
            button.bounds.size.y
        });
        shape.setPosition(button.bounds.position);
        shape.setFillColor(button.selected ? BUTTON_SELECTED : BUTTON_COLOR);
        shape.setOutlineColor(sf::Color(175, 184, 196));
        shape.setOutlineThickness(1.f);
        window.draw(shape);

        sf::Text label(font, button.label, 17);
        label.setFillColor(TEXT_COLOR);
        const sf::FloatRect textBounds = label.getLocalBounds();
        label.setPosition({
            button.bounds.position.x +
                (button.bounds.size.x - textBounds.size.x) / 2.f -
                textBounds.position.x,
            button.bounds.position.y +
                (button.bounds.size.y - textBounds.size.y) / 2.f -
                textBounds.position.y
        });
        window.draw(label);
    }

    bool loadFont(sf::Font& font)
    {
        // Try common Windows and Linux font locations.
        const std::vector<std::string> paths = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/calibri.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"
        };

        for (const auto& path : paths)
        {
            if (font.openFromFile(path))
                return true;
        }
        return false;
    }

    std::string formatTime(long long seconds)
    {
        const long long minutes = seconds / 60;
        const long long remainingSeconds = seconds % 60;
        std::ostringstream out;
        out << minutes << ':';
        if (remainingSeconds < 10)
            out << '0';
        out << remainingSeconds;
        return out.str();
    }

    int keyToDigit(sf::Keyboard::Key key)
    {
        switch (key)
        {
            case sf::Keyboard::Key::Num0:
            case sf::Keyboard::Key::Numpad0: return 0;
            case sf::Keyboard::Key::Num1:
            case sf::Keyboard::Key::Numpad1: return 1;
            case sf::Keyboard::Key::Num2:
            case sf::Keyboard::Key::Numpad2: return 2;
            case sf::Keyboard::Key::Num3:
            case sf::Keyboard::Key::Numpad3: return 3;
            case sf::Keyboard::Key::Num4:
            case sf::Keyboard::Key::Numpad4: return 4;
            case sf::Keyboard::Key::Num5:
            case sf::Keyboard::Key::Numpad5: return 5;
            case sf::Keyboard::Key::Num6:
            case sf::Keyboard::Key::Numpad6: return 6;
            case sf::Keyboard::Key::Num7:
            case sf::Keyboard::Key::Numpad7: return 7;
            case sf::Keyboard::Key::Num8:
            case sf::Keyboard::Key::Numpad8: return 8;
            case sf::Keyboard::Key::Num9:
            case sf::Keyboard::Key::Numpad9: return 9;
            default: return -1;
        }
    }
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
        "Sudoku"
    );
    window.setFramerateLimit(60);

    sf::Font font;
    if (!loadFont(font))
    {
        std::cerr << "Could not load a font. Install a font or update loadFont() "
                     "with the path to a .ttf file.\n";
        return 1;
    }

    const std::vector<int> sizes = {4, 9, 16, 25};
    int chosenSize = 9;
    int chosenDifficulty = 1;

    std::unique_ptr<game> sudoku;
    int selectedRow = -1;
    int selectedCol = -1;
    std::string inputBuffer;
    std::string status = "Choose grid size and difficulty, then click New Game.";
    sf::Color statusColor = STATUS_COLOR;
    bool started = false;
    bool saved = false;
    bool guiSolved = false;
    std::vector<std::vector<bool>> fixedCells;
    auto guiStartTime = std::chrono::steady_clock::now();

    std::vector<Button> sizeButtons;
    for (std::size_t i = 0; i < sizes.size(); ++i)
    {
        sizeButtons.push_back({
            {{220.f + static_cast<float>(i) * 82.f, 48.f}, {70.f, 34.f}},
            std::to_string(sizes[i]) + "x" + std::to_string(sizes[i]),
            sizes[i] == chosenSize
        });
    }

    std::vector<Button> difficultyButtons;
    const std::vector<std::string> difficultyNames = {"Easy", "Medium", "Hard"};
    for (int i = 0; i < 3; ++i)
    {
        difficultyButtons.push_back({
            {{650.f + static_cast<float>(i) * 90.f, 48.f}, {78.f, 34.f}},
            difficultyNames[static_cast<std::size_t>(i)],
            i + 1 == chosenDifficulty
        });
    }

    Button newGameButton{{{70.f, 100.f}, {125.f, 38.f}}, "New Game"};
    Button hintButton{{{207.f, 100.f}, {90.f, 38.f}}, "Hint"};
    Button checkButton{{{309.f, 100.f}, {90.f, 38.f}}, "Check"};
    Button saveButton{{{411.f, 100.f}, {115.f, 38.f}}, "Save Score"};
    Button clearButton{{{538.f, 100.f}, {90.f, 38.f}}, "Clear Cell"};


    return 0;
}
