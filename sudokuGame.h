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
    int size;
    board problem;
    board solution;
    int difficulty;
    int time;

    game(int s,int d);
    
    static void generateOrder(std::vector<int> &order,int boxSize, std::mt19937 &rng);
    void puzzelGenerator();
    // void startGame(); // generates puzzle
    // void log(); // user file update
};

#endif