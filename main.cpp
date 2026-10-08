#include <iostream>
#include <vector>
#include "sudokuBoard.h"
#include "sudokuSolver.h"
#include "sudokuGame.h"

using namespace std;

int main() {

    int choice;
    int size;

    cout << "==============================\n";
    cout << "       SUDOKU SOLVER\n";
    cout << "==============================\n\n";

    cout << "Choose Sudoku Grid Size:\n";
    cout << "1. 6x6\n";
    cout << "2. 9x9\n";
    cout << "3. 16x16\n";
    cout << "4. 25x25\n\n";

    cout << "Enter your choice: ";
    cin >> choice;

    switch(choice) {
        case 1:
            size = 6;
            break;

        case 2:
            size = 9;
            break;

        case 3:
            size = 16;
            break;

        case 4:
            size = 25;
            break;

        default:
            cout << "Invalid choice.\n";
            return 0;
    }

    if(size == 6) {
        cout << "\n6x6 Sudoku is not currently supported by the given";
        cout << "\nboard and solver classes.\n";
        cout << "The current solver supports square subgrids only.\n";
        cout << "A 6x6 Sudoku requires 2x3 subgrids.\n";
        cout << "\nPlease use 9x9, 16x16 or 25x25 for now.\n";
        return 0;
    }

    vector<vector<int>> grid(size, vector<int>(size));

    cout << "\nEnter the Sudoku grid.\n";
    cout << "Use -1 for empty cells.\n\n";

    for(int i = 0; i < size; i++) {
        cout << "Row " << i + 1 << ": ";

        for(int j = 0; j < size; j++) {
            cin >> grid[i][j];
        }
    }

    board problem(grid);

    solver sudokuSolver;

    cout << "\n==============================\n";
    cout << "       INPUT SUDOKU\n";
    cout << "==============================\n\n";

    problem.printBoard();

    if(!sudokuSolver.isValid(problem)) {
        cout << "\nThe entered Sudoku is invalid.\n";
        return 0;
    }

    vector<vector<int>> solution = sudokuSolver.solveGrid(problem);

    if(solution.empty()) {
        cout << "\nNo solution exists for the given Sudoku.\n";
        return 0;
    }

    board solvedBoard(solution);

    cout << "\n==============================\n";
    cout << "       SOLVED SUDOKU\n";
    cout << "==============================\n\n";

    solvedBoard.printBoard();

    return 0;
}