
#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include <optional>

constexpr int GRID_SIZE = 9;
constexpr float CELL_SIZE = 70.f;
constexpr float BOARD_X = 45.f;
constexpr float BOARD_Y = 55.f;
constexpr unsigned int WINDOW_WIDTH = 720;
constexpr unsigned int WINDOW_HEIGHT = 760;

using Board = std::array<std::array<int, GRID_SIZE>, GRID_SIZE>;

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
        "Sudoku - 9x9"
    );

    window.setFramerateLimit(60);

    // Load a Windows system font.
    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        return 1;
    }

    Board board{};
    int selectedRow = -1;
    int selectedCol = -1;

    sf::RectangleShape cell({CELL_SIZE, CELL_SIZE});

    sf::RectangleShape gridLine;
    sf::RectangleShape selection({CELL_SIZE, CELL_SIZE});
    selection.setFillColor(sf::Color(190, 220, 255));

    sf::Text title(font, "SUDOKU", 30);
    title.setFillColor(sf::Color(35, 45, 65));
    title.setPosition({BOARD_X, 10.f});

    sf::Text instructions(
        font,
        "Click a cell and type 1-9  |  Backspace: erase\n"
        "Arrow keys: move  |  R: reset",
        17
    );
    instructions.setFillColor(sf::Color(65, 65, 65));
    instructions.setPosition({BOARD_X, BOARD_Y + GRID_SIZE * CELL_SIZE + 15.f});

    while (window.isOpen())
    {
       
        window.draw(instructions);
        window.display();
    }

    return 0;
}
