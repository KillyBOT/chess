#include <string>
#include <iostream>

#include "move.h"

ChessMove::ChessMove(){
    this->piece = ChessPiece();
    this->captured = ChessPiece();
    this->oldPos = ChessPos();
    this->newPos = ChessPos();
    this->moveData = kMoveNone;
}
ChessMove::ChessMove(ChessPiece piece, ChessPos oldPos, ChessPos newPos){
    this->piece = piece;
    this->captured = ChessPiece();
    this->oldPos = oldPos;
    this->newPos = newPos;
    this->moveData = kMoveNone;
}
ChessMove::ChessMove(const ChessMove &move) {
    this->piece = move.piece;
    this->captured = move.captured;
    this->oldPos = move.oldPos;
    this->newPos = move.newPos;
    this->moveData = move.moveData;
}

PieceType ChessMove::promotionType() const {
    switch(this->moveData){
        case kMovePromotingToQueen:
        return kPieceQueen;
        case kMovePromotingToRook:
        return kPieceRook;
        case kMovePromotingToKnight:
        return kPieceKnight;
        case kMovePromotingToBishop:
        return kPieceBishop;
    }
    return kPieceNone;
}
bool ChessMove::isValid() const {
    return this->newPos.inBounds() && this->oldPos.pos != this->newPos.pos;
}
std::string ChessMove::str() const {
    std::string str;
    if(this->moveData == kMoveIsCastling){
        if(this->isCastlingKingside()) return "0-0";
        else return "0-0-0";
    }

    if(this->piece.type() != kPiecePawn) str += this->piece.pieceChar();
    if(this->captured.type() != kPieceNone) str += 'x';
    str += this->newPos.str();

    if(this->promotionType()){
        str += '=';
        str += ChessPiece(this->promotionType(), this->piece.player()).pieceChar();
    }

    return str;
}
std::string ChessMove::strUCI() const {
    std::string str;
    str += this->oldPos.str();
    str += this->newPos.str();
    if(this->isPromoting()){
        str += ChessPiece(this->promotionType(), kPlayerBlack).pieceChar();
    }

    return str;
}
bool ChessMove::isCastlingKingside() const {
    return this->newPos.fileChar() == 'g';
}