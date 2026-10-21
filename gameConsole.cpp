#include "sudokuGame.h"

#include <iostream>
#include <limits>
#include <string>

using namespace std;

namespace
{
    void printInstructions()
    {
        cout << "\nCommands:\n"
             << "  row col value  -> enter a value (rows/columns are 1-based)\n"
             << "  row col 0      -> clear a cell\n"
             << "  h              -> use one hint\n"
             << "  c              -> check whether the puzzle is solved\n"
             << "  p              -> print the current board\n"
             << "  q              -> quit\n"
             << "Example: 1 3 5 means put 5 in row 1, column 3.\n\n";
    }

    bool readInt(const string& prompt, int& value)
    {
        cout << prompt;
        if (cin >> value)
            return true;

        if (cin.eof())
            return false;

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter an integer.\n";
        return false;
    }
}

int main()
{
    int gridSize = 9;
    int difficulty = 1;

    cout << "=== Console Sudoku ===\n";

    while (true)
    {
        if (!readInt("Grid size (4, 9, 16, 25): ", gridSize))
            return 0;

        if (gridSize == 4 || gridSize == 9 ||
            gridSize == 16 || gridSize == 25)
            break;

        cout << "Choose 4, 9, 16, or 25.\n";
    }

    while (true)
    {
        if (!readInt("Difficulty (1 = easy, 2 = medium, 3 = hard): ",
                      difficulty))
            return 0;

        if (difficulty >= 1 && difficulty <= 3)
            break;

        cout << "Choose difficulty 1, 2, or 3.\n";
    }

    game sudoku(gridSize, difficulty);
    sudoku.startGame();

    cout << "\nPuzzle generated. Use 1-based row and column numbers.\n";
    printInstructions();

    while (true)
    {
        cout << "\n";
        sudoku.problem.printBoard();
        cout << "\nEnter command (h/c/p/q or row col value): ";

        string command;
        if (!(cin >> command))
            break;

        if (command == "q" || command == "Q")
        {
            cout << "Game exited.\n";
            break;
        }

        if (command == "p" || command == "P")
        {
            sudoku.problem.printBoard();
            continue;
        }

        if (command == "h" || command == "H")
        {
            if (sudoku.hint())
            {
                cout << "Hint applied.\n";
                sudoku.problem.printBoard();
            }
            else
            {
                cout << "No hint available (the board may already be solved).\n";
            }
            continue;
        }

        if (command == "c" || command == "C")
        {
            if (sudoku.check())
            {
                cout << "Correct! Final score: " << sudoku.score() << '\n';
                cout << "Save this result to the score file? (y/n): ";
                char answer;
                cin >> answer;

                if (answer == 'y' || answer == 'Y')
                    sudoku.log();

                cout << "Congratulations!\n";
                break;
            }

            cout << "Not solved yet. Keep going.\n";
            continue;
        }

        // First token should be the row number.
        int row;
        try
        {
            size_t used = 0;
            row = stoi(command, &used);
            if (used != command.size())
                throw invalid_argument("not an integer");
        }
        catch (...)
        {
            cout << "Unknown command. Type h, c, p, q, or row col value.\n";
            continue;
        }

        int col, value;
        if (!(cin >> col >> value))
        {
            if (cin.eof())
                break;

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Expected three integers: row col value.\n";
            continue;
        }

        // Convert from the user's 1-based coordinates to zero-based indexes.
        --row;
        --col;

        if (!sudoku.enterValue(row, col, value))
        {
            cout << "Move rejected. Check the coordinates, value, clue status, "
                    "and Sudoku row/column/subgrid rules.\n";
        }
        else
        {
            cout << "Move accepted.\n";

            if (sudoku.check())
            {
                cout << "Puzzle solved! Final score: " << sudoku.score() << '\n';
                cout << "Save this result to the score file? (y/n): ";
                char answer;
                cin >> answer;

                if (answer == 'y' || answer == 'Y')
                    sudoku.log();

                break;
            }
        }
    }

    return 0;
}
