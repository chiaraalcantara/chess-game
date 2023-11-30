#include "ChessBoard.h"
#include "Pawn.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Empty.h"
#include <utility>

#include "TextDisplay.h"
#include "GraphicsDisplay.h" // We call their notify
#include "Observer.h"
using namespace std;

// DONE
bool ChessBoard::boardIsValid() {
    // One White KIng - good
    // One Black King - good
    // No Pawns on the First Row (Row 0)   -good
    // No Pawns on the Last Row (Row 7) - good
    // Neither king is inCheck
    bool oneBlack = false;
    bool oneWhite = false;

    int rowSize = eb.size();
    int colSize = eb[1].size();

	for (vector<shared_ptr<Piece>> vec : gb){
		for (shared_ptr<Piece> p : vec){
            // CHECKS THAT THERE IS ONLY ONE BLACK PIECE
            if (!oneBlack && (p->getType() == 'k')) {
                oneBlack = true;
            } else if(oneBlack && (p->getType() == 'k')) {
                return false;
            }
            // CHECKS THAT THERE IS ONLY ONE WHITE PIECE
            if (!oneWhite && (p->getType() == 'K')) {
                oneWhite = true;
            } else if(oneWhite && (p->getType() == 'K')) {
                return false;
            }
        }
    }
    // If either white (true) or black (false) are in check, not a valid board
    if (isCheck(true) || isCheck(false)) return false;


    // Checks for Pawns on the first row
	for (shared_ptr<Piece> p : gb[0]){
        if(p->getType() == 'p' || p->getType() == 'P') return false;
    }
    // Checks for Pawns on the last row
    for (shared_ptr<Piece> p : gb[7]){
        if(p->getType() == 'p' || p->getType() == 'P') return false;
    }
    // After board is determined to be valid

	for (vector<shared_ptr<Piece>> vec : gb) {
		for (shared_ptr<Piece> p : vec) {
            if(p->getTeam() = turn) {
                p->getPossibleMoves();
                for (Vec end : p->returnPossibleMoves()) {
                    testMove(p->getCoordinate(), end);
                }
            } // Sets up that piece'possible moves
        }
    }
}


// DONE
char ChessBoard::getType(Vec coordinate){
    shared_ptr<Piece> p = getPiece(coordinate);
    char type = p->getType();
    return type;
}

// DONE
shared_ptr<Piece> ChessBoard::getPiece(Vec coordinate){
    int row = coordinate.getY();
    int col = coordinate.getX();
    return gb[row][col];
}

// DONE
bool ChessBoard::pawnMovedTwo(Vec coordinate, bool white){
    char type = getType(coordinate);
    shared_ptr<Piece> p = getPiece(coordinate);
    std::shared_ptr<Pawn> pawn;
    if (type == 'P' || type == 'p'){ shared_ptr<Pawn> pawn = dynamic_pointer_cast<Pawn>(p); }
    else { return false; }

    if (pawn->pawnMovedTwo(gb, coordinate, white)){ // Idk why has issues
        return true;
    }
    return false;
}

// DONE
void ChessBoard::replacePiece(Vec coordinate, shared_ptr<Piece> replacement){
    int row = coordinate.getY();
    int col = coordinate.getX();
    gb[row][col] = replacement;
}

// DONE
bool ChessBoard::isThere(Vec coordinate, bool white, vector<vector<shared_ptr<Piece>>> board){
	for (vector<shared_ptr<Piece>> vec : board){
		for (shared_ptr<Piece> p : vec){
			if ((p->getType() != ' ' || p->getType() != '_') && p->getCoordinate() == coordinate && p->getTeam() == white){ return true; }
		}
	}
	return false;
}

// DONE
bool twoStep(Vec start, Vec end){
    int startY = start.getY();
    int endY = end.getY();
    if (startY = endY - 2 || startY == endY + 2){ return true; }
    return false;
}

// Attach the player observers to each piece
// It will also now attach the playerWhite and playerBlack pointers
void ChessBoard::setupPlayers(unique_ptr<Observer> pWhite, unique_ptr<Observer> pBlack) {
    // You can make a unique to a shared, but not other way around
    // shared_ptr<Observer> sharedWhite = make_shared<Observer>(pWhite);
    // shared_ptr<Observer> sharedBlack = make_shared<Observer>(pBlack);
    playerWhite = move(pWhite);
    playerBlack = move(pBlack);

    // Iterate through the game board and set up observers for each piece
    // for (auto& row : gb) {
    //     for (auto& p : row) {
    //         if (p) { // Check if the pointer is not null
    //             p->attachWhite(sharedWhite); // We might have to change this to handle shared_ptr not unique_ptr
    //             p->attachBlack(sharedBlack);
    //         }
    //     }
    // }

}

// Piece::Piece(struct Vec coordinate, char type, bool colour): coordinate{coordinate}, type{type}, white{colour} {} Piece constructor
// Empty piece seems to just have a superclass constructor of parent class Piece.
// To create the empty construcotr, what do I actually pass? (Empty(Vec{row,col}, _ or ' ', true or false)))

// FRANKLIN
ChessBoard::ChessBoard() : playerWhite{nullptr}, playerBlack{nullptr}, td{make_unique<TextDisplay>()}, gd{make_unique<GraphicsDisplay>()}, game{}, bCheck{false}, wCheck{false}, turn{true}, bKing{}, wKing{} {
    // Setup the empty board and gameboard
    // unique_ptr<TextDisplay> td, unique_ptr<GraphicDisplay> gd, I have to make this here

    bool back = true;
    for (int row = 0; row < 8; row++) {
        vector<unique_ptr<Empty>> ebRow;
        vector<shared_ptr<Piece>> gbRow;
        for (int col = 0; col < 8; ++col) {
            // gb[row][col] = make_shared<Piece>();
            // Top left corner, the colour of the board is white. Bottom right (7,7) is white as well
            if(back) { // Alternating of black and white
                ebRow.push_back(make_unique<Empty>(Empty(Vec{row, col}, ' ', true)));
                gbRow.push_back(make_shared<Piece>(Empty(Vec{row, col}, ' ', true)));
                back = false;
            } else {
                ebRow.push_back(make_unique<Empty>(Empty(Vec{row, col}, '_', false)));
                gbRow.push_back(make_shared<Piece>(Empty(Vec{row, col}, '_', false)));
                back = true;
            }
        }
        eb.push_back(move(ebRow));
        gb.push_back(move(gbRow));
    }
}

// DONE
void ChessBoard::regMove(Vec start, Vec end){
    shared_ptr<Piece> startPiece = getPiece(start);
    shared_ptr<Piece> endPiece = getPiece(end);

    // find the piece at start and change the coordinates of the piece to end
    startPiece->setCoordinate(end);

    // move the startPiece to end -> now the piece at end is pointing to the address of startPiece
    replacePiece(end, startPiece);
    endPiece = startPiece; // now the endPiece points at the (start)piece that we just moved to the end

    // replace the vector at start with the empty piece -> getEmptyPiece needs to return a shared pointer but UNIQUE copy
    shared_ptr<Piece> emptyPiece = getEmptyPiece(start);

    // put the endPiece at start
    replacePiece(start, emptyPiece);
}

// DONE
void ChessBoard::castleMove(Vec start, Vec end){
    int startX = start.getX();
    int endX = end.getX();

    Vec rookCoord;
    // the y coordinate tells us which colour we want -> keep the y coordinate the same to get the correct rook
    rookCoord.setY(end.getY());

    if (endX = startX + 2){ // moved to the right -> move the rook to the right TO the left
        rookCoord.setY(7);
        Vec endCoord = Vec(end.getX() - 1, end.getY());
        regMove(rookCoord, endCoord);
    } else if (endX = startX - 3){ // moved to the left -> move the rook to the left TO the right
        rookCoord.setY(0);
        Vec endCoord = Vec(end.getX() + 1, end.getY());
        regMove(rookCoord, endCoord);
    }

}

// DONE
void ChessBoard::makeMove(Vec start, Vec end){
     // make the move
    regMove(start, end);
    char startType = getType(end);
    // check if it was a castle move and if it was make the appropriate rook move
    if (startType == 'K' || startType == 'k'){ castleMove(start, end); }
}

// DONE
void ChessBoard::updatePawnMoved(Vec start, Vec end){
    shared_ptr<Piece> endPiece = getPiece(end);
    shared_ptr<Pawn> pawn = dynamic_pointer_cast<Pawn>(endPiece);
    pawn->hasMoved();
    if (twoStep(start, end)){ pawn->hasMovedTwo(); }
}

// DONE
void ChessBoard::updateKingMoved(Vec end){
    shared_ptr<Piece> endPiece = getPiece(end);
    shared_ptr<King> king = dynamic_pointer_cast<King>(endPiece);
    king->hasMoved();
}

// DONE
void ChessBoard::updateKingCoord(Vec end, bool white){
    if (white){
        wKing == end;
    } else {
        bKing == end;
    }
}

// DONE
void ChessBoard::notify(Vec start, Vec end){

    makeMove(start, end);

    char startType = getType(end);

    // we will also change the king's booleans
    if (startType == 'K' || startType == 'k'){
        updateKingCoord(end, turn);
        updateKingMoved(end);
    }

    // check if the piece that moved is a pawn
    if (startType == 'P' || startType == 'p'){
        updatePawnMoved(start, end);
    }

    // update if THE OTHER TEAM is in check
    if (turn){
        bCheck = isCheck(false);
    } else {
        wCheck = isCheck(true);
    }

     // reset the legal moves of every piece // only go through the opponents
    bool isEnd = true;
    for (vector<shared_ptr<Piece>> vec : gb){
		for (shared_ptr<Piece> p : vec){
            p->resetMoves(); // clear all the legal moves
            p->getPossibleMoves(gb); // get the possible moves for this piece
            // test every possible move -> which will add it to the legal moves if it passes
            for (Vec move : p->returnPossibleMoves()){
                if (testMove(p->getCoordinate(), move)){ isEnd = false; };
            }
        }
    }

    shared_ptr<Piece> emptyPiece = getPiece(start);
    shared_ptr<Piece> endPiece = getPiece(end);

    // notify the gd and td
    char startChar = emptyPiece->getType();
    char endChar = endPiece->getType();
    for (unique_ptr<DisplayObserver> o: displays){
        o->notify(start, startChar, end, endChar);
    }
    // change the turn
    turn? false : true;

    if (isEnd) { endGame(); displayScore = true;  }
}

// DONE
// we can assume that the turn player has no moves
void ChessBoard::endGame() {
    if (turn) {
        if (wCheck) { game.updateWhite(false); }
        else { game.updateWhite(true); }
    } else {
        if (bCheck) { game.updateBlack(false); }
        else { game.updateBlack(true); }
    }
}


// DONE
bool ChessBoard::testMove(Vec start, Vec end){
    // make a deep copy of the gb
    // consult about making a deep copy of the board
    // creating a 2D vector of unique pointers by copying from the shared vector
    vector<vector<shared_ptr<Piece>>> boardCopy;

    for (std::vector<std::shared_ptr<Piece>> vec : gb) {
        std::vector<std::shared_ptr<Piece>> uniqueRow;
        for (std::shared_ptr<Piece> p : vec) {
            // Copying data from shared_ptr to unique_ptr
            uniqueRow.push_back(std::make_shared<Piece>(*p));
        }
        boardCopy.push_back(uniqueRow);
    }

    char startType = getType(start);
    Vec wKingCoord;
    Vec bKingCoord;

    if (turn && startType != 'K'){
        wKingCoord = wKing;
    } else if (!turn, startType != 'k'){
        bKingCoord = bKing;
    } else if (turn){
        wKingCoord = start;
    } else {
        bKingCoord = start;
    }

    // now we can make edits on the game board which we will later revert
    makeMove(start, end);
    // we can safely update the booleans of pieces and update

    // we will also change the king's booleans
    if (startType == 'K' || startType == 'k'){
        updateKingCoord(end, turn);
        updateKingMoved(end);
    }

    // check if the piece that moved is a pawn
    if (startType == 'P' || startType == 'p'){
        updatePawnMoved(start, end);
    }

    // TODO: technically to make this more efficient we only have it iterate through the current's possible moves
    for (vector<shared_ptr<Piece>> vec : gb){
		for (shared_ptr<Piece> p : vec){
            if (p->getTeam() == turn){ continue; }
            p->resetMoves(); // clear all the legal moves
            p->getPossibleMoves(gb); // get the possible moves for this piece
        }
    }
    // --------------------- at this point ALL the pieces have possible moves -----------------------------
    // we need to decide if any of these moves will put the opponent's king in check

    // need to check if that move puts the opponent in check
    bool check = isCheck(!turn);
    bool legal = false;

    // we decide its legal -> notify player
    if (!check){
        if (!turn){ playerWhite->notifyLM(start, end); } // if the next turn (opponent is white)
        else { playerBlack->notifyLM(start, end); }
        isCaptureMove(start, end, boardCopy);
        isCheckMove(start, end);
        isCheckMateMove(start, end);
        isAvoidCaptureMove(start, end, boardCopy);
        legal = true;
    }

    // revert the board -> switch the board copy to the gb
    // this swap might not work
    // Yup chatgpt said nada
    swap(gb, boardCopy);


    // revert the king's coordinates
    if (turn){
        swap(wKingCoord, wKing);
    } else {
        swap(bKingCoord, bKing);
    }

    return legal;
    // unique pointers will go out of scope once the function returns
}

// DONE
bool ChessBoard::isCheck(bool white){
    Vec kingCoord;
    if (white){
        kingCoord = wKing;
    } else {
        kingCoord = bKing;
    }

    // compare the legal moves of each piece with the king's coordinates
    for(vector<shared_ptr<Piece>> vec : gb){
        for(shared_ptr<Piece> p : vec){
            if (p->getTeam() == white){ continue; } // skip pieces on our own team
            for(Vec move : p->returnPossibleMoves()){
                if (move == kingCoord){
                    return true;
                }
            }
        }
    }
    return false;
}

void ChessBoard::isCheckMateMove(Vec start, Vec end){
    // get the possible moves of the current
    bool empty = true;
    for (vector<shared_ptr<Piece>> vec : gb){
        for (shared_ptr<Piece> p : vec){
            if (p->getTeam() != turn){ continue;}
            p->resetMoves();
            p->getPossibleMoves(gb);
            if (p->returnPossibleMoves().size() != 0){ empty = false}
        }
    }

    if (isCheck(turn) && empty){
        if (!turn){ playerWhite->notifyCMM(start, end); } // if the next turn (opponent is white)
        else { playerBlack->notifyCMM(start, end); }
    }
}

//  did this move put the current team in check
void ChessBoard::isCheckMove(Vec start, Vec end){
    if (isCheck(turn)){
        if (!turn){ playerWhite->notifyCheckM(start, end); }
        else { playerBlack->notifyCheckM(start, end); }
    }
}

// WILL this move capture any pieces on the opposing team
void ChessBoard::isCaptureMove(Vec start, Vec end, vector<vector<shared_ptr<Piece>>> ob){
    // if there is a piece at the end coordinate -> it is a capture move
    if (isThere(end, turn, ob)){
        if (!turn){ playerWhite->notifyCapM(start, end); } // if the next turn (opponent is white)
        else { playerBlack->notifyCapM(start, end); }
    }
}

// did the move take this piece out of a position to be captured
void ChessBoard::isAvoidCaptureMove(Vec start, Vec end, vector<vector<shared_ptr<Piece>>> ob){
    // assume it is not in a position to be captured
    bool capture = false;
    // was the piece (on the opponent's team) in a position to be captured in the first place in the old board
    // look at the current team's possible moves if they are equal to the piece's start
    for (vector<shared_ptr<Piece>> vec : ob){
        for (shared_ptr<Piece> p : vec){
            if (p->getTeam() != turn){ continue; }
            p->resetMoves();
            p->getPossibleMoves(ob);
            for (Vec move: p->returnPossibleMoves()){
                if (move == start){ capture == true; break;}
            }
        }
    }

    if (!capture){ return; }

    // will the move take the piece out of a position to be captured
    // consider the board after the move was made, will any of the other team's pieces capture mine (which is at end)
    for (vector<shared_ptr<Piece>> vec: gb){
        for (shared_ptr<Piece> p : vec){
            if (p->getTeam() != turn){ continue; }
            p->resetMoves();
            p->getPossibleMoves(gb);
            for (Vec move: p->returnPossibleMoves()){
                if (move == end){ capture == true; break;}
            }
        }
    }

    if (!capture){
        if (!turn){ playerWhite->notifyCheckM(start, end); }
        else { playerBlack->notifyCheckM(start, end); }
    }
}


// DONE
bool ChessBoard::getTurn(){
    return turn;
}

// DONE
// I need to do dynamic type casting.
void ChessBoard::setupWithChar(char type, Vec coordinate) {
    int row = coordinate.getY();
    int col = coordinate.getX();

    if (type == 'K' || type == 'k') {
        gb[row][col] = make_shared<King>(coordinate, type, (type == 'K') ? 1 : 0); // King
    } else if (type == 'Q' || type == 'q') {
        gb[row][col] = make_shared<Queen>(coordinate, type, (type == 'Q') ? 1 : 0); // Queen
    } else if (type == 'P' || type == 'p') {
        gb[row][col] = make_shared<Pawn>(coordinate, type, (type == 'P') ? 1 : 0); // Pawn
    } else if (type == 'N' || type == 'n') {
        gb[row][col] = make_shared<Knight>(coordinate, type, (type == 'N') ? 1 : 0); // Knight
    } else if (type == 'R' || type == 'r') {
        gb[row][col] = make_shared<Rook>(coordinate, type, (type == 'R') ? 1 : 0); // Rook
    } else if (type == 'B' || type == 'b') {
        gb[row][col] = make_shared<Bishop>(coordinate, type, (type == 'B') ? 1 : 0); // Bishop
    } else if (type == ' ' || type == '_') { //These may not be needed but im just having. I dont think it would ever reach it
        gb[row][col] = getEmptyPiece(coordinate);
    } else {
        // Handle other cases or provide a default behavior
    }
}

// DONE
void ChessBoard::setupWithPiece(shared_ptr<Piece> p, Vec coordinate) {
    int row = coordinate.getY();
    int col = coordinate.getX();

    gb[row][col] = p;
    // That should be fine because I am using this to only setup empty pieces.
    // It is fine that the gameboard starts and has empty pieces at first
}

// DONE
shared_ptr<Piece> ChessBoard::getEmptyPiece(Vec coord){
    // access the empty board and get the Piece* we want
    int row = coord.getY();
    int col = coord.getX();
    shared_ptr<Piece> emptyPiece = make_shared<Piece>(*(eb[row][col]));
    // Assume this copy construtor will work

    // return the copied piece
    return emptyPiece;
}

// DONE
void ChessBoard::forfeit(){
    if (turn){
        // update score +1 for black
        game.updateBlack(false);
    } else {
        // update score +1 for white
        game.updateWhite(false);
    }
}

// CHIARA
void ChessBoard::restartGame() {
    for(size_t i = 0; i < eb.size(); ++i) { //The row
        for (size_t j = 0; j < eb[i].size(); ++j) { // The column
        // Remmber, we have a vector<vector<>>>>
            gb[j][i] = make_shared<Piece>(*(eb[i][j]));
            // Assume the copy assignment operator works
        }
    }
    turn = true; // Default turn is always white
    bCheck = false;
    wCheck = false;
    // We don't need to reset bKing and wKing because it will be reset in next
    // game or if not, will just be destroyed.
}

// DONE
bool ChessBoard::upgradePawn(Vec end){
    char type = getType(end);
    if (type == 'P' || type == 'p'){ return true; }
    else { return false; }
}


// DONE
void ChessBoard::setWhiteKing(Vec coordinate){
    wKing = coordinate;

}

// DONE
void ChessBoard::setBlackKing(Vec coordinate){
    bKing = coordinate;

}

// possible moves in pieces (making sure there's no piece there) legal moves ()
void ChessBoard::defaultBoard() {

    // First setup pawns
    for (int i = 0; i < 8; ++i) {
        setupWithChar('P', Vec{i, 1}); // White pawns
        setupWithChar('p', Vec{i, 7}); // Black
        // x, y. This corresponds to second row
    }

    // Whites are the top side of the board. 0,0

    // Setup Rooks
    setupWithChar('R', Vec{0,0});
    setupWithChar('R', Vec{0, 7}); // Whites
    setupWithChar('r', Vec{7,0});
    setupWithChar('r', Vec{7, 7}); // Whites

    // Setup Knights
    setupWithChar('N', Vec{1,0});
    setupWithChar('N', Vec{6, 0}); // Whites
    setupWithChar('n', Vec{1,7});
    setupWithChar('n', Vec{6, 7}); // Black

    // Setup Bishops
    setupWithChar('B', Vec{2,0});
    setupWithChar('B', Vec{5, 0}); // Whites
    setupWithChar('b', Vec{2,7});
    setupWithChar('b', Vec{5, 7}); // Black

    // Setup Kings
    setupWithChar('K', Vec{0,3}); // White King
    setupWithChar('k', Vec{7, 3}); // Black King
    setWhiteKing(Vec{0,3});
    setBlackKing(Vec{7, 3});

    // Setup Queens
    setupWithChar('Q', Vec{0,4}); // White Queen
    setupWithChar('q', Vec{7, 4}); // Black Queen

    for (vector<shared_ptr<Piece>> vec : gb) {
		for (shared_ptr<Piece> p : vec) {
            if(p->getTeam() = turn) {
                p->getPossibleMoves();
                for (Vec end : p->returnPossibleMoves) {
                    testMove(p->getCoordinate(), end);
                }
            } // Sets up that piece'possible moves
        }
    }
}

ostream& operator<<(ChessBoard& cb, ostream& out) {
    out << *(cb.td);
    if (cb.displayScore) out << cb.game << endl;
}
