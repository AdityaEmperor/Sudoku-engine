
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
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Handle mouse clicks.
            if (const auto* mouse =
                    event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    const float mx = static_cast<float>(mouse->position.x);
                    const float my = static_cast<float>(mouse->position.y);

                    int col = static_cast<int>((mx - BOARD_X) / CELL_SIZE);
                    int row = static_cast<int>((my - BOARD_Y) / CELL_SIZE);

                    if (mx >= BOARD_X &&
                        mx < BOARD_X + GRID_SIZE * CELL_SIZE &&
                        my >= BOARD_Y &&
                        my < BOARD_Y + GRID_SIZE * CELL_SIZE)
                    {
                        selectedRow = row;
                        selectedCol = col;
                    }
                    else
                    {
                        selectedRow = -1;
                        selectedCol = -1;
                    }
                }
            }

            // Handle keyboard input.
            if (const auto* key =
                    event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }

                if (key->code == sf::Keyboard::Key::R)
                {
                    board = {};
                }

                if (selectedRow != -1 && selectedCol != -1)
                {
                    // Enter numbers from the top row or numpad.
                    int number = 0;

                    switch (key->code)
                    {
                        case sf::Keyboard::Key::Num1:
                        case sf::Keyboard::Key::Numpad1: number = 1; break;

                        case sf::Keyboard::Key::Num2:
                        case sf::Keyboard::Key::Numpad2: number = 2; break;

                        case sf::Keyboard::Key::Num3:
                        case sf::Keyboard::Key::Numpad3: number = 3; break;

                        case sf::Keyboard::Key::Num4:
                        case sf::Keyboard::Key::Numpad4: number = 4; break;

                        case sf::Keyboard::Key::Num5:
                        case sf::Keyboard::Key::Numpad5: number = 5; break;

                        case sf::Keyboard::Key::Num6:
                        case sf::Keyboard::Key::Numpad6: number = 6; break;

                        case sf::Keyboard::Key::Num7:
                        case sf::Keyboard::Key::Numpad7: number = 7; break;

                        case sf::Keyboard::Key::Num8:
                        case sf::Keyboard::Key::Numpad8: number = 8; break;

                        case sf::Keyboard::Key::Num9:
                        case sf::Keyboard::Key::Numpad9: number = 9; break;

                        default: break;
                    }

                    if (number != 0)
                    {
                        board[selectedRow][selectedCol] = number;
                    }

                    if (key->code == sf::Keyboard::Key::Backspace ||
                        key->code == sf::Keyboard::Key::Delete)
                    {
                        board[selectedRow][selectedCol] = 0;
                    }

                    // Navigate between cells.
                    if (key->code == sf::Keyboard::Key::Left)
                        selectedCol = (selectedCol + 8) % 9;

                    if (key->code == sf::Keyboard::Key::Right)
                        selectedCol = (selectedCol + 1) % 9;

                    if (key->code == sf::Keyboard::Key::Up)
                        selectedRow = (selectedRow + 8) % 9;

                    if (key->code == sf::Keyboard::Key::Down)
                        selectedRow = (selectedRow + 1) % 9;
                }
            }
        }

        //window.clear(sf::Color(245, 247, 250));

        //window.draw(instructions);
        //window.display();
    }

    return 0;
}
