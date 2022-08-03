#ifndef CHESS_DEFS_H_
#define CHESS_DEFS_H_

#include <string>
#include <array>

using Byte = unsigned char;

namespace chess_defs{

    enum Player {
        kPlayerWhite,
        kPlayerBlack
    };

    enum PieceType {
        kPiecePawn,
        kPieceRook,
        kPieceKnight,
        kPieceBishop,
        kPieceQueen,
        kPieceKing
    };

    const std::array<int, 6> kPieceValue = {1, 5, 3, 3, 9, 0};
}

#endif