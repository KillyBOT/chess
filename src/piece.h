#ifndef PIECE_H_
#define PIECE_H_

#include "chess_defs.h"

using namespace chess_defs;

struct ChessPiece {

    PieceType pieceType;
    Player player;

    ChessPiece(PieceType pieceType = kPiecePawn, Player player = kPlayerWhite);
    ChessPiece(const ChessPiece &piece);
    bool operator==(const ChessPiece &other) const;

    char pieceChar();

    void printPiece();
};

#endif