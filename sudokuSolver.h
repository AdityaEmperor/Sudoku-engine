#ifndef SUDOKU_SOLVER_H
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include "sudokuBoard.h"

class solver : public board {
    bool isValid(board problem);
    vector<vector<int>> solveGrid(board problem);
    vector<vector<vector<int>>> allSolutions(board problem);
};

#endif