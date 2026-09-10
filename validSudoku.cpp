#include<iostream>
#include<vector>
#include<unordered_map>
using namespace std;

class Sudoku {
public:

    vector<vector<char>> board;

    bool isValidSudoku() {
        // rows
        for(int i=0; i<9 ; i++) {
            unordered_map<int,int> rowf;
            for(int j=0 ; j<9 ; j++) {
                if(board[i][j] == '.') {
                    continue;
                }
                if(!rowf.contains(board[i][j])) {
                    rowf[board[i][j]] = 0;
                }
                rowf[board[i][j]]++;
                if(rowf[board[i][j]] > 1) {
                    return false;
                } 
            }
        }

        // columns
        for(int j=0; j<9 ; j++) {
            unordered_map<int,int> colf;
            for(int i=0 ; i<9 ; i++) {
                if(board[i][j] == '.') {
                    continue;
                }
                if(!colf.contains(board[i][j])) {
                    colf[board[i][j]] = 0;
                }
                colf[board[i][j]]++;
                if(colf[board[i][j]] > 1) {
                    return false;
                } 
            }
        }

        // subgrids
        for(int x=0; x<=6 ; x=x+3) {
            for(int y=0 ; y<=6 ; y=y+3) {
                unordered_map<int,int> subgridf;
                for(int i=0 ; i<3 ; i++) {
                    for(int j=0 ; j<3 ; j++) {
                        if(board[x+i][y+j] == '.') {
                            continue;
                        }
                        if(!subgridf.contains(board[x+i][y+j])) {
                            subgridf[board[x+i][y+j]] = 0;
                        }
                        subgridf[board[x+i][y+j]]++;
                        if(subgridf[board[x+i][y+j]] > 1) {
                            return false;
                        } 
                    }
                }
            }
        }

        return true;
    }
};

int main () {
    class Sudoku s;
    //vector<vector<char>> grid(9, vector<char>(9, '.'));
    vector<vector<char>> grid = {
    {'5','3','.','.','7','.','.','.','.'},
    {'6','.','.','1','9','5','.','.','.'},
    {'.','9','8','.','.','.','.','6','.'},
    {'8','.','.','.','6','.','.','.','3'},
    {'4','.','.','8','.','3','.','.','1'},
    {'7','.','.','.','2','.','.','.','6'},
    {'.','6','.','.','.','.','2','8','.'},
    {'.','.','.','4','1','9','.','.','5'},
    {'.','.','.','.','8','.','.','7','9'}
    };

    s.board = grid;

    if(s.isValidSudoku()) {
        cout << "VALID" << endl;
    }
    else {
        cout << "INVALID" << endl;
    }
}