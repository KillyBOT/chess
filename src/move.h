#ifndef MOVE_H_
#define MOVE_H_

#include <string>
#include <vector>

#include "chess_defs.h"
#include "pos.h"
#include "piece.h"

const Byte kMoveNone = 0;
const Byte kMoveIsCastling = 1;
const Byte kMovePromotingToQueen = 2;
const Byte kMovePromotingToRook = 3;
const Byte kMovePromotingToKnight = 4;
const Byte kMovePromotingToBishop = 5;
const Byte kMoveEnPassant = 6;
const Byte kMoveEnPassantEligible = 7;

struct ChessMove {

    ChessPos oldPos;
    ChessPos newPos;
    ChessPiece piece;
    ChessPiece captured;
    Byte moveData;
    
    ChessMove();
    ChessMove(ChessPiece piece, ChessPos oldPos, ChessPos newPos);
    ChessMove(const ChessMove &move);

    inline bool isCapturing() const {
        return this->captured.type() != kPieceNone;
    }
    inline bool isPromoting() const {
        return this->moveData == kMovePromotingToQueen || this->moveData == kMovePromotingToRook || this->moveData == kMovePromotingToKnight || this->moveData == kMovePromotingToBishop;
    }

    PieceType promotionType() const;
    bool isValid() const;
    bool isCastlingKingside() const;
    std::string str() const;
    std::string strUCI() const;

};

#endif