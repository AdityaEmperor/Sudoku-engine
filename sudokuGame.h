#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include "sudokuBoard.h"
#include "sudokuSolver.h"

class game {
public:
    std::mt19937 rng;
    int size;
    board problem;
    board solution;
    int difficulty;
    int time;
    int hints;

    game(int s = 9, int d = 1);

    static void generateOrder(std::vector<int>& order, int boxSize,
                              std::mt19937& rng);
    void puzzelGenerator();

    // Starts a new game and snapshots the original clues.
    void startGame();

    // Player interaction. Rows and columns are zero-indexed.
    // Use value == 0 to clear a non-fixed cell.
    bool enterValue(int row, int col, int value);

    // Reveals one cell's correct value.
    bool hint();

    // Returns true only when the entire board matches the solution.
    bool check();

    // Appends completed-game statistics to sudoku_scores.csv.
    void log();

    // Returns the current score.
    int score() const;

private:
    std::vector<std::vector<int>> originalPuzzle;
    std::vector<std::vector<bool>> fixedCells;

    std::chrono::steady_clock::time_point startTime;
    bool timerStarted = false;
    bool gameFinished = false;

    int elapsedSeconds() const;
};

#endif
