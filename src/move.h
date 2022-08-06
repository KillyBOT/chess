#ifndef MOVE_H_
#define MOVE_H_

#include <string>
#include <vector>

#include "chess_defs.h"
#include "pos.h"
#include "piece.h"

struct ChessMove{
    ChessPos pos;
    ChessPos newPos;
    ChessPiece piece, capture;
    bool isCastling, isPromoting;
    
    ChessMove(ChessPiece piece = ChessPiece(), ChessPos pos = ChessPos(), ChessPos newPos = ChessPos());

    bool isInBounds() const;
    std::string basicStr() const;
    std::string str() const;
};

#endif