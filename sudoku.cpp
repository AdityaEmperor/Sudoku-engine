#ifndef SUDOKU_BOARD_H
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

class board {
    public: 
    std::vector<std::vector<char>> grid;

    board();
    void getCell(int row , int col);
    void setCell(int row , int col , int val);
    void printBoart();
};


#endif