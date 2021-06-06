/****************************************
 * CMSC21 LB1A Machine Problem          *
 * Licup, John Paolo                    *
 * Santos, Ethan Mark                   *
****************************************/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <vector>
using namespace std;

struct cell {
    int value;
    int row;
    int column;
};

struct less_key {
    bool operator() (const cell &str1, const cell &str2) {
        return (str1.value < str2.value);
    } 
};

struct greater_key {
    bool operator() (const cell &str1, const cell &str2) {
        return (str1.value > str2.value);
    } 
};

void doTheThing(vector<vector <char>> &board, vector<string> &temp_board, int nRows, int nColumns, int nMoves, int nBombs, set<int> &unsafeRows, set<int> &unsafeColumns, vector<cell> &deathRow, vector<cell> &willLive, int &sum) {
    
    bool bombInThisRow;
    for (int i=0; i<nRows; i++) {
        bombInThisRow = false;
        for (int j=0; j<nColumns; j++) {
            if (board[i][j] == 'B') {
                bombInThisRow = true;
                unsafeColumns.insert(j);
            }
        }
        if (bombInThisRow == true) {
            unsafeRows.insert(i);
        }
    }

    cell temp;
    for (int i=0; i<nRows; i++) {
        if (unsafeRows.find(i) != unsafeRows.end()) { // if this row is unsafe
            for (int j=0; j<nColumns; j++) {
                if (board[i][j] != 'B') {
                    temp.value = board[i][j] - '0';
                    temp.row = i;
                    temp.column = j;
                    deathRow.push_back(temp);
                }
            }
        } else { // if this row is safe
            for (int j=0; j<nColumns; j++) {
                if (board[i][j] != 'B') {
                    if (unsafeColumns.find(j) != unsafeColumns.end()) { // if this column is unsafe
                        temp.value = board[i][j] - '0';
                        temp.row = i;
                        temp.column = j;
                        deathRow.push_back(temp);
                    } else { // safe row safe column
                        temp.value = board[i][j] - '0';
                        temp.row = i;
                        temp.column = j;
                        willLive.push_back(temp);
                    }
                }
            }
        }
        
    }     
    
    // PRINT OUT BEFORE EXPLOSION
    cout << "\nRoom before explosion:\n\n";
    for (string str: temp_board) {
        for (char c: str) {
            cout << c << " ";
        }
        cout << endl;
    }

    // SWAPPING
    if (nMoves != 0) {
        sort(deathRow.begin(), deathRow.end(), greater_key());
        cout << endl;
        int nToCheck; // prevents error if k is greater than number of bills that will survivable
        if (nMoves < willLive.size()) {
            nToCheck = nMoves;
        } else {
            nToCheck = willLive.size();
        }
        cell temp;
        for (int i=0; i<nToCheck; i++) {
            if (willLive[i].value < deathRow[i].value) {
                temp.value = willLive[i].value;
                temp.row = willLive[i].row;
                temp.column = willLive[i].column;
                cout << "Swapped " << deathRow[i].value << " from (" << deathRow[i].row << "," << deathRow[i].column << ") with " << willLive[i].value << " from (" << willLive[i].row << "," << willLive[i].column << ")" << endl;
                board[willLive[i].row][willLive[i].column] = '0' + deathRow[i].value;
                willLive[i].value = deathRow[i].value;
                willLive[i].row = deathRow[i].row;
                willLive[i].column = deathRow[i].column;                 
                board[deathRow[i].row][deathRow[i].column] = '0' + temp.value;
                deathRow[i].value = temp.value;
                deathRow[i].row = temp.value;
                deathRow[i].column = temp.column;
            }
        }
    }

    // PRINT OUT AFTER SWAPPING
    cout << "\nAfter Swapping:\n\n";
    for (vector<char> row: board) {
        for (char c: row) {
            cout << c << " ";
        }
        cout << endl;
    }

    // DETONATE BOMBS
    for (int i=0; i<nRows; i++) {
        if (unsafeRows.find(i) != unsafeRows.end()) { // if row is unsafe
            for (int j=0; j<nColumns; j++) {
                board[i][j] = '*';
            }
        } else { // if safe row
            for (int j=0; j<nColumns; j++) {
                if (unsafeColumns.find(j) != unsafeColumns.end()) { // if column is unsafe
                    board[i][j] = '*';
                }
            }
        }
    }

    // PRINT OUT AFTER EXPLOSION
    cout << "\nAfter explosion:\n\n";
    for (vector<char> row: board) {
        for (char c: row) {
            cout << c << " ";
        }
        cout << endl;
    }

    // GET AND DISPLAY SUM
    for (cell c: willLive) {
        sum += c.value;
    }
    cout << "\nMaximum amount that can be saved: " << sum << " bills.\n" << endl;

}

bool isValidCell(string str, int i, char* argv[], int &nBombs) {
    for (char c: str) {
        if (isdigit(c) == false) {
            if (c != 'B') {
                cout << "\nInvalid character detected: " << c << " on line " << i+2 << " of " << argv[1] << ".\n\n";
                return false;
            } else {
                nBombs++;
            }
        }
    }
    return true;
}

bool isDigitsOnly(string str) {
    for (char c: str) {
        if (isdigit(c) == false) {
            return false;
        }
    }
    return true;
}

bool isValidBoard(char* argv[], vector<vector<char>> &board, vector<string> &temp_board, int &nRows, int &nColumns, int &nMoves , int &nBombs) {
    ifstream txtFile(argv[1]);
    string str;
    getline(txtFile, str);
    string sub = str.substr(0, str.find(' '));
    if (isDigitsOnly(sub) == false) {
        cout << "\nError in line 1 of " << argv[1] << ". Make sure the format is followed properly.\n\n";
        return false;
    }
    nRows = stoi(sub);
    str.erase(0, str.find(' ') + 1 );

    sub = str.substr(0, str.find(' '));
    if (isDigitsOnly(sub) == false) {
        cout << "\nError in line 1 of " << argv[1] << ". Make sure the format is followed properly.\n\n";
        return false;
    }
    nColumns = stoi(sub);
    str.erase(0, str.find(' ') + 1 );

    sub = str.substr(0, str.length());
    if (isDigitsOnly(sub) == false) {
        cout << "\nError in line 1 of " << argv[1] << ". Make sure the format is followed properly.\n\n";
        return false;
    }
    nMoves = stoi(sub);

    while (getline(txtFile, str)) {
        temp_board.push_back(str);
    }

    for (int i=0; i<temp_board.size(); i++) {
        if (isValidCell(temp_board[i], i, argv, nBombs) == false) {
            return false;
        }
    }

    if (nRows != temp_board.size()) {
        cout << "\nExpected " << nRows << " row(s). Detected " << temp_board.size() << " row(s). Please check line 1 of txt file.\n\n";
        return false;
    }

    for (int i=0; i<temp_board.size(); i++) {

        if (temp_board[i].size() != nColumns) {
            cout << "\nError in line " << i+2 << " of " << argv[1] << ". Expected " << nColumns << " column(s). Detected " << temp_board[i].size() << " column(s).\n\n";
            return false;
        }
    }

    for (string str: temp_board) {
        board.push_back(vector<char> {});
    }

    for (int i=0; i<temp_board.size(); i++) {
        for (char c: temp_board[i]) {
            board[i].push_back(c);
        }
    }

return true; // if passed all tests
}

bool isValidArg(int argc, char* argv[]) { // checks if argc is only 2 (1 for filename, 1 for input txt file)
    if (argc != 2) {
        cout << "\nError! Argument count must be equal to 2. One for the program name and one for the input .txt file.\n\n";
        return false;
    }
    ifstream txtFile(argv[1]);
    if (txtFile.is_open() == false) {
        cout << "\nError opening file " << argv[1] << ". Please restart the program.\n\n";
        txtFile.close();
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {

    if (isValidArg(argc, argv) != true) {
        return 0;
    }

    vector<vector<char>> board;
    vector<string> temp_board;
    int nRows, nColumns, nMoves;
    int nBombs = 0;

    if (isValidBoard(argv, board, temp_board, nRows, nColumns, nMoves, nBombs) == false) {
        return 0;
    }

    set<int> unsafeRows;
    set<int> unsafeColumns;
    vector<cell> deathRow;
    vector<cell> willLive;
    int sum = 0;
    doTheThing(board, temp_board, nRows, nColumns, nMoves, nBombs, unsafeRows, unsafeColumns, deathRow, willLive, sum);

return 0;
}