#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <random>
#include "sudokuBoard.h"
#include "sudokuSolver.h"

class game : public board{
    public:
    mt19937 rng;
    int difficulty;
    int time;

    game(int s = 9,int d = 1);
    void fillBoard();
    void removeCells();
    void puzzelGenerator();
    void startGame();
    void log();
};

#endif