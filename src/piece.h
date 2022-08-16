#ifndef PIECE_H_
#define PIECE_H_

#include <unordered_set>

#include "chess_defs.h"
#include "pos.h"

using namespace chess_defs;

using ChessPiece = char;

inline PieceType piece_type(ChessPiece piece) {
    return piece & 0b111;
}
inline Player piece_player(ChessPiece piece) {
    return piece >> 3;
}

ChessPiece new_piece(PieceType type, Player player);
char piece_char(ChessPiece piece, bool incPlayer = true);

// struct ChessPiece {

//     Byte data;
//     bool hasMoved;

//     ChessPiece(Byte data = 0);
//     ChessPiece(PieceType type, Player player);
//     ChessPiece(const ChessPiece &piece);

//     bool operator==(const ChessPiece &piece) const;

//     inline PieceType type() const {
//         return (this->data & 0b111);
//     }
//     inline Player player() const {
//         return this->data >> 3;
//     }

//     char pieceChar(bool usePlayer = true) const;
// };

#endif