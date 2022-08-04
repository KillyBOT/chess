#ifndef MOVE_H_
#define MOVE_H_

#include <string>
#include <vector>

#include "chess_defs.h"
#include "piece.h"

struct ChessPos{
    char col, row;

    ChessPos(char col = 'a', char row = 1);

    bool isInBounds() const;
    std::string str() const;

    bool operator==(const ChessPos &other) const;
};

struct ChessPosHash{
    std::size_t operator() (const ChessPos &pos) const;
};

struct ChessMove{
    ChessPos pos;
    ChessPos newPos;
    ChessPiece piece, capture;
    bool isCastling;
    
    ChessMove(ChessPiece piece = ChessPiece(), ChessPos pos = ChessPos(), ChessPos newPos = ChessPos());

    bool isInBounds() const;
    std::string basicStr() const;
};

#endif