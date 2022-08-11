#include <vector>
#include <iostream>
#include <string>

#include "pos.h"

ChessPos::ChessPos(int pos){
    this->pos = pos;
}
//WARNING: don't put in random strings, cuz it's gonna break
ChessPos::ChessPos(std::string str) {
    this->pos = (str[1] - '1');
    this->pos |= (str[0] - 'a') << 3;
}

ChessPos::ChessPos(const ChessPos &pos) {
    this->pos = pos.pos;
}

bool ChessPos::inBounds() const {
    return pos >= 0 && pos < 64;
}
char ChessPos::rank() const {
    return (char)((this->pos >> 3) + 'a');
}
char ChessPos::file() const {
    return (char)((this->pos & 0b111) + '1');
}
std::string ChessPos::str() const {
    std::string str;
    str += this->file();
    str += this->rank() + '0';
    return str;
}

bool ChessPos::operator==(const ChessPos &other) const {
    return this->pos == other.pos;
}