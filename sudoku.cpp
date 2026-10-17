
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
    return 0;
}
