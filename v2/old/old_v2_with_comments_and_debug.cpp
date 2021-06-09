/************************************************************
 *  CMSC21 LB1A MACHINE PROBLEM: BILLS AND BOMBS            *
 *  LICUP, JOHN PAOLO                                       *
 *  SANTOS, ETHAN MARK                                      *
 *                                                          *
 *   CHECK THE README.DOCX FILE FOR MORE INFORMATION !!!    *
 *                                                          *
*************************************************************/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <random>
#include <set>
//#include <stdio.h>
//#include <stdlib.h>
#include <string>
//#include <time.h>
#include <vector>
using namespace std;

// STRUCTS

struct cell {
    int value; // bombs have a value of 0
    int row;
    int column;
};

struct hsPlayer {
    int score;
    string name;
};

struct less_key1 { // for sorting cells
    bool operator() (const cell &str1, const cell &str2) {
        return (str1.value < str2.value);
    } 
};

struct greater_key1 { // for sorting cells
    bool operator() (const cell &str1, const cell &str2) {
        return (str1.value > str2.value);
    } 
};

struct less_key2 { // for sorting high scores
    bool operator() (const hsPlayer &str1, const hsPlayer &str2) {
        return (str1.score < str2.score);
    } 
};

struct greater_key2 { // for sorting high scores
    bool operator() (const hsPlayer &str1, const hsPlayer &str2) {
        return (str1.score > str2.score);
    } 
};

// GLOBAL VARIABLES

bool again; // whether to ask again, for input handling
string input; // for input handling
string name; // player nickname for highscores
int nBombs; // number of bombs in the room
int nRows; // number of rows
int nColumns; // number of columns
int nMoves; // number of swappings allowed
int score; // total earnings of player
int stage; // current round
vector<vector<cell>> room; // main vector containing data of room
int answer; // player's guess
set<int> unsafeRows; // set containing indices of unsafe rows
set<int> unsafeColumns; // set containing indices of unsafe columns
vector<cell> willLive; // contains cells that will live, originally
vector<cell> deathRow; // contains cells that will be incinerated, originally
int nChecks; // if nMoves > willLive.size(), there would be a problem on checking, so this disregards the "extra" unneeded moves by setting nChecks to whichever of nMoves or willLive.size() is smaller
cell tempCell; // temp cell for swapping
int correctAnswer; // the correct answer for this round
bool toNextRound; // whether to proceed to next round or not
hsPlayer tempHsPlayer; // temp HsPlayer for getting rankigns
vector<hsPlayer> hsVec; // contains high scores data
bool playAgain; // whether user opted to play again or not

// FUNCTIONS

void cls() { // Clears screen | Learned from: https://stackoverflow.com/questions/17335816/clear-screen-using-c
    cout << "\033[2J\033[1;1H";
}

void displayHS() {
    cout << "\nTop 5 Scores:\n";
    ifstream inHS;
    inHS.open("hs.txt");
    string out;
    while (getline(inHS, out)) {
        cout << out << endl;
    }
    inHS.close();
}

void howToPlay() {
    cls();
    cout << endl << R"(
                       _            ___ _             
  /\  /\_____      __ | |_ ___     / _ \ | __ _ _   _ 
 / /_/ / _ \ \ /\ / / | __/ _ \   / /_)/ |/ _` | | | |
/ __  / (_) \ V  V /  | || (_) | / ___/| | (_| | |_| |
\/ /_/ \___/ \_/\_/    \__\___/  \/    |_|\__,_|\__, |
                                                |___/ 
)" << endl; // ASCII Art | From: http://patorjk.com/software/taag | Learned raw string literal from: https://stackoverflow.com/questions/37765925/ascii-art-in-c
    cout << "Welcome, " << name << "!\n\nEach round, a room with random number of bills and bombs will be generated.\nWhen a bomb 'B' detonates, all bills in the same row and column as any bomb will be incinerated.\n\nThere will also be a random number of 'moves' that you can use to swap any two bills from the room.\nYour goal is to correctly guess the maximum amount of bills that can be saved from the room given the number of times you can swap bills.\n\nIf your guess is correct, you'll move to the next round, adding the amount of saved bills to your total score. \n\nTry to beat the high scores!\n";
    displayHS();
    again = true;
    while (again == true) {
        again = false;
        cout << "\nAre you ready to play? (y/n)\nYour answer: ";
        getline(cin,input);
        if (input == "y" || input == "n") {
            if (input == "n") {
                cout << "Input y when you're ready.\n";
                again = true;
            }
        } else {
            cout << "Invalid input. y or n only.\n";
            again = true;
        }
    }
}

void mainMenu() {
    cls();
    cout << endl << R"(______                 _                           _  ______ _ _ _     
| ___ \               | |                         | | | ___ (_) | |    
| |_/ / ___  _ __ ___ | |__  ___    __ _ _ __   __| | | |_/ /_| | |___ 
| ___ \/ _ \| '_ ` _ \| '_ \/ __|  / _` | '_ \ / _` | | ___ \ | | / __|
| |_/ / (_) | | | | | | |_) \__ \ | (_| | | | | (_| | | |_/ / | | \__ \
\____/ \___/|_| |_| |_|_.__/|___/  \__,_|_| |_|\__,_| \____/|_|_|_|___/
                                                                       
                                                                       )" << endl; // ASCII Art | From: http://patorjk.com/software/taag | Learned homestring literal from: https://stackoverflow.com/questions/37765925/ascii-art-in-c
    cout << "Open the readMe file for more information!!\n";
    

    // DISPLAY HIGH SCORES
    displayHS();

    // PLAYER NAME INPUT HANDLING
    again = true;
    while (again == true) {
        again = false;
        cout << "\nEnter your nickname to play: ";
        getline(cin, input);
        if (input.length() < 1 || input.length() > 10) {
            cout << "Length must be between 1 and 10 characters.\n";
            again = true;
        } else {
            for (char c: input) {
                if (isalnum(c) == 0) {
                    cout << "Invalid character detected: '" << c << "'. You can only use alpha-numeric characters.\n";
                    again = true;
                }

            }
        }
    }
    again = false;
    name = input;
    score = 0;
    stage = 0;
}

int randomInt(int min, int max) {
    //srand(time(0));
    return (rand() % (max - min + 1)) + min;
}

void updateHighScores() {

    // hs.txt to hsVec
    ifstream inHS;
    inHS.open("hs.txt");
    string content, player;
    int playerScore;
    while (getline(inHS, content)) {
        player = content.substr(0, content.find('='));
        //cout << "\nplayer: " << player << endl; // debug
        playerScore = stoi(content.substr(content.find('=')+1, content.length()-content.find('=')-1));
        //cout << "\nplayerScore: " << playerScore << endl; // debug
        //cout << "\nPlayer: " << player << endl; // debug
        //cout << "Score: " << score << endl; // debug
        tempHsPlayer.name = player;
        tempHsPlayer.score = playerScore;
        hsVec.push_back(tempHsPlayer);
    }
    inHS.close();

    // ADD TO HIGH SCORES
    sort(hsVec.begin(), hsVec.end(), greater_key2());
    if (hsVec.size() < 5) {
        cout << "\nYou have made it to top scores!\n";
        tempHsPlayer.name = name;
        tempHsPlayer.score = score;
        hsVec.push_back(tempHsPlayer);
        sort(hsVec.begin(), hsVec.end(), greater_key2());
        // DELETE 6th AND ABOVE
        hsVec.erase(hsVec.begin()+5, hsVec.end());
        // UPDATE hs.txt FILE
        if (remove("hs.txt") == 0) {
            ofstream newHS;
            newHS.open("hs.txt");
            for (int i=0; i<hsVec.size(); i++) {
                newHS << hsVec[i].name << '=' << to_string(hsVec[i].score);
                if (hsVec.size() - i > 1) {
                    newHS << "\n";
                }
            }
            newHS.close();
        } else {
            cout << "\n\nError deleting hs.txt file!!!\n\n";
        }
        // DISPLAY NEW HIGH SCORES
        displayHS();
    } else {
        int lowest = hsVec[hsVec.size()-1].score;
        if (score > lowest) {
            cout << "\nYou have made it to top scores!\n";
            tempHsPlayer.name = name;
            tempHsPlayer.score = score;
            //cout << "\nPushing back: " << tempHsPlayer.name << ", " << tempHsPlayer.score << endl; // debug
            hsVec.push_back(tempHsPlayer);
            sort(hsVec.begin(), hsVec.end(), greater_key2());
            // DELETE 6th AND ABOVE
            hsVec.erase(hsVec.begin()+5, hsVec.end());
            // UPDATE hs.txt FILE
            if (remove("hs.txt") == 0) {
                ofstream newHS;
                newHS.open("hs.txt");
                for (int i=0; i<hsVec.size(); i++) {
                    newHS << hsVec[i].name << '=' << to_string(hsVec[i].score);
                    if (hsVec.size() - i > 1) {
                        newHS << "\n";
                    }
                }
                newHS.close();
            } else {
                cout << "\n\nError deleting hs.txt file!!!\n\n";
            }
            // DISPLAY NEW HIGH SCORES
            displayHS();
        }
    }




    // STOPPED HERE
    // TO DO: PROCEED TO NEXT ROUND, CHECK VARIABLES' DEFAULT VALUES
    // TO DO: CROSS CHECK RESULTS WITH V1

}

void gameOver() {
    cls();
    // Learned raw string literal delimiter from: https://stackoverflow.com/questions/30308088/include-in-raw-string-literal-without-terminating-said-literal | also: https://praveenmatanam.wordpress.com/2009/12/17/warning-backslash-and-newline-separated-by-space/
    cout << endl << R"###(  ______                       _____                   
 / _____)                     / ___ \
| /  ___  ____ ____   ____   | |   | |_   _ ____  ____ 
| | (___)/ _  |    \ / _  )  | |   | | | | / _  )/ ___)
| \____/( ( | | | | ( (/ /   | |___| |\ V ( (/ /| |    
 \_____/ \_||_|_|_|_|\____)   \_____/  \_/ \____)_|    
                                                       
)###" << endl; // ASCII Art | From: http://patorjk.com/software/taag | Learned raw string literal from: https://stackoverflow.com/questions/37765925/ascii-art-in-c
    
    again = true;
    while (again == true) {
        again = false;
        cout << "\nDo you want to play again?\n\nYour answer: ";
        getline(cin, input);
        if (input == "y") {
            playAgain = true;
        }
        else if (input == "n") {
            playAgain = false;
            cout << endl;
        } else {
            again = true;
            cout << "\nInput must be y or n only.";
        }
    }

}

void doTheThing() {

    toNextRound = true;
    while (toNextRound == true) {

        // INITIALIZE DEFAULT VALUES
        correctAnswer = 0;
        toNextRound = false;
        playAgain = false;

        // RANDOMIZE STUFF
        cls();
        stage++;
        srand(time(0)); // For generating random number. Learned from: https://www.geeksforgeeks.org/generating-random-number-range-c/
        nRows = randomInt(3, 15);
        nColumns = randomInt(3, 15);
        nBombs = randomInt(2, 5);
        nMoves = randomInt(0, 5);

        // GENERATE BILLS
        for (int i=0; i<nRows; i++) {
            room.push_back(vector<cell>());
            for (int j=0; j<nColumns; j++) {
                tempCell.value = randomInt(1, 9);
                tempCell.row = i;
                tempCell.column = j;
                room[i].push_back(tempCell);
            }
        }

        // PLACE BOMBS
        vector<int> bombsCoords; // for shuffling, see below
        for (int i=0; i<nRows; i++) {
            for (int j=0; j<nColumns; j++) {
                bombsCoords.push_back((i*10)+j); // generates all possible coordinates in form of 2 digit integers where 1st digit is row and 2nd digit is column
            }
        }
        //random_device rd; // for shuffling vector contents. Learned from: https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector || https://stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library
        //auto rng = default_random_engine {rd};
        //cout << "\nBEFORE RNG\n"; // debug
        default_random_engine rng; // for shuffling vector contents. Learned from: https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector
        shuffle(bombsCoords.begin(), bombsCoords.end(), rng); // bombsCoords is sorted ascendingly, so shuffle then get first nBombs ints
        int tempInt;
        for (int i=0; i<nBombs; i++) {
            int tempInt = bombsCoords[i];
            if (tempInt < 10) { // special case, 0-9 should be 00-09
                room[0][tempInt%10].value = 0;
                unsafeRows.insert(0);
                unsafeColumns.insert(tempInt%10);
            } else {
                room[tempInt/10][tempInt%10].value = 0;
                unsafeRows.insert(tempInt/10);
                unsafeColumns.insert(tempInt%10);
            }
        }

        // GENERATE DISPLAY
        string cellValue;
        cout << "\nROUND " << stage << endl << endl << "Rows: " << nRows << endl << "Columns: " << nColumns << endl << "Number of swapping allowed: " << nMoves << endl << "Number of bombs: " << nBombs << endl << endl;
        for (int i=0; i<nRows; i++) {
            for (int j=0; j<nColumns; j++) {
                if (room[i][j].value == 0) {
                    cellValue = "B";
                } else {
                    cellValue = to_string(room[i][j].value);
                }
                cout << cellValue << ' ';
            }
            cout << endl;
        }

        // ASK FOR ANSWER
        again = true;
        while (again == true) {
            again = false;
            cout << "\nWhat is the largest amount of bills can you save? ";
            getline(cin, input);
            for (char c: input) {
                if (isdigit(c) == 0) {
                    cout << "\nInvalid character: " << c << ". Only integer answers are allowed.\n";
                    again = true;
                    break;
                }
            }
        }
        answer = stoi(input);

        // DETERMINE CELLS THAT WILL LIVE OR DIE
        for (int i=0; i<nRows; i++) {
            if (unsafeRows.find(i) != unsafeRows.end()) {
                for (int j=0; j<nColumns; j++) {
                    deathRow.push_back(room[i][j]);
                }
            } else {
                for (int j=0; j<nColumns; j++) {
                    if (unsafeColumns.find(j) != unsafeColumns.end()) {
                        deathRow.push_back(room[i][j]);
                    } else {
                        willLive.push_back(room[i][j]);
                    }
                }
            }
        }

        if (nMoves != 0) {

            // SORT
            sort(deathRow.begin(), deathRow.end(), greater_key1());
            sort(willLive.begin(), willLive.end(), less_key1());

            // Important: If allowed moves is greater than number of savable bills, the remaining moves must not be used
            if (nMoves < willLive.size()) {
                nChecks = nMoves;
            } else {
                nChecks = willLive.size();
            }

            // SWAP
            cout << "\nRevealing solution...\n\n";
            for (int i=0; i<nChecks; i++) {
                cout << "Swapped " << deathRow[i].value << " from (" << deathRow[i].row << "," << deathRow[i].column << ") with " << willLive[i].value << " from (" << willLive[i].row << "," << willLive[i].column << ")\n";
                tempCell = room[deathRow[i].row][deathRow[i].column];
                room[deathRow[i].row][deathRow[i].column] = room[willLive[i].row][willLive[i].column];
                room[willLive[i].row][willLive[i].column] = tempCell;
                // we also need to reflect the changes (at least on values) in deathRow and willLive for the computation of total later
                tempCell.value = deathRow[i].value;
                deathRow[i].value = willLive[i].value;
                willLive[i].value = tempCell.value;
            }

            // DISPLAY AFTER SWAPPING
            cout << "\nAfter swapping: \n\n";
            for (int i=0; i<nRows; i++) {
                for (int j=0; j<nColumns; j++) {
                    if (room[i][j].value == 0) {
                        cellValue = "B";
                    } else {
                        cellValue = to_string(room[i][j].value);
                    }
                    cout << cellValue << ' ';
                }
                cout << endl;
            }
        }

        // DETONATE BOMBS
        for (int i=0; i<nRows; i++) {
            if (unsafeRows.find(i) != unsafeRows.end()) {
                for (int j=0; j<nColumns; j++) {
                    room[i][j].value = 0;
                }        
            } else {
                for (int j=0; j<nColumns; j++) {
                    if (unsafeColumns.find(j) != unsafeColumns.end()) {
                        room[i][j].value = 0;
                    }
                }
            }
        }

        // DISPLAY AFTER EXPLOSION
        cout << "\nAfter explosion:\n\n";
        for (int i=0; i<nRows; i++) {
            for (int j=0; j<nColumns; j++) {
                if (room[i][j].value == 0) {
                    cellValue = "*";
                } else {
                    cellValue = to_string(room[i][j].value);
                }
                cout << cellValue << ' ';
            }
            cout << endl;
        }

        // DETERMINE TOTAL SAVABLE
        for (int i=0; i<willLive.size(); i++) {
            correctAnswer += willLive[i].value;
        }
        cout << "\nThe correct answer is: " << correctAnswer << endl;

        // CHECK ANSWER
        if (correctAnswer == answer) { // debugging: change this to != if you want the answer to always be correct
            cout << "\nYour answer is... correct! You earned " << correctAnswer << " points this round.\n";
            score += correctAnswer;
            cout << "\nYour total score is now " << score << ". Congratulations!\n";
            toNextRound = true;
            again = true;
            while (again == true) {
                again = false;
                cout << "\nAre you ready for the next round? (y/n)\n";
                cout << "Your answer: ";
                getline(cin, input);
                if (input != "y") {
                    cout << "Input y when you are ready.\n";
                    again = true;
                }
            }
        } else {
            cout << "\nYour answer is... incorrect! Your final score is " << score << ". Better luck next time!\n";
            again = true;
            while (again == true) {
                cout << "\nProceed? (y/n)\nYour answer: ";
                getline(cin, input);
                if (input == "y") {
                    again = false;
                } else {
                    cout << "Input y when you're done reviewing the solution.\n";
                    again = true;
                }
            }
            toNextRound = false;
        }

        // NEXT ROUND / GAME OVER
        if (toNextRound == false) {
            // UPDATE HIGH SCORES
            if (score != 0) {
            updateHighScores();
            }        
            gameOver();
            return;
        }

        // CLEANING
        room.clear();
        bombsCoords.clear();
        unsafeRows.clear();
        unsafeColumns.clear();
        deathRow.clear();
        willLive.clear();
        cout << "\n\nREACHED THIS\n\n"; // debug

    }
}

int main() {

    playAgain = true;
    while (playAgain == true) {
        playAgain = false;
        // Checking highscores file | Learned from: https://www.cplusplus.com/doc/tutorial/files/
        ifstream hsFile("hs.txt");
        if (hsFile.is_open() == false) {
            cout << "\nError accessing hs.txt file. Make sure the file exists. Exiting program...\n\n";
            return 0;
        }
        hsFile.close();
        mainMenu();
        howToPlay();
        doTheThing();
    }

}