#ifndef CHESS_DEFS_H_
#define CHESS_DEFS_H_

#include <string>
#include <array>

using Byte = unsigned char;

namespace chess_defs{

    enum Player {
        kPlayerWhite,
        kPlayerBlack,
        kPlayerNone
    };

    enum PieceType {
        kPiecePawn,
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

    const std::array<int, 7> kPieceValue = {1, 5, 3, 3, 9, 0, 0};
}

#endif