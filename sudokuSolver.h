#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include "sudokuBoard.h"

class solver : public board {
    public:
    bool isValid(const board &problem);
    bool solve(board &solution);
    public:
    vector<vector<int>> solveGrid(const board &problem);
    vector<vector<vector<int>>> allSolutions(const board &problem);
};

#endif