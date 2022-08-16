#include <vector>
#include <iostream>
#include <string>

#include "pos.h"

ChessPos new_pos(char file, char rank) {
    ChessPos pos = rank - '1';
    pos <<= 3;
    pos |= file - 'a';
    return pos;
}
ChessPos new_pos(std::string str) {
    ChessPos pos = str[1] - '1';
    pos <<= 3;
    pos |= str[0] - 'a';
    return pos;
}

std::string pos_str(ChessPos pos) {
    std::string str;
    str += pos_file_char(pos);
    str += pos_rank_char(pos);
    return str;
}

// ChessPos::ChessPos(){
//     this->pos = -1;
// }
// ChessPos::ChessPos(char pos){
//     this->pos = pos;
// }
// ChessPos::ChessPos(char file, char rank) {
//     this->pos = (rank - '1');
//     this->pos <<= 3;
//     this->pos |= (file - 'a');
// }
// //WARNING: don't put in random strings, cuz it's gonna break
// ChessPos::ChessPos(std::string str) {
//     this->pos = (str[1] - '1');
//     this->pos <<= 3;
//     this->pos |= (str[0] - 'a');
// }

// ChessPos::ChessPos(const ChessPos &pos) {
//     this->pos = pos.pos;
// }

// bool ChessPos::inBounds() const {
//     return pos >= 0 && pos < 64;
// }
// char ChessPos::rankChar() const {
//     return (char)((this->pos >> 3) + '1');
// }
// char ChessPos::fileChar() const {
//     return (char)((this->pos & 0b111) + 'a');
// }
// char ChessPos::rank() const {
//     return this->pos >> 3;
// }
// char ChessPos::file() const {
//     return this->pos & 0b111;
// }
// std::string ChessPos::str() const {
//     std::string str;
//     str += this->fileChar();
//     str += this->rankChar();
//     return str;
// }

// bool ChessPos::operator==(const ChessPos &other) const {
//     return this->pos == other.pos;
// }

// bool ChessPos::operator!=(const ChessPos &other) const {
//     return this->pos != other.pos;
// }