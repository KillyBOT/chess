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
    bool isCastling : 1;
    bool isPromoting: 1;
    bool isEnPassant: 1;
    bool isEnPassantEligible: 1;
    bool castlingSide : 1; //False for queenside, true for kingside
    
    ChessMove(ChessPiece piece = ChessPiece(), ChessPos pos = ChessPos(), ChessPos newPos = ChessPos());
    ChessMove(const ChessMove &move);

    bool isInBounds() const;
    std::string basicStr() const;
    std::string str() const;
};

#endif