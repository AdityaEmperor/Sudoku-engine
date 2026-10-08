#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include "sudokuBoard.h"
#include "sudokuSolver.h"

class game : public solver{
    public :
    int difficulty;
    int time;

    game(int s = 9,int d = 1);
    void puzzelGenerator();
    void startGame();
    void log();
};

#endif