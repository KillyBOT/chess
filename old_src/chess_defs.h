#ifndef CHESS_DEFS_H_
#define CHESS_DEFS_H_

#include <string>
#include <array>

using Byte = unsigned char;

namespace chess_defs{

    enum Player {
        kPlayerWhite = 0,
        kPlayerBlack = 1,
        kPlayerNone
    };

    enum PieceType {
        kPiecePawn=0,
        kPieceRook,
        kPieceKnight,
        kPieceBishop,
        kPieceQueen,
        kPieceKing,
        kPieceNone
    };

    enum BoardState {
        kStateNone,
        kStateWhiteWin,
        kStateBlackWin,
        kStateWhiteInCheck,
        kStateBlackInCheck,
        kStateStalemate
    };

    const std::array<int, 7> kPieceValue = {100, 500, 300, 320, 900, 0};
}

#endif