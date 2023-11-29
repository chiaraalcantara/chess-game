#include "King.h"
#include "Player.h"
#include "Piece.h"

King::King() : Piece{}, moved{false} {}

void King::hasMoved(){
    moved = true;
}

void::getMOved() {
    return moved;
}

void King::getPossibleMoves(vector<vector<shared_ptr<Piece>>> gb) {
    Vec moveUp = Vec{coordinate.getX(), coordinate.getY() - 1};
    Vec moveLeft = Vec{coordinate.getX() - 1, coordinate.getY()};
    Vec moveRight = Vec{coordinate.getX() + 1, coordinate.getY()};
    Vec moveDown = Vec{coordinate.getX(), coordinate.getY() + 1};

    Vec castleLeft = Vec{coordinate.getX() - 2, coordinate.getY()};
    Vec castleRight = Vec{coordinate.getX() + 2, coordinate.getY()};

    // Verify that left castle move is possible
    // Not moved means we are at the base position of such a king.

    // Consider two cases of "Left" for White and Black
    // Check image in Discord
    if (!moved && !(gb[coordinate.getY()][coordinate.getX()]->getMoved()) && isEmptyPiece(Vec{coordinate.getX() - 1, coordinate.getY()} && isEmptyPiece({coordinate.getX() - 2, coordinate.getY()}))) {
        possibleMoves.push_back(castleLeft);
    }

    if (!moved && !(gb[coordinate.getY()][coordinate.getX()]->getMoved()) && isEmptyPiece(Vec{coordinate.getX() + 1, coordinate.getY()} && isEmptyPiece({coordinate.getX() + 2, coordinate.getY()}))) {
        possibleMoves.push_back(castleRight);
    }

    shared_ptr<Piece> p = pieceAt(moveUp);
    if (inBounds(moveUp) && (isEmptyPiece(p) || (p->getType() != getType()))) {
        possibleMoves.push_back(moveUp);
    }
    p = pieceAt(moveLeft);
    if (inBounds(moveLeft) && (isEmptyPiece(p) || (p->getType() != getType()))) {
        possibleMoves.push_back(moveLeft);
    }
    p = pieceAt(moveRight);
    if (inBounds(moveRight) && (isEmptyPiece(p) || (p->getType() != getType()))) {
        possibleMoves.push_back(moveRight);
    }
    p = pieceAt(moveDown);
    if (inBounds(moveDown) && (isEmptyPiece(p) || (p->getType() != getType()))) {
        possibleMoves.push_back(moveDown);
    }
    // Left Castle
    // THis is how a castle movehappens
    // 1. 
    // 2. 
    // 3. castleMove(start,end).THis is only called when the move (start,end) is from a King
    //      I just have to pass the start and end move. It willrecognize if it is a  + 2move by a King
    
}

// I need to check the logic for a castle

