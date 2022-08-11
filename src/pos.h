#ifndef POS_H_
#define POS_H_

#include <string>
#include <vector>

struct ChessPos{
    int pos;

    ChessPos(int pos = 0);
    ChessPos(std::string str);
    ChessPos(const ChessPos &pos);

    bool inBounds() const;
    char rank() const;
    char file() const;
    std::string str() const;

    bool operator==(const ChessPos &other) const;
};

std::vector<ChessPos> positions_in_ray(ChessPos &start, int dCol, int dRow);

#endif