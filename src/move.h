#ifndef MOVE_H_
#define MOVE_H_

#include <string>
#include <vector>

#include "chess_defs.h"
#include "pos.h"
#include "piece.h"

const Byte kMoveFlagNone = 0;
const Byte kMoveFlagIsCastling = 1;
const Byte kMoveFlagEnPassant = 2;
const Byte kMoveFlagEnPassantEligible = 3;
const Byte kMoveFlagPromotingToQueen = 4;
const Byte kMoveFlagPromotingToRook = 5;
const Byte kMoveFlagPromotingToKnight = 6;
const Byte kMoveFlagPromotingToBishop = 7;

struct ChessMove {

    ChessPos oldPos;
    ChessPos newPos;
    ChessPiece piece;
    ChessPiece captured;
    Byte moveData;
    
    ChessMove();
    ChessMove(ChessPiece piece, ChessPos oldPos, ChessPos newPos);
    //ChessMove(const ChessMove &move);

    inline bool isCapturing() const {
        return this->captured;
    }
    inline bool isPromoting() const {
        return this->moveData >= kMoveFlagPromotingToQueen;
    }
    inline bool isQuiet() const {
        //TODO: Make this more in depth
        return !(this->isCapturing() || this->isPromoting());
    }

    PieceType promotionType() const;
    bool isValid() const;
    bool isCastlingKingside() const;
    std::string str() const;
    std::string strUCI() const;

};

#endif