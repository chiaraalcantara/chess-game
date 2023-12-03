#include <iostream>
#include <string>
#include "ChessBoard.h"
#include "Player.h"
#include "Human.h"
#include "Computer.h"
#include "Piece.h"
#include "Observer.h"
#include "Level.h"

using namespace std;


int convertToInt(char c) {
    if(c >= 'a' && c <= 'h') {
        return (c - 'a'); // 'a' is equal to 97
        // We use 0-7
    } else {
        return -1;
    }
}

// bool checkInBounds(string coord) {

//     return (coord.substr(0,1)[0] >= 'a' && coord.substr(0,1)[0] <= 'h' && coord.substr(1))

// }

bool validPlayer(const string& player) {
    if (player == "human") return true;
    if (player.length() == 11 && player.substr(0, 8) == "computer") {
        int level = player[9] - '0';  // Convert character to integer
        return level >= 1 && level <= 4;
    }
    return false;
}


void outputRules() {
    string art =
    "+----------------------------------------------------+\n"
    "|                                                    |\n"
    "|                    Chess Game                      |\n"
    "|                                                    |\n"
    "|          By: Chiara, Franklin, Helena              |\n"
    "|                                                    |\n"
    "+----------------------------------------------------+\n";
    cout << art << endl;
    cout << "Welcome to our chess game. Please follow the rules and commands strictly.\n";
    cout << "Commands:\n";
    cout << "  - game white-player black-player: Starts a new game. 'white-player' and 'black-player' can be 'human' or 'computer[1-4]'.\n";
    cout << "  - resign: Concede the game to your opponent.\n";
    cout << "  - move <start> <end>: Make a move from <start> to <end>. For castling, use king's initial and final positions.\n";
    cout << "  - move <start> <end> <promotion>: Pawn promotion move.\n";
    cout << "  - move: Computer player will make a move.\n\n";

    cout << "Setup Commands (only available when a game is not running):\n";
    cout << "  - setup: Enter setup mode.\n";
    cout << "  - + <char> <coords>: Add piece at the specified coordinates.\n";
    cout << "  - - <coords>: Remove piece at the specified coordinates.\n";
    cout << "  - = <colour>: Set the next turn to the specified colour.\n";
    cout << "  - done: Exit setup mode, if all setup conditions are met.\n\n";

    cout << "In setup mode, ensure the following conditions are met before exiting:\n";
    cout << "  - One white king and one black king must be on the board.\n";
    cout << "  - No pawns are on the first or last rows of the board.\n";
    cout << "  - Neither king is in check.\n\n";

    cout << "To get started with a match, enter game for a default board or setup for a custom one." << endl;
}


int main() {
    string cmd;
    // Xwindow xw;
    bool usedSetup = false;
    // ChessBoard cb; // Default chessBoard constructor
    shared_ptr<ChessBoard> cb = make_shared<ChessBoard>();

    outputRules();

    while (cin >> cmd) {
        // Make a prompt output to get enter game or setup
        // We prompt the user to enter game or setup
        // If they enter game, they immediately start a game. THey are prompted for player 1 and player 2
        // After that, we check whether they inputted setup before this (inputting setup before this meant they setup a custom board). If false
        // use the default board. If true, that means they used setup beforehand so we use that custome board.
        // If we are to use the default board, we simply default construct that gameboard and get players
        // If we we are to use the custom board, we just get players cuz we already created this custom board in setup.
        // On top of this, we have a isEnd that resets the gameBoard and players.
        shared_ptr<Player> playerWhite;
        shared_ptr<Player> playerBlack;

        

        if(cmd == "game") {
            // Observer -> Player -> Computer or Human. They are subclasses of Observer
            string player1, player2;
            int level, level2;

            // Can now handle invalid input from the user
            while (cin >> player1 >> player2) {
                int length1 = player1.length();
                int length2 = player2.length();

                if (!(validPlayer(player1) || validPlayer(player2))) {
                    cout << "Invalid Input. Please read the rules again. " << endl;
                    cout << "  - game white-player black-player: Starts a new game. 'white-player' and 'black-player' can be 'human' or 'computer[1-4]'" << endl;
                    cin >> cmd;
                    continue;
                } 
                else if (player1 == "human" && player2 == "human") {

                    playerWhite = make_shared<Human>(1, cb);
                    playerBlack = make_shared<Human>(0, cb);
                    cb->setupPlayers(playerWhite, playerBlack); // Then players
                    break;

                } else if (player1 == "human" && player2.substr(0,8) == "computer" ) {

                    try {
                        level2 = stoi(player2.substr(9, 1));
                    } catch (...) {
                        cout << "Invalid input. Please enter a valid level." << endl;
                        continue;
                    }

                    level2 = stoi(player2.substr(9, 1));
                    if (!(level2 >= 1 && level2 <= 4)) continue; 
                    playerWhite = make_shared<Human>(1, cb);
                    playerBlack = make_shared<Computer>(0, cb, level2);

                    cb->setupPlayers(playerWhite, playerBlack); // Then players
                    break;

                } else if (player1.substr(0,8) == "computer" && player2 == "human" ) {

                    try {
                        level = stoi(player1.substr(9, 1)); // Not sure if we can use stoi But this should level = the number in the brackets
                    } catch (...) {
                        cout << "Invalid input. Please enter a valid level." << endl;
                        continue;
                    }

                    level = stoi(player1.substr(9, 1));
                    if (!(level >= 1 && level <= 4)) continue; 
                    playerWhite = make_shared<Computer>(1, cb, level);
                    playerBlack = make_shared<Human>(0, cb);
                    cb->setupPlayers(playerWhite, playerBlack); // Then players
                    break;

                } else if (player1.substr(0,8) == "computer" && player2.substr(0,8) == "computer" ) {


                    try {
                        level = stoi(player1.substr(9, 1)); // Not sure if we can use stoi But this should level = the number in the brackets
                        level2 = stoi(player2.substr(9, 1));
                    } catch (...) {
                        cout << "Invalid input. Please enter a valid level." << endl;
                        continue;
                    }

                    level = stoi(player1.substr(9, 1)); // Not sure if we can use stoi But this should level = the number in the brackets
                    level2 = stoi(player2.substr(9, 1));
                    if (!(level2 >= 1 && level2 <= 4)) continue; 
                    if (!(level >= 1 && level <= 4)) continue; 

                    playerWhite = make_shared<Computer>(1, cb, level);
                    playerBlack = make_shared<Computer>(0, cb, level2);
                    cb->setupPlayers(playerWhite, playerBlack); // Then players
                    break;

                } else {
                    cout << "Invalid Input. Please read the rules again. " << endl;
                    cout << "  - game white-player black-player: Starts a new game. 'white-player' and 'black-player' can be 'human' or 'computer[1-4]'" << endl;
                    cin >> cmd;
                }
                
            }
            
            
            if(!usedSetup) cb->defaultBoard(); // In both cases setup board first
            else cb->setUpStartMoves(); 

            string cmd2;
            cout << *(cb);

            while(cin >> cmd2) {
                if(!cb->getTurn()) { //If passes means playerWhite turn. Remember, everything is negated. 
                    if (cmd2 == "move" && player1 == "human") {

                        string start, end;
                        cin >> start >> end;

                        try {
                            int y = stoi(start.substr(1));
                            int x = convertToInt(start.substr(0,1)[0]);
                            int x2 = convertToInt(end.substr(0,1)[0]);
                            int y2 = stoi(end.substr(1));
                            if (!(x >= 0 && x <= 7 && y >= 1 && y <= 8 && x2 >= 0 && x2 <= 7 && y2 >= 1 && y2 <= 8)) {
                                throw invalid_argument("Invalid coordinates");
                            }
                        } catch (...) {
                            cout << "Invalid input. Please enter valid coordinates." << endl;
                            continue;
                        }
                                            
                        int x = convertToInt(start.substr(0,1)[0]);
                        int y = stoi(start.substr(1));
                        Vec coordinate1 = Vec{x, y - 1};

                        int x2 = convertToInt(end.substr(0,1)[0]);
                        int y2 = stoi(end.substr(1));

                        if(!(x >= 0 && x <= 7 && y>= 1 && y <= 8 && x2 >= 0 && x2 <= 7 && y2 >= 1 && y2 <= 8)) 
                        {cout << "Invalid coordinates. Please input a location on the board." << endl; continue;}
                        cout << "get here" << endl;

                        Vec coordinate2 = Vec{x2, y2 - 1};
                        
                        shared_ptr<Human> humanWhite = dynamic_pointer_cast<Human>(cb->getPlayerWhite());

                        if(humanWhite->makeHumanMove(coordinate1, coordinate2)) {

                            // Valid Move
                            if(cb->upgradePawn(coordinate2)) { 
                                char newPiece;
                                cout << "Please input the character you want the Pawn to be upgraded to : " << endl;
                                while (cin >> newPiece) {
                                    if (!((newPiece == 'Q') || (newPiece == 'R')  || (newPiece == 'N')  || (newPiece == 'B'))){ cout << "Invalid Input. Try Again!"  << endl; continue;}
                                    cb->setupWithChar(newPiece, coordinate2);
                                    break;
                                }
                            } else cin.ignore(80, '\n');


                        } else {

                            cout << "Invalid move. Please retry" << endl;

                        }
                    } else if(cmd2 == "move" && player1.substr(0,8) == "computer") {
                        // Make Computer Move

                        shared_ptr<Computer>  computerWhite = dynamic_pointer_cast<Computer>(cb->getPlayerWhite());

                        // cout << computerWhite->getLevel() << endl;
                        int level = computerWhite->getLevel();

                        Vec end = computerWhite->makeComputerMove(level); // TO DO UPDATE WITH HELENA'S NEW FUNCTION
                        if(cb->upgradePawn(end)){
                            cb->setupWithChar('Q', end);
                        }


                    } else if(cmd2 == "resign") {
                        //Player1 now has to lose

                        cb->forfeit(); // This function will update the white and black score
                        cout << "Current Score: " << endl;
                        cout << *(cb);
                        cb->restartGame(); // Restart match
                        break;

                    } else if (cmd2 == "skip") {

                        cb->setTurn(!cb->getTurn()); 
                        

                    } else {

                        cout << "Invalid Command. Try again" << endl;

                    }
                } else {
                    if (cmd2 == "move" && player2 == "human") {
                        string start, end;
                        cin >> start >> end;

                        try {
                            int y = stoi(start.substr(1));
                            int x = convertToInt(start.substr(0,1)[0]);

                            int x2 = convertToInt(end.substr(0,1)[0]);
                            int y2 = stoi(end.substr(1));
                        
                            if (!(x >= 0 && x <= 7 && y >= 1 && y <= 8 && x2 >= 0 && x2 <= 7 && y2 >= 1 && y2 <= 8)) {
                                throw invalid_argument("Invalid coordinates");
                            }
                        } catch (...) {
                            cout << "Invalid input. Please enter valid coordinates." << endl;
                            continue;
                        }
                          
                        int x = convertToInt(start.substr(0,1)[0]);
                        int y = stoi(start.substr(1));
                        Vec coordinate1 = Vec{x, y - 1};

                        int x2 = convertToInt(end.substr(0,1)[0]);
                        int y2 = stoi(end.substr(1));

                        // if(!(x >= 0 && x <= 7 && y>= 1 && y <= 8 && x2 >= 0 && x2 <= 7 && y2 >= 1 && y2 <= 8)) 
                        // {cout << "Invalid coordinates. Please input a location on the board." << endl; continue;}

                        Vec coordinate2 = Vec{x2, y2 - 1}; // Start at row 0
                        
                        shared_ptr<Human> humanBlack = dynamic_pointer_cast<Human>(cb->getPlayerBlack());
                        
                        if(humanBlack->makeHumanMove(coordinate1, coordinate2)) {

                            if(cb->upgradePawn(coordinate2)) {
                                char newPiece;
                                cout << "Please input the character you want the Pawn to be upgraded to : " << endl;
                                while (cin >> newPiece) {
                                    if (!((newPiece == 'q') || (newPiece == 'r')  || (newPiece == 'n')  || (newPiece == 'n'))) { cout << "Invalid Input. Try Again!"  << endl; continue;}
                                    cb->setupWithChar(newPiece, coordinate2);
                                }  
                            } else cin.ignore(80, '\n');
   

                        } else {

                            cout << "Invalid move. Please retry" << endl;

                        }

                    } else if (cmd2 == "move" && player2.substr(0,8) == "computer") {

                        shared_ptr<Computer> computerBlack = dynamic_pointer_cast<Computer>(cb->getPlayerBlack());
                        int level = computerBlack->getLevel();
                        // cout << computerBlack->getLevel() << endl;
                        
                        Vec end = computerBlack->makeComputerMove(level);
                        if(cb->upgradePawn(end)){
                            cb->setupWithChar('q', end);
                        }

                    } else if(cmd2 == "resign") {

                        // Player2 has to lose
                        cb->forfeit();
                        cout << "Current Score: " << endl;
                        cout << *(cb); 
                        cb->restartGame();
                        break;

                    } else if (cmd2 == "skip") {

                        cb->setTurn(!cb->getTurn()); 
                        
                    } else {
                        
                        cout << "Invalid Command. Try again" << endl;

                    }
                }
                cout << *(cb);
            }
        } else if (cmd == "setup") {
            string cmd2, coord, colour;
            char piece;
            cb->setupTurn(false); // We just want to change the output to White

            while(cin >> cmd2) { 
                
                if(cmd2 == "+") {

                    cin >> piece >> coord;

                    try {
                        int x = convertToInt(coord.substr(0,1)[0]);
                        int y = stoi(coord.substr(1));

                        if ((!(x >=0 && x <=7) || !(y <= 8 && y >= 1)) || !(piece == 'k' || piece == 'K' || piece == 'q' || piece == 'Q' ||
                            piece == 'b' || piece == 'B' || piece == 'n' || piece == 'N' ||
                            piece == 'r' || piece == 'R' || piece == 'p' || piece == 'P')) {
                            throw invalid_argument("Invalid coordinates");
                        }
                    } catch (...) {
                        cout << "Invalid input. Please enter valid coordinates." << endl;
                        continue;
                    }

                    int x = convertToInt(coord.substr(0,1)[0]);
                    int y = stoi(coord.substr(1));
                    Vec coordinate = Vec{x, y- 1};


                    // Need to also check that the char was a proper input TODO
                    // if ((!(x >=0 && x <=7) || !(y <= 8 && y >= 1)) || !(piece == 'k' || piece == 'K' || piece == 'q' || piece == 'Q' ||
                    //     piece == 'b' || piece == 'B' || piece == 'n' || piece == 'N' ||
                    //     piece == 'r' || piece == 'R' || piece == 'p' || piece == 'P')) {
                    //     cout << "Invalid Input. Input a Command Again." << endl;
                    //     // I should not be breaking. I should be in the while loop waiting for commands
                    // } else {
                        if(piece == 'k') cb->setBlackKing(coordinate);
                        // Vec black = cb->getBKing();
                        // cout << "BLACK KING: " << black << endl;
                        if(piece == 'K') cb->setWhiteKing(coordinate);
                        cb->setupWithChar(piece, coordinate);
                        cout << *(cb);
                    // }

                    
        
                } else if (cmd2 == "-") {

                    cin >> coord;

                    try {
                        int x = convertToInt(coord.substr(0,1)[0]);
                        int y = stoi(coord.substr(1));

                         if ((!(x >=0 && x <=7) || !(y <= 8 && y >= 1))) {
                            throw invalid_argument("Invalid coordinates");
                        }
                    } catch (...) {
                        cout << "Invalid input. Please enter valid coordinates." << endl;
                        continue;
                    }


                    int x = convertToInt(coord.substr(0,1)[0]);
                    int y = stoi(coord.substr(1));
                    // Remove a piece on a board by placing an empty piece on that coordinate
                    Vec coordinate = Vec{x, y - 1}; // We have to minus one because we are 0-7
                    // if ((!(x >=0 && x <=7) || !(y <= 8 && y >= 1))) { cout << "Invalid Input. Input a Command Again." << endl; }
                    // else { 
                        cb->setupWithPiece(cb->getEmptyPiece(coordinate), coordinate);
                        cout << *(cb);
                        // }

                } else if (cmd2 == "=") {
                    // From my understanding, "makes it colors turn to go next" means when the game start, it is their turn
                    if (cin >> colour) {

                        if(colour == "black") {
                            cb->setupTurn(1); // Everything is negated
                            cout << *(cb);
                        } else if (colour == "white") {
                            cb->setupTurn(0);
                            cout << *(cb);
                        } else {
                            cout << "Invalid input, please try again." << endl;
                        }
                    } else { cout << "Invalid input, please try again." << endl;}
    
                } else if (cmd2 == "done") {
                    
                    if(cb->boardIsValid()) {
                        usedSetup = true;
                        break; // Only case where we break
                        // Now a flag has been raised telling the main that the game has been setup with a gameboard.
                    }
                    cout << "Invalid Board Setup" << endl;
                    // Now we have to reset the board (as well as Players) and bring the user all the way back up again.
                    cb->restartGame();
                    break;
                }
                
            }
        } else { // Invalid Input

            cout << "Invalid Input, try Again" << endl;

        }
        cout << "To get started with a match, enter game or setup." << endl;
    }
    // END OF GAME
    cb->setDisplayScore(true);
    // cout << "Final Score:" << endl;
    // cout << *(cb); // We not outputting again? 
    cout << endl;
    cout << "Thank you for playing. We hope you enjoyed!" << endl;
    cout << "Make sure to play again!" << endl;
    return 0;
}

