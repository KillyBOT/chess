#ifndef CHESS_DEFS_H_
#define CHESS_DEFS_H_

#include <string>
#include <array>

using Byte = unsigned char;
using Player = Byte;
using PieceType = Byte;

namespace chess_defs{

    const Player kPlayerWhite = 0;
    const Player kPlayerBlack = 1;

    const PieceType kPieceNone = 0;
    const PieceType kPiecePawn = 1;
    const PieceType kPieceRook = 2;
    const PieceType kPieceKnight = 3;
    const PieceType kPieceBishop = 4;
    const PieceType kPieceQueen = 5;
    const PieceType kPieceKing = 6;

    const std::array<int, 7> kPieceValue = {0, 100, 500, 300, 320, 900, 0};

    inline Player player_opponent(Player player){
        return !player;
        //return player == kPlayerWhite ? kPlayerBlack : kPlayerWhite;
    }
}

#endif