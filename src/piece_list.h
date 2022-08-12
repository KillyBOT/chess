#ifndef PIECE_LIST_
#define PIECE_LIST_

#include <bitset>

#include "chess_defs.h"
#include "pos.h"

class ChessPieceList {
    int pieceNum_;
    int pieceList_[64];
    std::bitset<64> bitmap_;

    int getPosInPieceList(ChessPos pos);

    public:

    ChessPieceList();

    inline int pieceNum() const {
        return this->pieceNum_;
    }

    inline std::bitset<64> bitmap() const {
        return this->bitmap_;
    }
    inline bool ChessPieceList::pieceAtPos(ChessPos pos) const {
        return pos.inBounds() && this->bitmap_[pos.pos];
    }

    void addPiece(ChessPos pos);
    void removePiece(ChessPos pos);
    void movePiece(ChessPos oldPos, ChessPos newPos);

};

#endif