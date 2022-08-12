#ifndef PIECE_H_
#define PIECE_H_

#include <unordered_set>

#include "chess_defs.h"
#include "pos.h"

using namespace chess_defs;

struct ChessPiece {

    Byte data;

    ChessPiece(Byte data = 0);
    ChessPiece(PieceType type, Player player);
    ChessPiece(const ChessPiece &piece);

    inline PieceType type() const {
        return (this->data & 0b111);
    }
    inline Player player() const {
        return (this->data >> 3) & 1;
    }
    inline bool hasMoved() const {
        return (this->data >> 4) & 1;
    }

    char pieceChar(bool usePlayer = true) const;
};

#endif