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

    auto beginNewGame = [&]()
    {
        sudoku = std::make_unique<game>(chosenSize, chosenDifficulty);
        sudoku->startGame();

        selectedRow = -1;
        selectedCol = -1;
        inputBuffer.clear();
        started = true;
        saved = false;
        guiSolved = false;
        fixedCells.assign(sudoku->size, std::vector<bool>(sudoku->size, false));
        for (int r = 0; r < sudoku->size; ++r)
            for (int c = 0; c < sudoku->size; ++c)
                fixedCells[r][c] = (sudoku->problem.grid[r][c] != -1);
        guiStartTime = std::chrono::steady_clock::now();
        status = "Game started. Click an editable cell, type a value, then press Enter.";
        statusColor = STATUS_COLOR;
    };

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* mouse =
                         event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button != sf::Mouse::Button::Left)
                    continue;

                const sf::Vector2f point(
                    static_cast<float>(mouse->position.x),
                    static_cast<float>(mouse->position.y)
                );

                bool clickedControl = false;

                for (std::size_t i = 0; i < sizeButtons.size(); ++i)
                {
                    if (contains(sizeButtons[i].bounds, point))
                    {
                        chosenSize = sizes[i];
                        for (std::size_t j = 0; j < sizeButtons.size(); ++j)
                            sizeButtons[j].selected = (j == i);

                        status = "Selected " + std::to_string(chosenSize) +
                                 "x" + std::to_string(chosenSize) +
                                 ". Click New Game to apply.";
                        statusColor = STATUS_COLOR;
                        clickedControl = true;
                        break;
                    }
                }

                if (clickedControl)
                    continue;

                for (std::size_t i = 0; i < difficultyButtons.size(); ++i)
                {
                    if (contains(difficultyButtons[i].bounds, point))
                    {
                        chosenDifficulty = static_cast<int>(i) + 1;
                        for (std::size_t j = 0; j < difficultyButtons.size(); ++j)
                            difficultyButtons[j].selected = (j == i);

                        status = "Selected " + difficultyNames[i] +
                                 ". Click New Game to apply.";
                        statusColor = STATUS_COLOR;
                        clickedControl = true;
                        break;
                    }
                }

                if (clickedControl)
                    continue;

                if (contains(newGameButton.bounds, point))
                {
                    beginNewGame();
                    continue;
                }

                if (contains(hintButton.bounds, point))
                {
                    if (!started || !sudoku)
                    {
                        status = "Start a game first.";
                        statusColor = ERROR_COLOR;
                    }
                    else if (sudoku->hint())
                    {
                        status = "Hint used. One cell was filled correctly.";
                        statusColor = STATUS_COLOR;
                        if (sudoku->check())
                        {
                            guiSolved = true;
                            status = "Puzzle solved! Score: " +
                                     std::to_string(sudoku->score());
                            statusColor = SUCCESS_COLOR;
                        }
                    }
                    else
                    {
                        status = "No hint available; the board may already be solved.";
                        statusColor = STATUS_COLOR;
                    }
                    continue;
                }

                if (contains(checkButton.bounds, point))
                {
                    if (started && sudoku && sudoku->check())
                    {
                        guiSolved = true;
                        status = "Puzzle solved! Score: " +
                                 std::to_string(sudoku->score());
                        statusColor = SUCCESS_COLOR;
                    }
                    else
                    {
                        status = "Not solved yet. Keep going.";
                        statusColor = STATUS_COLOR;
                    }
                    continue;
                }

                if (contains(saveButton.bounds, point))
                {
                    if (started && sudoku && sudoku->check())
                    {
                        guiSolved = true;
                        if (!saved)
                        {
                            sudoku->log();
                            saved = true;
                            status = "Score saved to sudoku_scores.csv.";
                        }
                        else
                        {
                            status = "This game's score has already been saved.";
                        }
                        statusColor = SUCCESS_COLOR;
                    }
                    else
                    {
                        status = "Solve the puzzle before saving a score.";
                        statusColor = ERROR_COLOR;
                    }
                    continue;
                }

                if (contains(clearButton.bounds, point))
                {
                    if (started && sudoku && selectedRow >= 0)
                    {
                        if (sudoku->enterValue(selectedRow, selectedCol, 0))
                        {
                            inputBuffer.clear();
                            status = "Cell cleared.";
                            statusColor = STATUS_COLOR;
                        }
                        else
                        {
                            status = "That cell is a fixed clue or cannot be changed.";
                            statusColor = ERROR_COLOR;
                        }
                    }
                    continue;
                }

                if (!started || !sudoku)
                    continue;

                const float cellSize = GRID_SIZE / static_cast<float>(sudoku->size);
                const sf::FloatRect gridBounds(
                    {GRID_X, GRID_Y}, {GRID_SIZE, GRID_SIZE}
                );

                if (gridBounds.contains(point))
                {
                    selectedCol = static_cast<int>((point.x - GRID_X) / cellSize);
                    selectedRow = static_cast<int>((point.y - GRID_Y) / cellSize);
                    inputBuffer.clear();

                    if (selectedRow >= 0 && selectedRow < sudoku->size &&
                        selectedCol >= 0 && selectedCol < sudoku->size)
                    {
                        status = "Selected row " + std::to_string(selectedRow + 1) +
                                 ", column " + std::to_string(selectedCol + 1) +
                                 ". Type a value and press Enter.";
                        statusColor = STATUS_COLOR;
                    }
                }
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                    continue;
                }

                if (!started || !sudoku || selectedRow < 0 || selectedCol < 0)
                    continue;

                if (key->code == sf::Keyboard::Key::Enter)
                {
                    if (inputBuffer.empty())
                        continue;

                    int value = -1;
                    try
                    {
                        std::size_t used = 0;
                        value = std::stoi(inputBuffer, &used);
                        if (used != inputBuffer.size())
                            value = -1;
                    }
                    catch (...)
                    {
                        value = -1;
                    }

                    if (value >= 0 &&
                        sudoku->enterValue(selectedRow, selectedCol, value))
                    {
                        status = (value == 0) ? "Cell cleared." : "Value entered.";
                        statusColor = STATUS_COLOR;
                        if (sudoku->check())
                        {
                            guiSolved = true;
                            status = "Puzzle solved! Score: " +
                                     std::to_string(sudoku->score());
                            statusColor = SUCCESS_COLOR;
                        }
                    }
                    else
                    {
                        status = "Move rejected. Check the value, Sudoku rules, and whether the cell is fixed.";
                        statusColor = ERROR_COLOR;
                    }
                    inputBuffer.clear();
                }
                else if (key->code == sf::Keyboard::Key::Backspace)
                {
                    if (!inputBuffer.empty())
                        inputBuffer.pop_back();
                }
                else if (key->code == sf::Keyboard::Key::Delete)
                {
                    if (sudoku->enterValue(selectedRow, selectedCol, 0))
                    {
                        inputBuffer.clear();
                        status = "Cell cleared.";
                        statusColor = STATUS_COLOR;
                    }
                    else
                    {
                        status = "That cell is a fixed clue or cannot be changed.";
                        statusColor = ERROR_COLOR;
                    }
                }
                else
                {
                    const int digit = keyToDigit(key->code);
                    if (digit >= 0 && inputBuffer.size() < 2)
                        inputBuffer += static_cast<char>('0' + digit);
                }
            }
        }

        window.clear(BACKGROUND);

        drawText(window, font, "SUDOKU", 29, {70.f, 8.f});
        drawText(window, font, "Grid:", 16, {165.f, 55.f});
        for (const auto& button : sizeButtons)
            drawButton(window, font, button);

        drawText(window, font, "Difficulty:", 16, {565.f, 55.f});
        for (const auto& button : difficultyButtons)
            drawButton(window, font, button);

        drawButton(window, font, newGameButton);
        drawButton(window, font, hintButton);
        drawButton(window, font, checkButton);
        drawButton(window, font, saveButton);
        drawButton(window, font, clearButton);

        if (started && sudoku)
        {
            const float cellSize = GRID_SIZE / static_cast<float>(sudoku->size);

            // Cell backgrounds: fixed clues have a distinct subtle tint.
            for (int r = 0; r < sudoku->size; ++r)
            {
                for (int c = 0; c < sudoku->size; ++c)
                {
                    sf::RectangleShape cell({cellSize, cellSize});
                    cell.setPosition({
                        GRID_X + c * cellSize,
                        GRID_Y + r * cellSize
                    });

                    if (r == selectedRow && c == selectedCol)
                        cell.setFillColor(SELECTED_COLOR);
                    else if (fixedCells[r][c])
                        cell.setFillColor(FIXED_CELL_COLOR);
                    else
                        cell.setFillColor(sf::Color::White);

                    window.draw(cell);

                    const int value = sudoku->problem.grid[r][c];
                    if (value != -1)
                    {
                        sf::Text number(font, std::to_string(value),
                                        sudoku->size >= 25 ? 14 :
                                        sudoku->size >= 16 ? 17 :
                                        sudoku->size >= 9 ? 23 : 30);

                        number.setFillColor(
                            fixedCells[r][c] ? GIVEN_COLOR : PLAYER_COLOR
                        );

                        const sf::FloatRect textBounds = number.getLocalBounds();
                        number.setPosition({
                            GRID_X + c * cellSize +
                                (cellSize - textBounds.size.x) / 2.f -
                                textBounds.position.x,
                            GRID_Y + r * cellSize +
                                (cellSize - textBounds.size.y) / 2.f -
                                textBounds.position.y - 1.f
                        });
                        window.draw(number);
                    }
                }
            }

            // Draw thin cell lines and heavier subgrid boundaries.
            for (int i = 0; i <= sudoku->size; ++i)
            {
                const bool thick = (i % sudoku->problem.subGridSize == 0);
                sf::RectangleShape vertical({
                    thick ? 2.5f : 1.f, GRID_SIZE
                });
                vertical.setPosition({
                    GRID_X + i * cellSize - (thick ? 1.25f : 0.5f),
                    GRID_Y
                });
                vertical.setFillColor(thick ? BOX_LINE : GRID_LINE);
                window.draw(vertical);

                sf::RectangleShape horizontal({
                    GRID_SIZE, thick ? 2.5f : 1.f
                });
                horizontal.setPosition({
                    GRID_X,
                    GRID_Y + i * cellSize - (thick ? 1.25f : 0.5f)
                });
                horizontal.setFillColor(thick ? BOX_LINE : GRID_LINE);
                window.draw(horizontal);
            }

            long long elapsed = sudoku->time;
            if (!guiSolved)
            {
                elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - guiStartTime
                ).count();
            }

            drawText(window, font,
                     "Time: " + formatTime(elapsed) +
                     "    Hints: " + std::to_string(sudoku->hints) +
                     "    Score: " + std::to_string(sudoku->score()),
                     18, {70.f, 920.f});
        }
        else
        {
            sf::RectangleShape placeholder({GRID_SIZE, GRID_SIZE});
            placeholder.setPosition({GRID_X, GRID_Y});
            placeholder.setFillColor(sf::Color::White);
            placeholder.setOutlineColor(GRID_LINE);
            placeholder.setOutlineThickness(1.f);
            window.draw(placeholder);

            drawText(window, font, "Select a grid size and difficulty,",
                     24, {GRID_X + 150.f, GRID_Y + 300.f});
            drawText(window, font, "then click New Game.",
                     24, {GRID_X + 230.f, GRID_Y + 340.f});
        }

        drawText(window, font, "Status: " + status, 15,
                 {70.f, 875.f}, statusColor);

        if (!inputBuffer.empty())
            drawText(window, font, "Input: " + inputBuffer + "  (Enter to apply)",
                     16, {650.f, 875.f}, TEXT_COLOR);

        drawText(window, font,
                 "Click an editable cell, type digits, press Enter. Delete clears; Esc exits.",
                 14, {70.f, 905.f}, STATUS_COLOR);

        window.display();
    }

    return 0;
}
