#include "sudokuGame.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;

game::game(int s, int d)
    : rng(random_device{}()),
      size(s),
      problem(s),
      solution(s),
      difficulty(d),
      time(0),
      hints(0)
{
}

void game::generateOrder(vector<int>& order, int boxSize, mt19937& rng)
{
    int n = boxSize * boxSize;

    vector<int> groups(boxSize);
    for (int i = 0; i < boxSize; i++)
        groups[i] = i;

    shuffle(groups.begin(), groups.end(), rng);

    order.clear();
    order.reserve(n);

    for (int group : groups)
    {
        vector<int> positions(boxSize);
        for (int i = 0; i < boxSize; i++)
            positions[i] = i;

        shuffle(positions.begin(), positions.end(), rng);

        for (int pos : positions)
            order.push_back(group * boxSize + pos);
    }
}

void game::puzzelGenerator()
{
    // Existing generation approach retained: randomized row/column orders,
    // symbol permutation, then removal of cells by difficulty.
    if ((size != 4 && size != 9 && size != 16 && size != 25) ||
        difficulty < 1 || difficulty > 3)
    {
        cerr << "Invalid Sudoku size or difficulty.\n";
        return;
    }

    int n = size;
    int boxSize = static_cast<int>(sqrt(n));

    solution.size = n;
    solution.subGridSize = boxSize;
    solution.grid.assign(n, vector<int>(n, -1));

    problem.size = n;
    problem.subGridSize = boxSize;
    problem.grid.assign(n, vector<int>(n, -1));

    vector<int> rows, cols, symbols;

    generateOrder(rows, boxSize, rng);
    generateOrder(cols, boxSize, rng);

    symbols.resize(n);
    for (int i = 0; i < n; i++)
        symbols[i] = i + 1;

    shuffle(symbols.begin(), symbols.end(), rng);

    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n; c++)
        {
            int baseRow = rows[r];
            int baseCol = cols[c];

            int value = (baseRow * boxSize + baseRow / boxSize + baseCol) % n;
            solution.grid[r][c] = symbols[value];
        }
    }

    problem.grid = solution.grid;

    int emptyPercent;
    if (difficulty == 1)
        emptyPercent = 30;
    else if (difficulty == 2)
        emptyPercent = 45;
    else
        emptyPercent = 60;

    int totalCells = n * n;
    int emptyCount = totalCells * emptyPercent / 100;

    vector<int> positions(totalCells);
    for (int i = 0; i < totalCells; i++)
        positions[i] = i;

    shuffle(positions.begin(), positions.end(), rng);

    for (int i = 0; i < emptyCount; i++)
    {
        int r = positions[i] / n;
        int c = positions[i] % n;
        problem.grid[r][c] = -1;
    }

    problem.printBoard();
    solution.printBoard();
}

int game::elapsedSeconds() const
{
    if (!timerStarted || gameFinished)
        return time;

    return static_cast<int>(
        chrono::duration_cast<chrono::seconds>(
            chrono::steady_clock::now() - startTime
        ).count()
    );
}

void game::startGame()
{
    if ((size != 4 && size != 9 && size != 16 && size != 25) ||
        difficulty < 1 || difficulty > 3)
    {
        cerr << "Invalid Sudoku size or difficulty.\n";
        return;
    }

    puzzelGenerator();

    // Do not start if generation did not produce a correctly sized grid.
    if (static_cast<int>(problem.grid.size()) != size ||
        static_cast<int>(solution.grid.size()) != size)
    {
        cerr << "Puzzle generation failed.\n";
        return;
    }

    originalPuzzle = problem.grid;
    fixedCells.assign(size, vector<bool>(size, false));

    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            fixedCells[r][c] = (originalPuzzle[r][c] != -1);

    time = 0;
    hints = 0;
    gameFinished = false;
    startTime = chrono::steady_clock::now();
    timerStarted = true;
}

bool game::enterValue(int row, int col, int value)
{

}

bool game::hint()
{

}

bool game::check()
{

}

int game::score() const
{
    int seconds = elapsedSeconds();
    double baseScore = static_cast<double>(size) * size * difficulty * 10.0;
    double timeFactor = 1.0 + seconds / 300.0;

    int finalScore = static_cast<int>(baseScore / timeFactor - hints * 50.0);
    return max(0, finalScore);
}

void game::log()
{

}
