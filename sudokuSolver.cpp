#include "sudokuBoard.h"
#include "sudokuSolver.h"
using namespace std;

bool solver::isValid(board problem) {
    // rows
        for(int i=0; i<problem.size ; i++) {
            unordered_map<int,int> rowf;
            for(int j=0 ; j<problem.size ; j++) {
                if(problem.grid[i][j] == -1) {
                    continue;
                }
                if(!rowf.contains(problem.grid[i][j])) {
                    rowf[problem.grid[i][j]] = 0;
                }
                rowf[problem.grid[i][j]]++;
                if(rowf[problem.grid[i][j]] > 1) {
                    return false;
                } 
            }
        }

        // columns
        for(int j=0; j<problem.size ; j++) {
            unordered_map<int,int> colf;
            for(int i=0 ; i<problem.size ; i++) {
                if(problem.grid[i][j] == -1) {
                    continue;
                }
                if(!colf.contains(problem.grid[i][j])) {
                    colf[problem.grid[i][j]] = 0;
                }
                colf[problem.grid[i][j]]++;
                if(colf[problem.grid[i][j]] > 1) {
                    return false;
                } 
            }
        }

        // subgrids
        for(int x=0; x <= subGridSize * (subGridSize-1) ; x = x+subGridSize) {
            for(int y=0; y <= subGridSize * (subGridSize-1) ; y = y+subGridSize) {
                unordered_map<int,int> subgridf;
                for(int i=0 ; i<subGridSize ; i++) {
                    for(int j=0 ; j<subGridSize ; j++) {
                        if(problem.grid[x+i][y+j] == -1) {
                            continue;
                        }
                        if(!subgridf.contains(problem.grid[x+i][y+j])) {
                            subgridf[problem.grid[x+i][y+j]] = 0;
                        }
                        subgridf[problem.grid[x+i][y+j]]++;
                        if(subgridf[problem.grid[x+i][y+j]] > 1) {
                            return false;
                        } 
                    }
                }
            }
        }

        return true;
}

vector<vector<int>> solver::solveGrid(board problem) {

}

vector<vector<vector<int>>> solver::allSolutions(board problem){

}