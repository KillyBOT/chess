#include <iostream>

#include "move.h"

using std::string;

ChessPos::ChessPos(char col, char row){
    this->col = col;
    this->row = row;
}

bool ChessPos::operator==(const ChessPos &other) const {
    return (this->row == other.row && this->col == other.col);
}

bool ChessPos::isInBounds(){
    return this->col >= 'a' && this->col <= 'h' && this->row >= 1 && this->row <= 8;
}

string ChessPos::str(){
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

ChessMove::ChessMove(ChessPos pos, ChessPos newPos){
    this->pos = pos;
    this->newPos = newPos;
}

bool ChessMove::isInBounds(){
    return (this->pos.isInBounds() && this->newPos.isInBounds());
}