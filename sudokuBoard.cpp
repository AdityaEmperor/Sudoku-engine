#include "sudokuBoard.h"
using namespace std;

board::board() {
    board::size = 9;
    grid = std::vector<std::vector<char>>(board::size, std::vector<char>(size, '.'));
}

board::board(int s) {
    board::size = s;
    grid = std::vector<std::vector<char>>(board::size, std::vector<char>(size, '.'));
}

board::board(vector<vector<char>> g) {
    if(g.size() != g[0].size())
        return;
    
    int size = g.size();
    if(size!=4 || size!=9 || size!=16 || size!=25)
    return;

    board::grid = g;
    switch(size) {
        case 4 :
            subGridSize = 2;
            break;
        case 9 :
            subGridSize = 3;
            break;
        case 16 :
            subGridSize = 4;
            break;
        case 25 :
            subGridSize = 5;
            break;
    }

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