#ifndef POS_H_
#define POS_H_

#include <string>
#include <vector>

#include "chess_defs.h"

struct ChessPos{
    char pos;

    ChessPos(char pos = -1);
    ChessPos(char file, char rank);
    ChessPos(std::string str);
    ChessPos(const ChessPos &pos);

    bool inBounds() const;
    char rank() const;
    char file() const;
    std::string str() const;

    bool operator==(const ChessPos &other) const;
    bool operator!=(const ChessPos &other) const;
};

#endif