#include <string>

#include "pos.h"

using std::string;

ChessPos::ChessPos(char col, char row){
    this->col = col;
    this->row = row;
}

ChessPos::ChessPos(const ChessPos &pos){
    this->col = pos.col;
    this->row = pos.row;
}

bool ChessPos::operator==(const ChessPos &other) const {
    return (this->row == other.row && this->col == other.col);
}

bool ChessPos::isInBounds() const {
    return this->col >= 'a' && this->col <= 'h' && this->row >= 1 && this->row <= 8;
}

int ChessPos::asInt() const {
    return (int)(this->col - 'a') * 8 + (int)(this->row - 1);
}

string ChessPos::str() const{
    if(!this->isInBounds()) return "Pos out of bounds!";
    string str;
    str += this->col;
    str += this->row  + '0';
    return str;
}

string ChessPos::str_int() const{
    string str;
    str += std::to_string((int)(this->col) - 'a');
    str += ' ';
    str += std::to_string((int)(this->row));
    return str;
}

std::size_t ChessPosHash::operator() (const ChessPos &pos) const {
    // std::size_t hash = pos.col;
    // hash <<= 8;
    // hash |= pos.row;
    // return hash;
    return ((pos.row << 8) | pos.col) << 8;
}

std::vector<ChessPos> positions_in_ray(ChessPos start, char dCol, char dRow){
    std::vector<ChessPos> positions;

    start.col += dCol;
    start.row += dRow;

    while(start.isInBounds()){
        positions.push_back(start);
        start.col += dCol;
        start.row += dRow;
    }

    return positions;
}