#include <iostream>
#include <vector>
#include <string>

#include "piece.h"
#include "pos.h"
#include "move.h"

using std::string;

ChessMove::ChessMove(ChessPiece piece, ChessPos pos, ChessPos newPos){
    this->pos = pos;
    this->newPos = newPos;
    this->piece = piece;
    this->capture = ChessPiece();
    this->isCastling = false;
    this->isPromoting = false;
    this->isEnPassant = false;
}

ChessMove::ChessMove(const ChessMove &move){
    this->pos = move.pos;
    this->newPos = move.newPos;
    this->piece = move.piece;
    this->capture = move.capture;
    this->isCastling = move.isCastling;
    this->isPromoting = move.isPromoting;
    this->isEnPassant = move.isEnPassant;
}

//Theoretically, you should never have a situation where the old pos is never out of bounds
bool ChessMove::isInBounds() const {
    return this->newPos.isInBounds();
}

string ChessMove::basicStr() const {
    string str;
    str = this->piece.pieceChar();
    return str + " " + this->pos.str() + " " + this->newPos.str();
}

string ChessMove::str() const {
    string str;
    if(this->piece.pieceType != kPiecePawn && !this->isPromoting) str += this->piece.pieceChar(false);
    if(this->capture.pieceType != kPieceNone) str += 'x';

    str += this->newPos.str();

    if(this->isPromoting){
        str += "=" + this->piece.pieceChar(false);
    }

    if(this->isCastling){
        if(this->newPos.col == 'b') str = "0-0-0";
        else str = "0-0";
    }

    return str;
}