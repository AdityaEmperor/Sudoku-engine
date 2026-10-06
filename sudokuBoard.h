using namespace std;
#ifndef SUDOKU_BOARD_H
#define SUDOKU_BOARD_H
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

class board {
    public: 
    std::vector<std::vector<int>> grid;
    int size;
    int subGridSize;

    board();
    board(int s);
    board(std::vector<std::vector<int>> g);
    int getCell(int row , int col);
    void setCell(int row , int col , int val);
    void printBoard();
};

#endif