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

    }

    void drawButton(sf::RenderWindow& window, const sf::Font& font,
                    const Button& button)
    {
      
    }

    bool loadFont(sf::Font& font)
    {
    
    }

    std::string formatTime(long long seconds)
    {

    }

    int keyToDigit(sf::Keyboard::Key key)
    {
    
    }
}

int main()
{

}
