#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "sudokuBoard.h"
#include "sudokuSolver.h"

namespace {
constexpr unsigned WINDOW_WIDTH = 1400;
constexpr unsigned WINDOW_HEIGHT = 850;

const sf::Color BACKGROUND(22, 25, 34);
const sf::Color PANEL(31, 36, 48, 205); // Slight transparency lets the painting show through
const sf::Color GRID_LINE(100, 111, 130);
const sf::Color THICK_LINE(220, 225, 235);
const sf::Color TEXT_COLOR(235, 239, 246);
const sf::Color MUTED(160, 170, 188);
const sf::Color ACCENT(78, 135, 245);
const sf::Color SELECTED(55, 76, 112);
const sf::Color ERROR(245, 105, 105);
const sf::Color SUCCESS(104, 215, 151);

struct Button {
    sf::FloatRect bounds;
    std::string label;
    bool active = false;
};

bool contains(const sf::FloatRect& r, sf::Vector2f p) {
    return r.contains(p);
}

void drawText(sf::RenderWindow& window, const sf::Font& font,
              const std::string& value, unsigned size, sf::Vector2f position,
              sf::Color color = TEXT_COLOR) {
    sf::Text text(font, value, size);
    text.setFillColor(color);
    text.setPosition(position);
    window.draw(text);
}

void drawButton(sf::RenderWindow& window, const sf::Font& font,
                const Button& button) {
    sf::RectangleShape shape({button.bounds.size.x, button.bounds.size.y});
    shape.setPosition(button.bounds.position);
    shape.setFillColor(button.active ? ACCENT : sf::Color(48, 56, 73));
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(button.active ? sf::Color(135, 174, 255)
                                        : sf::Color(75, 85, 105));
    window.draw(shape);

    sf::Text label(font, button.label, 18);
    label.setFillColor(TEXT_COLOR);
    const auto textBounds = label.getLocalBounds();
    label.setPosition({
        button.bounds.position.x + (button.bounds.size.x - textBounds.size.x) / 2.f
            - textBounds.position.x,
        button.bounds.position.y + (button.bounds.size.y - textBounds.size.y) / 2.f
            - textBounds.position.y - 1.f
    });
    window.draw(label);
}

int subgridSizeFor(int n) {
    switch (n) {
        case 4: return 2;
        case 9: return 3;
        case 16: return 4;
        case 25: return 5;
        default: return 0;
    }
}

std::string valueLabel(int value) {
    return value == -1 ? "." : std::to_string(value);
}

struct SudokuPanel {
    sf::Vector2f origin;
    float gridSize = 590.f;
    float cellSize = 590.f / 9.f;
};

} // namespace

int main() {
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                            "Sudoku Solver", sf::Style::Titlebar |
                            sf::Style::Close | sf::Style::Resize);
    window.setFramerateLimit(60);

    // Load the background painting from the executable's working directory.
    sf::Texture backgroundTexture;
    const bool backgroundLoaded =
        backgroundTexture.loadFromFile("Starry_Night_Over_the_Rhone.jpg");

    if (!backgroundLoaded) {
        std::cerr << "Could not load Starry_Night_Over_the_Rhone.jpg. "
                     "Place it beside the executable.\n";
    } else {
        backgroundTexture.setSmooth(true);
    }

    // SFML 3 Sprite keeps a reference to the texture, so the texture must
    // remain alive for the entire application.
    std::optional<sf::Sprite> backgroundSprite;
    if (backgroundLoaded) {
        backgroundSprite.emplace(backgroundTexture);
        backgroundSprite->setColor(sf::Color(255, 255, 255, 125)); // Painting opacity
    }

    sf::Font font;
    bool fontLoaded = font.openFromFile("arial.ttf");
    if (!fontLoaded) fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) {
        std::cerr << "Could not load a font. Put arial.ttf beside the executable, "
                     "or update the font path in main.cpp.\n";
        return 1;
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////
    const std::vector<int> supportedSizes{4, 9, 16};
    std::vector<Button> sizeButtons;
    for (std::size_t i = 0; i < supportedSizes.size(); ++i) {
        sizeButtons.push_back({
            {{30.f + static_cast<float>(i) * 125.f, 65.f}, {108.f, 42.f}},
            std::to_string(supportedSizes[i]) + " x " +
                std::to_string(supportedSizes[i]),
            supportedSizes[i] == 9
        });
    }

    Button solveButton{{{1130.f, 65.f}, {220.f, 44.f}}, "SOLVE", false};
    Button clearButton{{{1130.f, 120.f}, {220.f, 38.f}}, "CLEAR BOARDS", false};

    int boardSize = 9;
    int selectedRow = 0;
    int selectedCol = 0;
    bool hasSelection = true;
    std::vector<std::vector<int>> input(
        boardSize, std::vector<int>(boardSize, -1));
    std::vector<std::vector<int>> output;
    std::string pendingNumber;
    std::string status = "Select a cell, then type a value. Empty cells are shown as dots.";
    sf::Color statusColor = MUTED;

    // The two panels occupy the lower part of the window.
    SudokuPanel leftPanel{{30.f, 195.f}, 590.f, 590.f / boardSize};
    SudokuPanel rightPanel{{720.f, 195.f}, 590.f, 590.f / boardSize};

    auto resetBoard = [&](int newSize) {
        boardSize = newSize;
        input.assign(boardSize, std::vector<int>(boardSize, -1));
        output.clear();
        selectedRow = selectedCol = 0;
        hasSelection = true;
        pendingNumber.clear();
        status = "Enter values from 1 to " + std::to_string(boardSize) + ".";
        statusColor = MUTED;
        for (auto& button : sizeButtons) {
            button.active = button.label ==
                (std::to_string(boardSize) + " x " + std::to_string(boardSize));
        }
    };

    auto resizePanels = [&]() {
        // Keep both grids legible and on-screen for 25x25 boards.
        const float available = std::min(590.f, static_cast<float>(window.getSize().y) - 245.f);
        const float grid = std::max(200.f, available);
        leftPanel.gridSize = grid;
        rightPanel.gridSize = grid;
        leftPanel.cellSize = grid / boardSize;
        rightPanel.cellSize = grid / boardSize;
        const float y = 195.f;
        leftPanel.origin = {30.f, y};
        rightPanel.origin = {720.f, y};
    };

    auto commitPending = [&]() {
        if (pendingNumber.empty()) return;
        int value = 0;
        try {
            value = std::stoi(pendingNumber);
        } catch (...) {
            pendingNumber.clear();
            return;
        }
        if (value >= 1 && value <= boardSize) {
            input[selectedRow][selectedCol] = value;
            output.clear();
            status = "Entry updated. Press SOLVE when ready.";
            statusColor = MUTED;
        } else {
            status = "Invalid value: enter a number from 1 to " +
                     std::to_string(boardSize) + ".";
            statusColor = ERROR;
        }
        pendingNumber.clear();
    };

    auto solvePuzzle = [&]() {
        commitPending();

        // Build a board using the constructor supplied by the user's project.
        board problem(input);
        solver sudokuSolver;

        if (!sudokuSolver.isValid(problem)) {
            output.clear();
            status = "INVALID BOARD: duplicate value in a row, column, or subgrid.";
            statusColor = ERROR;
            return;
        }

        status = "Solving...";
        statusColor = MUTED;

        // solveGrid returns an empty vector for invalid or unsolvable puzzles.
        // isValid above separates duplicate-entry errors from no-solution cases.
        const auto solved = sudokuSolver.solveGrid(problem);
        if (solved.empty()) {
            output.clear();
            status = "UNSOLVABLE: no solution exists for these entries.";
            statusColor = ERROR;
            return;
        }

        output = solved;
        status = "SOLVED: a valid solution is displayed on the right.";
        statusColor = SUCCESS;
    };

    while (window.isOpen()) {
        resizePanels();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                window.setView(sf::View(sf::FloatRect(
                    {0.f, 0.f},
                    {static_cast<float>(resized->size.x),
                     static_cast<float>(resized->size.y)})));
            } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button != sf::Mouse::Button::Left) continue;
                const sf::Vector2f point = window.mapPixelToCoords(mouse->position);

                bool handled = false;
                for (std::size_t i = 0; i < sizeButtons.size(); ++i) {
                    if (contains(sizeButtons[i].bounds, point)) {
                        resetBoard(supportedSizes[i]);
                        handled = true;
                        break;
                    }
                }
                if (handled) continue;

                if (contains(solveButton.bounds, point)) {
                    solvePuzzle();
                    continue;
                }
                if (contains(clearButton.bounds, point)) {
                    resetBoard(boardSize);
                    status = "Board cleared. Enter a new puzzle.";
                    statusColor = MUTED;
                    continue;
                }

                const float x = point.x - leftPanel.origin.x;
                const float y = point.y - leftPanel.origin.y;
                if (x >= 0.f && y >= 0.f &&
                    x < leftPanel.gridSize && y < leftPanel.gridSize) {
                    selectedCol = std::clamp(
                        static_cast<int>(x / leftPanel.cellSize), 0, boardSize - 1);
                    selectedRow = std::clamp(
                        static_cast<int>(y / leftPanel.cellSize), 0, boardSize - 1);
                    hasSelection = true;
                    pendingNumber.clear();
                }
            } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    pendingNumber.clear();
                } else if (key->code == sf::Keyboard::Key::Enter) {
                    commitPending();
                } else if (key->code == sf::Keyboard::Key::Backspace) {
                    if (!pendingNumber.empty()) {
                        pendingNumber.pop_back();
                    } else if (hasSelection) {
                        input[selectedRow][selectedCol] = -1;
                        output.clear();
                        status = "Cell cleared.";
                        statusColor = MUTED;
                    }
                } else if (key->code == sf::Keyboard::Key::Delete) {
                    if (hasSelection) {
                        input[selectedRow][selectedCol] = -1;
                        pendingNumber.clear();
                        output.clear();
                    }
                } else if (key->code == sf::Keyboard::Key::Tab) {
                    if (hasSelection) {
                        selectedCol++;
                        if (selectedCol >= boardSize) {
                            selectedCol = 0;
                            selectedRow = (selectedRow + 1) % boardSize;
                        }
                        pendingNumber.clear();
                    }
                } else if (key->code == sf::Keyboard::Key::S &&
                           (key->control || key->system)) {
                    solvePuzzle();
                }
            } else if (const auto* typed = event->getIf<sf::Event::TextEntered>()) {
                if (!hasSelection) continue;
                const char32_t unicode = typed->unicode;
                if (unicode >= U'0' && unicode <= U'9') {
                    const char digit = static_cast<char>(unicode);
                    if (boardSize <= 9) {
                        const int value = digit - '0';
                        if (value == 0) {
                            input[selectedRow][selectedCol] = -1;
                        } else if (value <= boardSize) {
                            input[selectedRow][selectedCol] = value;
                        } else {
                            status = "Value must be between 1 and " +
                                     std::to_string(boardSize) + ".";
                            statusColor = ERROR;
                        }
                        output.clear();
                        pendingNumber.clear();
                        status = "Entry updated. Press SOLVE when ready.";
                        statusColor = MUTED;
                    } else {
                        if (pendingNumber.size() < 2) pendingNumber.push_back(digit);
                        if (pendingNumber.size() == 2) {
                            // Two-digit values are committed automatically.
                            commitPending();
                        } else {
                            status = "Typing " + pendingNumber +
                                     " — press Enter for a one-digit value, "
                                     "or type a second digit (10–25).";
                            statusColor = MUTED;
                        }
                    }
                }
            }
        }

        window.clear(BACKGROUND);

        // Scale the image to cover the full window while preserving its aspect ratio.
        if (backgroundSprite) {
            const sf::Vector2u textureSize = backgroundTexture.getSize();
            const sf::Vector2u windowSize = window.getSize();
            if (textureSize.x > 0 && textureSize.y > 0) {
                const float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
                const float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
                const float scale = std::max(scaleX, scaleY);
                backgroundSprite->setScale({scale, scale});
                const sf::FloatRect bounds = backgroundSprite->getGlobalBounds();
                backgroundSprite->setPosition({
                    (static_cast<float>(windowSize.x) - bounds.size.x) / 2.f,
                    (static_cast<float>(windowSize.y) - bounds.size.y) / 2.f
                });
                window.draw(*backgroundSprite);
            }
        }

        drawText(window, font, "SUDOKU SOLVER", 28, {30.f, 18.f});
        drawText(window, font, "Choose board size", 15, {30.f, 43.f}, MUTED);
        for (const auto& button : sizeButtons) drawButton(window, font, button);
        drawButton(window, font, solveButton);
        drawButton(window, font, clearButton);

        drawText(window, font, "INPUT PUZZLE", 20, {30.f, 155.f});
        drawText(window, font, "SOLUTION", 20, {720.f, 155.f});
        drawText(window, font, "Click a cell to edit it", 13, {190.f, 160.f}, MUTED);
        drawText(window, font, "Read-only result", 13, {830.f, 160.f}, MUTED);

        auto drawGrid = [&](const SudokuPanel& panel,
                            const std::vector<std::vector<int>>& values,
                            bool editable) {
            const float cell = panel.cellSize;
            sf::RectangleShape background({panel.gridSize, panel.gridSize});
            background.setPosition(panel.origin);
            background.setFillColor(PANEL);
            window.draw(background);

            if (editable && hasSelection) {
                sf::RectangleShape selected({cell, cell});
                selected.setPosition({
                    panel.origin.x + selectedCol * cell,
                    panel.origin.y + selectedRow * cell
                });
                selected.setFillColor(SELECTED);
                window.draw(selected);
            }

            for (int r = 0; r < boardSize; ++r) {
                for (int c = 0; c < boardSize; ++c) {
                    const int value = values[r][c];
                    if (value == -1) continue;
                    const unsigned characterSize =
                        static_cast<unsigned>(std::clamp(cell * 0.48f, 10.f, 25.f));
                    sf::Text number(font, std::to_string(value), characterSize);
                    number.setFillColor(editable ? TEXT_COLOR : SUCCESS);
                    const auto bounds = number.getLocalBounds();
                    number.setPosition({
                        panel.origin.x + c * cell + (cell - bounds.size.x) / 2.f
                            - bounds.position.x,
                        panel.origin.y + r * cell + (cell - bounds.size.y) / 2.f
                            - bounds.position.y - 1.f
                    });
                    window.draw(number);
                }
            }

            for (int i = 0; i <= boardSize; ++i) {
                const bool thick = (i % subgridSizeFor(boardSize) == 0);
                sf::RectangleShape vertical({thick ? 2.f : 1.f, panel.gridSize});
                vertical.setPosition({panel.origin.x + i * cell - (thick ? 1.f : 0.f),
                                      panel.origin.y});
                vertical.setFillColor(thick ? THICK_LINE : GRID_LINE);
                window.draw(vertical);

                sf::RectangleShape horizontal({panel.gridSize, thick ? 2.f : 1.f});
                horizontal.setPosition({panel.origin.x,
                                        panel.origin.y + i * cell - (thick ? 1.f : 0.f)});
                horizontal.setFillColor(thick ? THICK_LINE : GRID_LINE);
                window.draw(horizontal);
            }

            if (editable && hasSelection && !pendingNumber.empty()) {
                drawText(window, font, "Pending: " + pendingNumber, 12,
                         {panel.origin.x, panel.origin.y + panel.gridSize + 5.f}, ACCENT);
            }
        };

        drawGrid(leftPanel, input, true);
        if (!output.empty()) {
            drawGrid(rightPanel, output, false);
        } else {
            sf::RectangleShape empty({rightPanel.gridSize, rightPanel.gridSize});
            empty.setPosition(rightPanel.origin);
            empty.setFillColor(PANEL);
            empty.setOutlineThickness(1.f);
            empty.setOutlineColor(GRID_LINE);
            window.draw(empty);
            drawText(window, font, "Your solved board", 21,
                     {rightPanel.origin.x + 20.f, rightPanel.origin.y + 25.f}, MUTED);
            drawText(window, font, "will appear here.", 17,
                     {rightPanel.origin.x + 20.f, rightPanel.origin.y + 58.f}, MUTED);
        }

        // Footer/status line stays below the grids.
        const float footerY = std::min(
            static_cast<float>(window.getSize().y) - 45.f,
            leftPanel.origin.y + leftPanel.gridSize + 32.f);
        drawText(window, font, status, 16, {30.f, footerY}, statusColor);
        drawText(window, font,
                 boardSize <= 9
                    ? "Keys: 1-9 enter | 0/Backspace clear | Tab next cell"
                    : "For 16x16 / 25x25: type a value; Enter commits one digit; "
                      "two-digit values commit automatically. Backspace clears.",
                 12, {30.f, static_cast<float>(window.getSize().y) - 20.f}, MUTED);

        window.display();
    }

    return 0;
}
