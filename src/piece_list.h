#ifndef PIECE_LIST_H_
#define PIECE_LIST_H_

#include "pos.h"
#include "bitboard.h"

class ChessPieceList {

    ChessPos positions_[64];
    char map_[64];
    int pieceNum_;
    BitBoard bb_;

    public:

    ChessPieceList();

    inline int size() const {
        return this->pieceNum_;
    }
    inline BitBoard mask() const {
        return this->bb_;
    }
    inline const ChessPos &operator[](char ind) const {
        return this->positions_[ind];
    }

    void addPiece(ChessPos pos);
    void removePiece(ChessPos pos);
    void movePiece(ChessPos oldPos, ChessPos newPos);

    void printList() const;
};

#endif