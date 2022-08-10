#ifndef POS_H_
#define POS_H_

#include <string>
#include <vector>

struct ChessPos{
    char row, col;

    ChessPos(const ChessPos &pos);
    ChessPos(char col = 'a', char row = 0);

    bool isInBounds() const;
    int asInt() const;
    std::string str() const;
    std::string str_int() const;

    bool operator==(const ChessPos &other) const;
};

struct ChessPosHash{
    std::size_t operator() (const ChessPos &pos) const;
};

std::vector<ChessPos> positions_in_ray(ChessPos start, char dCol, char dRow);

#endif