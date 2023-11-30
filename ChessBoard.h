#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include <vector>
#include <iostream>
#include <memory>
#include "Game.h" // Because we are creating an actual Game object
#include "Empty.h"
#include "King.h"
#include "Pawn.h"
#include "ChessBoardObserver.h"
using namespace std;

class Piece; // Because we are not directly access fields/functions of Piece, a forward declaration suffices
class Observer; // Same reasonin
class Vec;
class Player;
class TextDisplay;
class GraphicsDisplay;
class DisplayObserver;



// CASTLE MOVE
// to make a castle move -> validate that we are moving a king two spaces to the left or right -> have a function that returns true or false so if it is a castle move we know to move the rook as well

// remember that we want to make DEEP copies of pieces so then we need UNIQUE pointers to make sure that we don't accidentally make shallow copies

//td is observer, chessboard is subject

class ChessBoard: public ChessBoardObserver {
    vector<vector<shared_ptr<Piece>>> gb;
    vector<vector<unique_ptr<Empty>>> eb;
    unique_ptr<Observer> playerWhite;
    unique_ptr<Observer> playerBlack;
    // vector<unique_ptr<DisplayObserver>> displays;
    shared_ptr<DisplayObserver> textDisplay;
    shared_ptr<DisplayObserver> graphicsDisplay;
    shared_ptr<TextDisplay> td;
    shared_ptr<GraphicsDisplay> gd;
    Game game;
    bool bCheck;
    bool wCheck;
    bool turn; // true is for white, false is for black
    Vec bKing;
    Vec wKing;
    bool displayScore;
    // one for td pointer
    public:

        string checkString();

        // returns the type of a piece at that coordinate
        char getType(Vec coordinate);

        // returns a piece pointer
        shared_ptr<Piece> getPiece(Vec coordinate);

        // check at that coordinate if a piece is a pawn and it has moved two -> here white represents the CURRENT PLAYER
        bool pawnMovedTwo(Vec coordinate, bool white);

        // check if there is a piece at that coordinate
        bool isThere(Vec coordinate, bool white, vector<vector<shared_ptr<Piece>>> board);

        // replace a piece at coordinate with replacement
        void replacePiece(Vec coordinate, shared_ptr<Piece> replacement);

        // check if a move is valid
        bool isValid(Vec start, Vec end);

        // chessboard ctor
        ChessBoard();

        // setUp players
        void setupPlayers(unique_ptr<Observer> pWhite, unique_ptr<Observer> pblack);

        void regMove(Vec start, Vec end);

        void castleMove(Vec start, Vec end);

        void makeMove(Vec start, Vec end);

        void updatePawnMoved(Vec start, Vec end);

        void updateKingMoved(Vec end);

        void updateKingCoord(Vec end, bool white);

        // change the gameboard based on validated move
        void notify(Vec start, Vec end) override;

        bool testMove(Vec start, Vec end);

        void isAvoidCaptureMove(Vec start, Vec end, vector<vector<shared_ptr<Piece>>> ob);
        void isCaptureMove(Vec start, Vec end, vector<vector<shared_ptr<Piece>>> ob);
        void isCheckMove(Vec start, Vec end);
        void isCheckMateMove(Vec start, Vec end);

        // checks if the king is in check -> ie. any of the opponents legal moves capture king
        bool isCheck(bool white);

        // get the turn status
        bool getTurn();

        // Setup functions for char and piece
        void setupWithChar(char type, Vec coordinate);
        void setupWithPiece(shared_ptr<Piece> p, Vec coordinate);

        // returns a DEEP COPY of an empty piece
        shared_ptr<Piece> getEmptyPiece(Vec coord);

        // main calls this function if a human wants to forfeit
        void forfeit();

        // main calls this function to check if the game has ended
        void endGame();

        // main calls this function to restart a game this will reset gameboard, reset turn, reset playerWhite and playerBlack
        void restartGame();

        // at this point, the move/turn is finished, we want to check if the piece at end is a pawn and return true/false accordingly
        bool upgradePawn(Vec end);

        // set the default board
        void defaultBoard();

        // set white king
        void setWhiteKing(Vec coordinate);

        // set black king
        void setBlackKing(Vec coordinate);

        // Check if it is a valid board
        bool boardIsValid();

        void setTurn(bool turn);

        friend ostream& operator<<(ostream& out, ChessBoard& cb);

};

#endif
