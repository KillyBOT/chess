#ifndef PIECE_H_
#define PIECE_H_

#include <unordered_set>

#include "chess_defs.h"
#include "pos.h"

using namespace chess_defs;

struct ChessPiece {

    PieceType pieceType;
    Player player;
    int moveNum;

    ChessPiece(PieceType pieceType = kPieceNone, Player player = kPlayerNone);
    ChessPiece(const ChessPiece &piece);
    bool operator==(const ChessPiece &other) const;

    char pieceChar(bool usePlayer = true) const;

    void printPiece();
};

#endif