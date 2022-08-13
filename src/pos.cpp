#include <vector>
#include <iostream>
#include <string>

#include "pos.h"

ChessPos::ChessPos(){
    this->pos = -1;
}
ChessPos::ChessPos(char pos){
    this->pos = pos;
}
ChessPos::ChessPos(char file, char rank) {
    this->pos = (rank - '1');
    this->pos <<= 3;
    this->pos |= (file - 'a');
}
//WARNING: don't put in random strings, cuz it's gonna break
ChessPos::ChessPos(std::string str) {
    this->pos = (str[1] - '1');
    this->pos <<= 3;
    this->pos |= (str[0] - 'a');
}

ChessPos::ChessPos(const ChessPos &pos) {
    this->pos = pos.pos;
}

bool ChessPos::inBounds() const {
    return pos >= 0 && pos < 64;
}
char ChessPos::rank() const {
    return (char)((this->pos >> 3) + '1');
}
char ChessPos::file() const {
    return (char)((this->pos & 0b111) + 'a');
}
std::string ChessPos::str() const {
    std::string str;
    str += this->file();
    str += this->rank();
    return str;
}

bool ChessPos::operator==(const ChessPos &other) const {
    return this->pos == other.pos;
}

bool ChessPos::operator!=(const ChessPos &other) const {
    return this->pos != other.pos;
}