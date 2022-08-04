#include <iostream>
#include <vector>
#include <string>

#include "move.h"

using std::string;

ChessPos::ChessPos(char col, char row){
    this->col = col;
    this->row = row;
}

bool ChessPos::operator==(const ChessPos &other) const {
    return (this->row == other.row && this->col == other.col);
}

bool ChessPos::isInBounds() const{
    return this->col >= 'a' && this->col <= 'h' && this->row >= 1 && this->row <= 8;
}

string ChessPos::str() const{
    string str;
    str += this->col;
    str += this->row  + '0';
    return str;
}

std::size_t ChessPosHash::operator() (const ChessPos &pos) const {
    std::size_t hash = pos.col;
    hash <<= 8;
    hash |= pos.row;
    return hash;
}

ChessMove::ChessMove(ChessPiece piece, ChessPos pos, ChessPos newPos){
    this->pos = pos;
    this->newPos = newPos;
    this->piece = piece;
    this->capture = ChessPiece();
    this->isCastling = false;
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