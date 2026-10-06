#include "sudokuBoard.h"

board::board() {
    board::size = 9;
    grid = std::vector<std::vector<char>>(board::size, std::vector<char>(size, '.'));
}

board::board(int s) {
    board::size = s;
    grid = std::vector<std::vector<char>>(board::size, std::vector<char>(size, '.'));
}

char board::getCell(int row , int col) {
    std::cout << " board [" << row << "]["<< col << "] = " << board::grid[row][col] << std::endl; 
    return board::grid[row][col];
}

void board::setCell(int row , int col , int val) {
    board::grid[row][col] = val;
}

void board::printBoard() {
    for(int i=0;i<size;i++) {
        for(int j=0;j<size;j++) {
            std::cout << " " << board::grid[i][j] << " , " ;
        }
        std::cout << "\n";
    }
}