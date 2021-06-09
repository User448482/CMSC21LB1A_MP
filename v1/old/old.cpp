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

    // temp. print out unsafeRows .. debug
    /*
    cout << endl << endl << "unsafeRows: ";
    for (int i: unsafeRows) {
        cout << i << " ";
    }
    */

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


    /*
    // CASE: 0 BOMBS
    if (nBombs == 0) {
        cout << "\nSpecial case detected: Bomb number is 0. Sum will be sum of all bills in the room." << endl;
        for (cell c: willLive) {
            sum += c.value;
        }
    }

    // CASE: 0 MOVES
    if (nMoves == 0) {
        cout << "\nSpecial case detected: Zero moves allowed. Sum will be sum of all unaffected bills.\n";
        for (cell c: willLive) {
            sum += c.value;
        }
        //cout << endl << sum << endl; // debug
    }
    */

    // SWAPPING
    //cout << "\n\nnMoves is " << nMoves << endl; // debug
    if (nMoves != 0) {
        sort(deathRow.begin(), deathRow.end(), greater_key());
        
        /*
        cout << "\nPrinting deathRow:" << endl; // debug
        for (cell c: deathRow) {
            cout << c.value << " "; // debug
        }

        sort(willLive.begin(), willLive.end(), less_key());
        
        cout << "\nPrinting willLive:" << endl; // debug
        for (cell c: willLive) {
            cout << c.value << " "; // debug
        }
        */
        
        cout << endl;
        int nToCheck; // prevents error if k is greater than number of bills that will survivable
        if (nMoves < willLive.size()) {
            nToCheck = nMoves;
        } else {
            nToCheck = willLive.size();
        }
        cout << "\nnToCheck is " << nToCheck << endl; // debug
        cell temp;
        for (int i=0; i<nToCheck; i++) {
            cout << "\ni is now " << i << endl; // debug
            //cout << "\nfor loop ran" << endl; // debug
            //cout << "\nComparing " << willLive[i].value << " with " << deathRow[i].value << endl; // debug
            if (willLive[i].value < deathRow[i].value) {
                temp.value = willLive[i].value;
                temp.row = willLive[i].row;
                temp.column = willLive[i].column;
                //cout << "\ntemp is now: " << temp.value << " " << temp.row << " " << temp.column << endl; // debug
                cout << "Swapped " << deathRow[i].value << " from (" << deathRow[i].row << "," << deathRow[i].column << ") with " << willLive[i].value << " from (" << willLive[i].row << "," << willLive[i].column << ")" << endl;
                //cout << "deathRow[i].value is " << deathRow[i].value << endl; // debug
                //cout << "willLive[i].row is " << willLive[i].row << endl; // debug
                //cout << "willLive[i].column is " << willLive[i].column << endl; // debug
                //cout << "board[willLive[i].row][willLive[i].column] is " << board[willLive[i].row][willLive[i].column] << endl; // debug
                board[willLive[i].row][willLive[i].column] = '0' + deathRow[i].value;
                //cout << "board[willLive[i].row][willLive[i].column] is now " << board[willLive[i].row][willLive[i].column] << endl; // debug                
                willLive[i].value = temp.value;
                willLive[i].row = temp.row;
                willLive[i].column = temp.column;                 
                board[deathRow[i].row][deathRow[i].column] = '0' + temp.value;
                deathRow[i].value = willLive[i].value;
                deathRow[i].row = willLive[i].value;
                deathRow[i].column = willLive[i].column;
                //cout << "deathRow is now " << board[deathRow[i].row][deathRow[i].column] << endl; // debug
                //cout << "willLive is now " << board[willLive[i].row][willLive[i].column] << endl; // debug            
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
            //cout << "\ni: " << i << " is found" << endl; //debug
            for (int j=0; j<nColumns; j++) {
                board[i][j] = '*';
            }
            /* OLD. USING TEMPBAORD. NOT WORKING FOR SOME REASON: MAYBE BC OF THE ENHANCED FOR LOOP
            for (char c: temp_board[i]) {
                cout << endl << "c: " << c << " changed to *" << endl; // debug
                c = '*';
                cout << "\ntemp_board[0][0] is: " << temp_board[0][0] << endl; // debug
            }
            */
        } else { // if safe row
            for (int j=0; j<nColumns; j++) {
                if (unsafeColumns.find(j) != unsafeColumns.end()) { // if column is unsafe
                    board[i][j] = '*';
                }
            }
        }
    }

    //cout << "\ntempboard[0][0] is " << temp_board[0][0] << endl; // debug

    // PRINT OUT AFTER EXPLOSION
    cout << "\nAfter explosion:\n\n";
    for (vector<char> row: board) {
        for (char c: row) {
            cout << c << " ";
        }
        cout << endl;
    }

    // STOPPED HERE

    // temp. print deathRow and willLive
    /*
    cout << "\ndeathRow:\n";
    for (cell c: deathRow) {
        cout << c.value << " | " << c.row << " | " << c.column << endl;
    }
    cout << "\nwillLive:\n";
    for (cell c: willLive) {
        cout << c.value << " | " << c.row << " | " << c.column << endl;
    }
    */


}

bool isValidCell(string str, int i, char* argv[], int &nBombs) {
    //nBombs = 0; // debug
    for (char c: str) {
        if (isdigit(c) == false) {
            //cout << "\nChecking c: " << c << endl; // debug
            if (c != 'B') {
                cout << "\nInvalid character detected: " << c << " on line " << i+2 << " of " << argv[1] << ".\n\n";
                return false;
            } else {
                //cout << "\nnBombs incremented!" << endl; // debug
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
    // str += "\n"; // debug
    string sub = str.substr(0, str.find(' '));
    if (isDigitsOnly(sub) == false) {
        cout << "\nError in line 1 of " << argv[1] << ". Make sure the format is followed properly.\n\n";
        return false;
    }
    nRows = stoi(sub);
    str.erase(0, str.find(' ') + 1 );
    //cout << "nRows is " << nRows << endl; // debug

    sub = str.substr(0, str.find(' '));
    if (isDigitsOnly(sub) == false) {
        cout << "\nError in line 1 of " << argv[1] << ". Make sure the format is followed properly.\n\n";
        return false;
    }
    nColumns = stoi(sub);
    str.erase(0, str.find(' ') + 1 );
    //cout << "nColumns is " << nColumns << endl; // debug

    sub = str.substr(0, str.length());
    if (isDigitsOnly(sub) == false) {
        cout << "\nError in line 1 of " << argv[1] << ". Make sure the format is followed properly.\n\n";
        return false;
    }
    //cout << "sub is " << sub << endl; // debug
    nMoves = stoi(sub);
    //cout << "k is " << k << endl; // debug

    //cout << endl << str << endl << endl; // debug
    //return false; // debug

    // STOPPED HERE // debug

    //vector<string> temp_board; // debug

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

    // temp_board to board

    for (string str: temp_board) {
        board.push_back(vector<char> {});
    }

    for (int i=0; i<temp_board.size(); i++) {
        for (char c: temp_board[i]) {
            board[i].push_back(c);
        }
    }

    // temp. print both boards
    /*
    for (string str: temp_board) {
        cout << str << endl;
    }
    for (int i=0; i<board.size(); i++) {
        for (char c: board[i]) {
            cout << c;
        }
        cout << endl;
    }
    */

    return true; // debug
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

    if (isValidArg(argc, argv) == true) {
        //cout << "yey" << endl; // debug
    } else {
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

    // print before n after + sum

    // temp. print unsafeRows and unsafeColumns
    /*
    cout << "\n\nUnsafe Rows: \n";
    for (int n: unsafeRows) {
        cout << n << endl;
    }
    cout << "\n\nUnsafe Columns: \n";
    for (int n: unsafeColumns) {
        cout << n << endl;
    }
    */

    cout << "\nProgram finished!\n"; // debug

return 0;
}