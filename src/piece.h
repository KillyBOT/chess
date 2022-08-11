#ifndef PIECE_H_
#define PIECE_H_

#include <unordered_set>

#include "chess_defs.h"
#include "pos.h"

using namespace chess_defs;

struct ChessPiece {

    PieceType type;
    Player player;
    bool hasMoved;
    bool justMoved;

    ChessPiece(PieceType type = kPieceNone, Player player = kPlayerWhite);
    ChessPiece(const ChessPiece &piece);

    char pieceChar(bool usePlayer = true) const;
};

#endif