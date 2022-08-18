#ifndef POS_H_
#define POS_H_

#include <string>
#include <vector>

#include "chess_defs.h"

using ChessPos = char;

ChessPos new_pos(char file, char rank);
ChessPos new_pos(std::string str);

inline bool pos_in_bounds(ChessPos pos){
    return pos >= 0 && pos < 64;
}
inline char pos_rank(ChessPos pos){
    return pos >> 3;
}
inline char pos_file(ChessPos pos) {
    return pos & 0b111;
}
inline char pos_rank_char(ChessPos pos){
    return pos_rank(pos) + '1';
}
inline char pos_file_char(ChessPos pos){
    return pos_file(pos) + 'a';
}

std::string pos_str(ChessPos pos);

// struct ChessPos{
//     char pos;

//     ChessPos();
//     ChessPos(char pos);
//     ChessPos(char file, char rank);
//     ChessPos(std::string str);
//     ChessPos(const ChessPos &pos);

//     bool inBounds() const;
//     char rankChar() const;
//     char fileChar() const;
//     char rank() const;
//     char file() const;
//     std::string str() const;

//     bool operator==(const ChessPos &other) const;
//     bool operator!=(const ChessPos &other) const;
// };

#endif