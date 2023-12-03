#include "ChessBoard.h"
#include "TextDisplay.h"
#include "GraphicsDisplay.h" 
using namespace std;

// ISSUE: verify the board checks if there are no BLACK pawns on the first row and no WHITE pawns on the last row 
// checks if there exists one white king, one back king, no black pawns on the first row, no white pawns on the last row
// and neither king is in check 
bool ChessBoard::boardIsValid() {
    bool oneBlack = false;
    bool oneWhite = false;

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

    // if there are only white or only black pieces return false 
    if (!oneBlack || !oneWhite) return false;

    // ISSUE: THIS IS CHECK FUNCTION ISNT TOTALLY RIGHT 
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
  
    td->notify(turn);
    
    return true;
}

// decides the starting legal moves when a user calls setup
void ChessBoard::setUpStartMoves(){
    // assume the game has ended unless the starting turn has legal moves 
    bool isEnd = true;

    // keep track of legal moves 
    vector<Vec> legalMoves;

    // decide the starting turn's legal moves 
    for (vector<shared_ptr<Piece>> vec : gb) {
		for (shared_ptr<Piece> p : vec) {
            if (p->getType() == ' ' || p->getType() == '_' || 
            
            // ISSUE: WE SHOULDN'T NEED THIS LINE 
            p == nullptr
            
            ){ continue; }
            p->getPossibleMoves(gb);
            Vec v = p->getCoordinate();
            vector<Vec> moves = p->returnPossibleMoves();
            for (Vec end : moves) {
                    if (testMove(v, end, true)){ 
                        if(p->getTeam() != turn) { 
                            isEnd = false;
                        } else {
                            legalMoves.push_back(end);
                        }
                    }
                }
        }
    }

    // ISSUE: WE SHOULDN'T NEED TO DO THIS CHECK SINCE THE BOARD CANNOT START IN CHECK -> GOOD FOR TESTING THO 
    validCheck(legalMoves);

    // if the game starts in stalemate end the game 
    if (isEnd) { endGame(); displayScore = true;  }
}


// gets the type of piece at coordinate 
char ChessBoard::getType(Vec coordinate){
    shared_ptr<Piece> p = getPiece(coordinate);
    char type = p->getType();
    return type;
}

// gets the piece at coordinate 
shared_ptr<Piece> ChessBoard::getPiece(Vec coordinate){
    int row = coordinate.getY();
    int col = coordinate.getX();
    return gb[row][col];
}


// determines if the pawn at coordinate has moved two 
bool ChessBoard::pawnMovedTwo(Vec coordinate, bool white){
    char type = getType(coordinate);
    shared_ptr<Piece> p = getPiece(coordinate);
    shared_ptr<Pawn> pawn;
    if (type == 'P' || type == 'p'){ shared_ptr<Pawn> pawn = dynamic_pointer_cast<Pawn>(p); }
    else { return false; }
    return pawn->pawnMovedTwo(gb, coordinate, white);
}

// replaces the piece at coordinate with replacement 
void ChessBoard::replacePiece(Vec coordinate, shared_ptr<Piece> replacement){
    int row = coordinate.getY();
    int col = coordinate.getX();
    gb[row][col] = replacement;
}

// check if there is a piece on the team "white" at coordinate on the gameboard
bool ChessBoard::isThere(Vec coordinate, bool white, vector<vector<shared_ptr<Piece>>> board){
	for (vector<shared_ptr<Piece>> vec : board){
		for (shared_ptr<Piece> p : vec){
			if ((p->getType() != ' ' && p->getType() != '_') && p->getCoordinate() == coordinate && p->getTeam() == white){ return true; }
		}
	}
	return false;
}

// determines if a move is a twostep move 
bool twoStep(Vec start, Vec end){
    int startY = start.getY();
    int endY = end.getY();
    if (startY == endY - 2 || startY == endY + 2){ return true; }
    return false;
}

// Attach the player observers to the chessboard 
void ChessBoard::setupPlayers(shared_ptr<Observer> pWhite, shared_ptr<Observer> pBlack) {
    if (playerWhite != nullptr && playerBlack != nullptr) return;
    playerWhite = pWhite;
    playerBlack = pBlack;
}

// ISSUE: PUT BACK THE GRAPHICS STUFF
// gd{make_shared<GraphicsDisplay>()}
// ChessBoard constructor 
ChessBoard::ChessBoard() : playerWhite{nullptr}, playerBlack{nullptr}, td{make_shared<TextDisplay>()}, game{}, bCheck{false}, wCheck{false}, turn{false}, bKing{}, wKing{}, displayScore{false} {
    // Setup the empty board and gameboard
    for (int row = 0; row < 8; row++) {
        vector<unique_ptr<Empty>> ebRow;
        vector<shared_ptr<Piece>> gbRow;
        for (int col = 0; col < 8; ++col) {
            if((((row % 2 == 0) && (col % 2 == 0)) || ((row % 2 == 1) && (col % 2 == 1)))) { 
                ebRow.push_back(make_unique<Empty>(Empty(Vec{row, col}, '_', true)));
                shared_ptr<Empty> emptyPtr = std::make_shared<Empty>(Vec{row, col}, '_', true);
                gbRow.push_back(dynamic_pointer_cast<Piece>(emptyPtr));
            } else {
                ebRow.push_back(make_unique<Empty>(Empty(Vec{row, col}, ' ', false)));
                shared_ptr<Empty> emptyPtr = std::make_shared<Empty>(Vec{row, col}, ' ', false);
                gbRow.push_back(dynamic_pointer_cast<Piece>(emptyPtr));
            }
        }
        eb.push_back(move(ebRow));
        gb.push_back(move(gbRow));
    }
    // ISSUE: I DON'T THINK THIS SHOULD BE HERE 
    // graphicsDisplay = gd;
}

// makes a regular move on the board 
void ChessBoard::regMove(Vec start, Vec end){
    shared_ptr<Piece> startPiece = getPiece(start);
    shared_ptr<Piece> endPiece = getPiece(end);

    // find the piece at start and change the coordinates of the piece to end
    startPiece->setCoordinate(end);

    // move the startPiece to end -> now the piece at end is pointing to the address of startPiece
    replacePiece(end, startPiece);

    // now the endPiece points at the (start)piece that we just moved to the end
    endPiece = startPiece; 

    // replace the vector at start with the empty piece 
    shared_ptr<Piece> emptyPiece = getEmptyPiece(start);

    // put the emptyPiece at start
    replacePiece(start, emptyPiece);
}

// DONE
void ChessBoard::castleMove(Vec start, Vec end){
    int startX = start.getX();
    int endX = end.getX();
    Vec rookCoord;

    // keep the y coordinate the same to get the correct rook
    rookCoord.setY(end.getY());

    // castle left 
    if (endX == startX - 2){ 
        rookCoord.setX(0);
        Vec endCoord = Vec(end.getX() + 1, end.getY());
        regMove(rookCoord, endCoord);
    // castle right
    } else if (endX == startX + 3){ 
        rookCoord.setX(7);
        Vec endCoord = Vec(end.getX() - 1, end.getY());
        regMove(rookCoord, endCoord);
    }
}

// verifies a legal move is a castle move and notifies the display
void ChessBoard::isCastleMove(Vec start, Vec end){
    int col = start.getX();
    int endRow = end.getY();
    int endCol = end.getX();
    Vec rookCoord;

    // keep the y coordinate the same to get the correct rook
    rookCoord.setY(end.getY());

    // castle right 
    if ((gb[endRow][endCol]->getType() == 'K' || gb[endRow][endCol]->getType() == 'k')  && (col == endCol - 3)){
        Vec endCoord = Vec(end.getX() - 1, end.getY());
        rookCoord.setX(7);
        char rook = gb[rookCoord.getY()][rookCoord.getX()]->getType();
        char endPiece = gb[endCoord.getY()][endCoord.getX()]->getType();
        td->notifyMoves(rookCoord, rook, endCoord, endPiece, checkString());
    // castle left 
    } else if ((gb[endRow][endCol]->getType() == 'K' || gb[endRow][endCol]->getType() == 'k')  && (col == endCol + 2)){
        Vec endCoord = Vec(end.getX() + 1, end.getY());
        rookCoord.setX(0);
        char rook = gb[rookCoord.getY()][rookCoord.getX()]->getType();
        char endPiece = gb[endCoord.getY()][endCoord.getX()]->getType();
        td->notifyMoves(rookCoord, rook, endCoord, endPiece, checkString());
    }
}

// removes the captured piece from the board from en passant 
void ChessBoard::passantMove(Vec start, Vec end){
    shared_ptr<Piece> p = getPiece(end);
    char type = p->getType();
    Vec rightDiagonal;
    Vec leftDiagonal;
    Vec captureRight = Vec(start.getX() + 1, start.getY());
    Vec captureLeft = Vec(start.getX() - 1, start.getY());
    // black makes an en passant 
    if (type == 'p'){
        rightDiagonal = Vec(start.getX() + 1, start.getY() - 1);
        leftDiagonal = Vec(start.getX() - 1, start.getY() - 1);
        // capture to the right
        if (end == rightDiagonal){ 
            if (pawnMovedTwo(captureRight, true)){ td->notify(captureRight, getEmptyPiece(captureRight)->getType()); } 
        // capture to the left
        } else if ( end == leftDiagonal ){
            if (pawnMovedTwo(captureLeft, true)) { td->notify(captureLeft, getEmptyPiece(captureLeft)->getType()); }
        }  
    // white makes an en passant 
    } else if (type == 'P'){
        rightDiagonal = Vec(start.getX() + 1, start.getY() + 1);
        leftDiagonal = Vec(start.getX() - 1, start.getY() + 1);
        // capture to the right
        if ( end == rightDiagonal ){        
            if (pawnMovedTwo(captureRight, false)){ td->notify(captureRight, getEmptyPiece(captureRight)->getType()); }
        // capture to the left 
        } else if ( end == leftDiagonal ){
            if (pawnMovedTwo(captureLeft, false)) { td->notify(captureLeft, getEmptyPiece(captureLeft)->getType()); }
        }
    }
}

// makes any type of move on the board 
void  ChessBoard::makeMove(Vec start, Vec end){
    char startType = getType(start);
    // check if it was a castle move and if it was make the appropriate rook move
    if (startType == 'K' || startType == 'k'){ castleMove(start, end);}
    // make the move
    regMove(start, end);
}


// update if a pawn has moved on the board 
void ChessBoard::updatePawnMoved(Vec start, Vec end){
    shared_ptr<Piece> endPiece = getPiece(end);
    shared_ptr<Pawn> pawn = dynamic_pointer_cast<Pawn>(endPiece);
    pawn->hasMoved();
    if (twoStep(start, end)){ pawn->hasMovedTwo(); }
}

// update if a king has moved on the board 
void ChessBoard::updateKingMoved(Vec end){
    shared_ptr<Piece> endPiece = getPiece(end);
    shared_ptr<King> king = dynamic_pointer_cast<King>(endPiece);
    king->hasMoved();
}

// ISSUE: DOESN'T ROOK ALSO HAVE A MOVED BOOLEAN -> I NEVER UPDATED IT WHOOPS 

// update the king's coordinates if they change on the board 
void ChessBoard::updateKingCoord(Vec end, bool white){
    if (!white){
        wKing = end;
    } else {
        bKing = end;
    }
}

// ISSUE: THIS IS WRONG AND NEEDS TO BE REPLACED BY THE OTHER ONE IF THERE'S TIME 
bool ChessBoard::isCheck(bool white){
    Vec kingCoord;
    if (white){ 
        kingCoord = bKing;
    } else {
        kingCoord = wKing;
    }

    // compare the legal moves of each piece with the king's coordinates
    for(vector<shared_ptr<Piece>> vec : gb){
        for(shared_ptr<Piece> p : vec){
            // skip pieces not on the specified team 
            if (p->getTeam() != white){ continue; } 
            for(Vec move : p->returnPossibleMoves()){
                if (move == kingCoord){
                    return true;
                }
            }
        }
    }
    return false;
}

// identifies if a team is in check and updates the booleans 
void ChessBoard::validCheck(vector<Vec> legalMoves){
    bool check = false;
    cout << "King Coordinate Black : " << bKing << endl;
    cout << "King Coordinate White : " << wKing << endl;
    for (Vec move : legalMoves){
        cout << "Moves : " << move << endl;
        if(turn){ 
            cout << "here" << endl;
            if(bKing == move) {
                check = true; 
                break; 
            }
        } else { 
            if (wKing == move) {
                check = true;  // Fixed
                break;
            } 
        }
    }
    if (turn) { bCheck = check; }
    else wCheck = check; 
}

// identifies if a team is in check and updates the booleans 
// Franklin Created (Passed the legalMoves of enemy, and the )
// Example: legalMoves of Black , playerTurn is false (signifiying white because we want to see if any of black's moves puts white into check)
bool ChessBoard::IsvalidCheck(vector<Vec> legalMoves, bool playerTurn){
    bool check = false;
    for (Vec move : legalMoves){
        if(playerTurn){  // True for white, false for black
            if(bKing == move) {
                check = true; 
                break; 
            }
        } else { 
            if (wKing == move) {
                check = true;  // Fixed
                break;
            } 
        }
    }
    return check; 

}


// modifies the gameboard and notifies the next player of their legal moves 
void ChessBoard::notify(Vec start, Vec end){
    
    // alter the game board with the move 
    makeMove(start, end);
    
    char startType = getType(end);

    // we will also change the king's booleans
    if (startType == 'K' || startType == 'k'){
        updateKingCoord(end, !turn);
        updateKingMoved(end);
    }

    // check if the piece that moved is a pawn
    if (startType == 'P' || startType == 'p'){
        updatePawnMoved(start, end);
    }

    bool isEnd = true;
    turn = !turn;
    vector<Vec> legalMoves;

    // get the legal moves of the current player to see if the opponent is in check 
    // get the legal moves of the next player to predict their moves 
    for (vector<shared_ptr<Piece>> vec : gb){
		for (shared_ptr<Piece> p : vec){
            if (p->getType() == ' ' || p->getType() == '_') { continue; }
            p->resetMoves(); // clear all the legal moves
            p->getPossibleMoves(gb); // get the possible moves for this piece
            Vec v = p->getCoordinate();
            vector<Vec> moves = p->returnPossibleMoves();
            // test every possible move 
            cout << "START: "<< v << endl;
            for (Vec move : moves){
                // if the piece is on the next turn's team then see if its legal -> if it has any possible moves the game should continue
                if (p->getTeam() != turn && testMove(v, move, true)){
                    // cout << "firs if statement " << move << endl;
                    isEnd = false;
                // if the piece is on the current team's moves append them to legal moves and see if the opponent is in check 
                } else if (testMove(v, move, false)){
                   //  cout << "second if statement " << move << endl;
                    legalMoves.push_back(move); 
                }
            }
        }
    }

    shared_ptr<Piece> emptyPiece = getPiece(start);
    shared_ptr<Piece> endPiece = getPiece(end);

    // ISSUE: DOES A PLAYER GET TAKEN OUT OF CHECK? 

    // update if the next player is in check 
    validCheck(legalMoves);

    
        // notify the gd and td
    char startChar = emptyPiece->getType();
    char endChar = endPiece->getType();
    td->notifyMoves(start, startChar, end, endChar, checkString());
    // graphicsDisplay->notifyMoves(start, startChar, end, endChar, checkString());
    // ISSUE: ADD NOTIFY GD IN ISCASTLE AND PASSANTMOVE
    isCastleMove(start, end);
    passantMove(start, end);
    td->notify(turn);
    
    if (isEnd) { endGame(); displayScore = true;  }
}

// update the check string
string ChessBoard::checkString(){
    if (wCheck){ return "White"; }
    else if (bCheck) { return "Black"; }
    return "";
}

// DONE
void ChessBoard::endGame() {
    if (turn) {
        cout << "bCheck : " << bCheck << endl;
        cout << "wCheck : " << wCheck << endl;
        if (bCheck) { game.updateWhite(false); }
        else { game.updateWhite(true); }
    } else {
        cout << "bCheck : " << bCheck << endl;
        cout << "wCheck : " << wCheck << endl;
        if (wCheck) { game.updateBlack(false); }
        else { game.updateBlack(true); }
    }
}


// DONE
bool ChessBoard::testMove(Vec start, Vec end, bool notify){

    vector<vector<shared_ptr<Piece>>> boardCopy;

    // copy the original board 
    for (std::vector<std::shared_ptr<Piece>> vec : gb) {
        std::vector<std::shared_ptr<Piece>> uniqueRow;
        for (std::shared_ptr<Piece> p : vec) {
            shared_ptr<Piece> newPiece = p->clone();
            uniqueRow.push_back(newPiece);
        }
        boardCopy.push_back(uniqueRow);
    }

    char startType = getType(start);
    Vec wKingCoord;
    Vec bKingCoord;

    // save the coordinates of the king 
    if (!turn && startType != 'K'){
        wKingCoord = wKing;
    } else if (turn && startType != 'k'){
        bKingCoord = bKing;
    } else if (!turn){
        wKingCoord = start;
    } else {
        bKingCoord = start;
    }

    // now we can make edits on the game board which we will later revert
    makeMove(start, end);

    // we will also change the king's booleans
    // ISSUE: DO I HAVE TO UPDATE THE ROOK COORD IN CASTLE MOVE 
    if (startType == 'K' || startType == 'k'){
        updateKingCoord(end, turn);
        updateKingMoved(end);
    }

    // check if the piece that moved is a pawn
    // ISSUE: SHOULD THIS CHANGE IF THERE'S AN ENPASSANT 
    if (startType == 'P' || startType == 'p'){
        updatePawnMoved(start, end);
    }

    // store the possibleMoves 
    vector<Vec> possibleMoves; 

    // reset all the possible moves for the pieces 
    // White (true) means turn (false)
    // Black (false) means turn (true)
    for (vector<shared_ptr<Piece>> vec : gb){
		for (shared_ptr<Piece> p : vec){
            if (p->getTeam() != turn || p->getType() == ' ' || p->getType() == '_'){ continue; }
            p->resetMoves(); // clear all the legal moves
            p->getPossibleMoves(gb); // get the possible moves for this piece

            for (Vec pMoves : p->returnPossibleMoves()) {
                possibleMoves.push_back(pMoves);
            }
            // @franklin611 add a for loop to append all possible moves to possibleMoves (489)
            
        }
    }

    // ISSUE: SWITCH TO ISVALIDCHECK
    // This checks the possible moves of the turn pieces 
    bool check = IsvalidCheck(possibleMoves, turn); //Returns true or false based on whose turn it is
    bool legal = false;
    bool team = false; 
    if (getPiece(end)->getType() >= 'A' && getPiece(end)->getType() <= 'Z'){ team = true; }

    if (!check){ legal = true; }
 

    // we decide its legal meaning the move does not put itself in check so we notify the players of the moves 
    if (!check && notify){
        if (team){ 
            playerWhite->notifyLM(start, end); 
        } 
        else { 
            playerBlack->notifyLM(start, end); 
        }
        // Assume move doesnt put us in check, now we check what type of move it is
        isCaptureMove(start, end, boardCopy);
        isCheckMove(start, end);
        isCheckMateMove(start, end);
        isAvoidCaptureMove(start, end, boardCopy);
    }


    // revert back to the original board 
    for (size_t row = 0; row < gb.size(); ++row) {
        for (size_t col = 0; col < gb[row].size(); ++col) {
            gb[row][col] = move(boardCopy[row][col]);
        }
    }

    // revert the king's coordinates
    if (!turn){ 
        swap(wKingCoord, wKing);
    } else {
        swap(bKingCoord, bKing);
    }

    // If it puts us in check, legal stays false
    return legal;
}

// determines if a move should be categorized as a checkmate move and notifies the player
void ChessBoard::isCheckMateMove(Vec start, Vec end){
    bool empty = true;

    // determines the possible moves of the opposite team 
    // ISSUE: THIS IS BASED ON POSSIBLE NOT LEGAL MOVES -> UPDATE 
    for (vector<shared_ptr<Piece>> vec : gb){
        for (shared_ptr<Piece> p : vec){
            if (p->getTeam() != turn){ continue;}
            p->resetMoves();
            p->getPossibleMoves(gb);
            if (p->returnPossibleMoves().size() != 0){ empty = false; }
        }
    }

// TODO
    // if the opposite team as no possible moves and they are in check then that is a checkmate move 
    if (isCheck(!turn) && empty){
        if (!turn){ playerWhite->notifyCMM(start, end); } 
        else { playerBlack->notifyCMM(start, end); }
    }
}

//  determine if a move will put the other team in check 
void ChessBoard::isCheckMove(Vec start, Vec end){
    // ISSUE: BASED ON POSSIBLE NOT LEGAL MOVES 

    // If we reached here, assume that the move we want to make doesn't put us in check
    // We check if it puts the enemy in check!
    // We do !turn, cuz we want to see that the move we simulated in testMove
    // Puts the enemy king in check
    
    if (isCheck(!turn)){
        if (!turn){ playerWhite->notifyCheckM(start, end); }
        else { playerBlack->notifyCheckM(start, end); }
    }
}

// determine if this move will capture any pieces on the opposing team
void ChessBoard::isCaptureMove(Vec start, Vec end, vector<vector<shared_ptr<Piece>>> ob){
    // if there is a piece at the end coordinate then it is a capture move
    if (isThere(end, turn, ob)){
        if (!turn){ playerWhite->notifyCapM(start, end); } // if the next turn (opponent is white)
        else { playerBlack->notifyCapM(start, end); }
    }
}

// categorizes a move as an avoid capture move 
void ChessBoard::isAvoidCaptureMove(Vec start, Vec end, vector<vector<shared_ptr<Piece>>> ob){
    // assume it is not in a position to be captured
    bool capture = false;
    // was the piece (on the opponent's team) in a position to be captured in the first place in the old board
    // ISSUE: THIS IS BASED ON POSSIBLE AND NOT LEGAL MOVES 
    // look at the current team's legal moves if they are equal to the piece's start
    for (vector<shared_ptr<Piece>> vec : ob){
        for (shared_ptr<Piece> p : vec){
            // Example a7 b7 
            // If it is our team, we dont care for them
            if (p->getTeam() != turn || p->getType() == ' ' || p->getType() == '_'){ continue; }
            // cout << p->getType() << endl;
            // cout << "Kings (sepcific example) Team: " << p->getTeam() << endl;
            // cout << "Start: " << start << endl;
            // cout << "End : " << end << endl;
            // It hit b6 b7, so it determined that it was a move
            p->resetMoves();
            p->getPossibleMoves(ob);
            for (Vec move: p->returnPossibleMoves()){
                // cout << "Start position of piece we want to determine the avoid move of : " << start << endl;
                // cout << "Move by " << p->getType() <<  move <<  endl;
                if (move == start){
                // cout << "Piece doing move " << p->getType() << endl;
                capture = true; break;}
            }
        }
    }

    if (!capture){ return; }
    capture  = false;

    // will the move take the piece out of a position to be captured
    // consider the board after the move was made, will any of the other team's pieces capture mine (which is at end)
    for (vector<shared_ptr<Piece>> vec: gb){
        for (shared_ptr<Piece> p : vec){
            if (p->getTeam() == turn){ continue; } // We should be checking current team's turn?
            p->resetMoves();
            p->getPossibleMoves(gb);
            for (Vec move: p->returnPossibleMoves()){
                if (move == end){ capture = true; break;}
            }
        }
    }

    if (!capture){
        if (!turn){ playerWhite->notifyACM(start, end); }
        else { playerBlack->notifyACM(start, end); }
    }
}


// gets the current turn of the board
bool ChessBoard::getTurn(){
    return turn;
}

// sets up a piece at coordinate 
void ChessBoard::setupWithPiece(shared_ptr<Piece> p, Vec coordinate) {
    int row = coordinate.getY();
    int col = coordinate.getX();
    gb[row][col] = p;
}

// returns a copy of an empty piece from an empty board 
shared_ptr<Empty> ChessBoard::getEmptyPiece(Vec coord){
    int row = coord.getY();
    int col = coord.getX();
    shared_ptr<Empty> emptyPiece = make_shared<Empty>(Vec{col, row}, eb[row][col]->getType(), eb[row][col]->getTeam()); 
    return emptyPiece;
}

// updates the score if someone forfeits
void ChessBoard::forfeit(){
    displayScore = true;
    if (!turn){
        // update score +1 for black
        game.updateBlack(false);
    } else {
        // update score +1 for white
        game.updateWhite(false);
    }
}


// restarts a game by resetting the game 
void ChessBoard::restartGame() {
    for(size_t i = 0; i < eb.size(); ++i) { //The row
        for (size_t j = 0; j < eb[i].size(); ++j) { // The column
        // Remmber, we have a vector<vector<>>>>
            // COPY CTOR DIDNT WORK 
            // gb[j][i] = make_shared<Piece>(*(eb[i][j]));
            // cout << "Type Before : " <<  gb[i][j]->getType() << endl;

            td->notify(gb[i][j]->getCoordinate(), eb[i][j]->getType()); // Fixed to now notify text display of the change
            
            gb[i][j] = eb[i][j]->clone();
        }
        cout << endl;
    }
    turn = false; 
    bCheck = false;
    wCheck = false;
    displayScore = false;
}

// sees if a move is considered an upgrade pawn move 
bool ChessBoard::upgradePawn(Vec end){
    char type = getType(end);
    if (type == 'P' && getPiece(end)->getCoordinate().getY() == 7) {
        return true;
    } else if (type == 'p' && getPiece(end)->getCoordinate().getY() == 0) {
        return true;
    }
    return false;
}


// sets the white king coordinate
void ChessBoard::setWhiteKing(Vec coordinate){
    wKing = coordinate;

}

// sets the black king coordinate 
void ChessBoard::setBlackKing(Vec coordinate){
    bKing = coordinate;

}

// sets up a piece at coordinate with type 
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
    } else if (type == ' ' || type == '_') { 
        gb[row][col] = getEmptyPiece(coordinate);
    } 

    // ISSUE: ADD GRAPHICS DISPLAY
    td->notify(coordinate, type);
    // gd->notify(coordinate, type);
}

// if the user does not call setup this function is used to create the default board 
void ChessBoard::defaultBoard() {

    // First setup pawns
    for (int i = 0; i < 8; ++i) {
        setupWithChar('P', Vec{i, 1}); // White 
        setupWithChar('p', Vec{i, 6}); // Black
    }

    // Setup Rooks
    setupWithChar('R', Vec{0,0});
    setupWithChar('R', Vec{7, 0}); // Whites
    setupWithChar('r', Vec{0,7});
    setupWithChar('r', Vec{7, 7}); // Black

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
    setupWithChar('K', Vec{3,0}); // White King
    setupWithChar('k', Vec{3, 7}); // Black King
    setWhiteKing(Vec{3,0});
    setBlackKing(Vec{3, 7});

    // Setup Queens
    setupWithChar('Q', Vec{4,0}); // White Queen
    setupWithChar('q', Vec{4, 7}); // Black Queen

    // set the legal moves for the pieces of the current turn 
    for (vector<shared_ptr<Piece>> vec : gb) {
		for (shared_ptr<Piece> p : vec) {
            if(p->getTeam() != turn) {
                p->getPossibleMoves(gb); 
                Vec v = p->getCoordinate();
                vector<Vec> moves = p->returnPossibleMoves();
                for (Vec end : moves) { 
                    testMove(v, end, true);
                }
            } 
        }
    }

    // notify the display of who's turn it is 
    // ISSUE: ADD GRAPHICS DISPLAY 
    td->notify(turn);
}

// setup turn during setup and notify the display 
void ChessBoard::setupTurn(bool turn) {
    this->turn = turn;
    td->notify(turn);
}

// used to skip a turn 
void ChessBoard::setTurn(bool turn) {
    this->turn = turn;

    td->notify(turn);
    bool isEnd = true;
    // set up the turn's moves 
    for (vector<shared_ptr<Piece>> vec : gb){
		for (shared_ptr<Piece> p : vec){
            if (p->getTeam() == turn) { continue; }
            p->resetMoves(); // clear all the legal moves
            p->getPossibleMoves(gb); // get the possible moves for this piece
            // test every possible move -> which will add it to the legal moves if it passes
            Vec v = p->getCoordinate();
            vector<Vec> moves = p->returnPossibleMoves();
            for (Vec move : moves){
                if (testMove(v, move, true)){ isEnd = false; };
            }
        }
    }

    if (isEnd) { endGame(); displayScore = true;  }
}

// returns the playerWhite
shared_ptr<Observer> ChessBoard::getPlayerWhite() {
    return playerWhite;
}

// returns the playerBlack 
shared_ptr<Observer> ChessBoard::getPlayerBlack() {
    return playerBlack;
}

// changes the display score boolean
void ChessBoard::setDisplayScore(bool b) {
    displayScore = b;
}

// chessboard output operator 
ostream& operator<<(ostream& out, const ChessBoard& cb) {
    out << *(cb.td) << endl;
    if (cb.displayScore) {
        out << "GAME ENDED! SCORE: " << endl;
        out << cb.game << "\n" << endl;
    }
    return out;
}


