#include "sudokuBoard.h"
#include "sudokuSolver.h"

using namespace std;

bool solver::isValid(const board &problem) {

    for(int i = 0; i < problem.size; i++) {

        unordered_map<int, int> rowf;

        for(int j = 0; j < problem.size; j++) {

            if(problem.grid[i][j] == -1) {
                continue;
            }

            if(rowf.find(problem.grid[i][j]) == rowf.end()) {
                rowf[problem.grid[i][j]] = 0;
            }

            rowf[problem.grid[i][j]]++;

            if(rowf[problem.grid[i][j]] > 1) {
                return false;
            }
        }
    }

    for(int j = 0; j < problem.size; j++) {

        unordered_map<int, int> colf;

        for(int i = 0; i < problem.size; i++) {

            if(problem.grid[i][j] == -1) {
                continue;
            }

            if(colf.find(problem.grid[i][j]) == colf.end()) {
                colf[problem.grid[i][j]] = 0;
            }

            colf[problem.grid[i][j]]++;

            if(colf[problem.grid[i][j]] > 1) {
                return false;
            }
        }
    }

    for(int x = 0; x <= problem.subGridSize * (problem.subGridSize - 1); x += problem.subGridSize) {

        for(int y = 0; y <= problem.subGridSize * (problem.subGridSize - 1); y += problem.subGridSize) {

            unordered_map<int, int> subgridf;

            for(int i = 0; i < problem.subGridSize; i++) {

                for(int j = 0; j < problem.subGridSize; j++) {

                    if(problem.grid[x + i][y + j] == -1) {
                        continue;
                    }

                    if(subgridf.find(problem.grid[x + i][y + j]) == subgridf.end()) {
                        subgridf[problem.grid[x + i][y + j]] = 0;
                    }

                    subgridf[problem.grid[x + i][y + j]]++;

                    if(subgridf[problem.grid[x + i][y + j]] > 1) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool solver::solve(board &solution) {

    for(int i = 0; i < solution.size; i++) {

        for(int j = 0; j < solution.size; j++) {

            if(solution.grid[i][j] == -1) {

                for(int num = 1; num <= solution.size; num++) {

                    solution.grid[i][j] = num;

                    if(solver::isValid(solution)) {

                        if(solve(solution)) {
                            return true;
                        }
                    }

                    solution.grid[i][j] = -1;
                }

                return false;
            }
        }
    }

    return true;
}

vector<vector<int>> solver::solveGrid(const board &problem) {

    board solution = problem;

    if(!isValid(problem)) {
        return {};
    }

    if(!solve(solution)) {
        return {};
    }

    return solution.grid;
}

vector<vector<vector<int>>> solver::allSolutions(const board &problem) {

    return {};
}