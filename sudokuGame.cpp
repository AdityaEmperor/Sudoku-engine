#include "sudokuGame.h"
using namespace std;

game::game(int s = 9, int d = 1)
{
    size = s;
    time = 0;
    board problem = board(s);
    difficulty = d;
}

void game::generateOrder(vector<int>& order,int boxSize, mt19937& rng)
{
    int n = boxSize * boxSize;

    vector<int> groups(boxSize);

    for (int i = 0; i < boxSize; i++)
        groups[i] = i;

    shuffle(groups.begin(), groups.end(), rng);

    order.clear();
    order.reserve(n);

    for (int group : groups)
    {
        vector<int> positions(boxSize);

        for (int i = 0; i < boxSize; i++)
            positions[i] = i;

        shuffle(positions.begin(), positions.end(), rng);

        for (int pos : positions)
            order.push_back(group * boxSize + pos);
    }
}

void game::puzzelGenerator()
{
    // Validate input
    if ((size != 4 && size != 9 &&
         size != 16 && size != 25) ||
        difficulty < 1 || difficulty > 3)
    {
        cerr << "Invalid Sudoku size or difficulty.\n";
        return;
    }

    int n = size;
    int boxSize = static_cast<int>(sqrt(n));

    // Initialize both boards
    solution.size = n;
    solution.subGridSize = boxSize;
    solution.grid.assign(n, vector<int>(n, -1));

    problem.size = n;
    problem.subGridSize = boxSize;
    problem.grid.assign(n, vector<int>(n, -1));

    // Generate randomized row and column orders
    vector<int> rows, cols, symbols;

    generateOrder(rows, boxSize, rng);
    generateOrder(cols, boxSize, rng);

    // Randomize symbols from 1 to n
    symbols.resize(n);

    for (int i = 0; i < n; i++)
        symbols[i] = i + 1;

    shuffle(symbols.begin(), symbols.end(), rng);

    // Generate the complete solution board
    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n; c++)
        {
            int baseRow = rows[r];
            int baseCol = cols[c];

            int value =
                (baseRow * boxSize
                 + baseRow / boxSize
                 + baseCol) % n;

            solution.grid[r][c] = symbols[value];
        }
    }

    // Copy solution into the playable puzzle
    problem.grid = solution.grid;

    // Determine percentage of empty cells
    int emptyPercent;

    if (difficulty == 1)
        emptyPercent = 30;
    else if (difficulty == 2)
        emptyPercent = 45;
    else
        emptyPercent = 60;

    int totalCells = n * n;
    int emptyCount = totalCells * emptyPercent / 100;

    // Shuffle cell positions
    vector<int> positions(totalCells);

    for (int i = 0; i < totalCells; i++)
        positions[i] = i;

    shuffle(positions.begin(), positions.end(), rng);

    // Remove cells from the playable puzzle only
    for (int i = 0; i < emptyCount; i++)
    {
        int r = positions[i] / n;
        int c = positions[i] % n;

        problem.grid[r][c] = -1;
    }

    // Synchronize the inherited board with the puzzle
    this->board::size = n;
    this->board::subGridSize = boxSize;
    this->board::grid = problem.grid;

    problem.printBoard();
    solution.printBoard();
}