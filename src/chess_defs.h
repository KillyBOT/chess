#ifndef CHESS_DEFS_H_
#define CHESS_DEFS_H_

#include <string>

using Byte = unsigned char;

namespace chess_defs{

    enum Player {
        kPlayerWhite = false,
        kPlayerBlack = true
    };

    enum PieceType {
        kPiecePawn,
        kPieceRook,
        kPieceKnight,
        kPieceBishop,
        kPieceQueen,
        kPieceKing
    };
}

#endif