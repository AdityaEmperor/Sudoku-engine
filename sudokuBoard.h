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
    int size;

    board();
    char getCell(int row , int col);
    void setCell(int row , int col , int val);
    void printBoard();
};

#endif