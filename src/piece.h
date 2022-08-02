#ifndef PIECE_H_
#define PIECE_H_

#include "chess_defs.h"

using namespace chess_defs;

struct ChessPiece {

    PieceType pieceType;
    bool captured;
    Player player;

    ChessPiece(PieceType pieceType = kPiecePawn, Player player = kPlayerWhite);

    char pieceChar();

    void printPiece();
};

#endif