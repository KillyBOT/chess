#ifndef PIECE_H_
#define PIECE_H_

#include "chess_defs.h"

using namespace chess_defs;

struct ChessPiece {

    PieceType pieceType;
    Player player;
    bool hasMoved;

    ChessPiece(PieceType pieceType = kPieceNone, Player player = kPlayerNone);
    ChessPiece(const ChessPiece &piece);
    bool operator==(const ChessPiece &other) const;

    char pieceChar() const;

    void printPiece();
};

#endif