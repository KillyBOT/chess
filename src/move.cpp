#include <string>
#include <iostream>

#include "move.h"

ChessMove::ChessMove(){
    this->piece = 0;
    this->captured = 0;
    this->oldPos = -1;
    this->newPos = -1;
    this->moveData = kMoveFlagNone;
}
ChessMove::ChessMove(ChessPiece piece, ChessPos oldPos, ChessPos newPos){
    this->piece = piece;
    this->captured = 0;
    this->oldPos = oldPos;
    this->newPos = newPos;
    this->moveData = kMoveFlagNone;
}
// ChessMove::ChessMove(const ChessMove &move) {
//     this->piece = move.piece;
//     this->captured = move.captured;
//     this->oldPos = move.oldPos;
//     this->newPos = move.newPos;
//     this->moveData = move.moveData;
// }

PieceType ChessMove::promotionType() const {
    switch(this->moveData){
        case kMoveFlagPromotingToQueen:
        return kPieceQueen;
        case kMoveFlagPromotingToRook:
        return kPieceRook;
        case kMoveFlagPromotingToKnight:
        return kPieceKnight;
        case kMoveFlagPromotingToBishop:
        return kPieceBishop;
    }
    return kPieceNone;
}
bool ChessMove::isValid() const {
    return pos_in_bounds(newPos) && this->oldPos != this->newPos;
}
std::string ChessMove::str() const {
    std::string str;
    if(this->moveData == kMoveFlagIsCastling){
        if(this->isCastlingKingside()) return "0-0";
        else return "0-0-0";
    }

    if(piece_type(this->piece) != kPiecePawn) str += piece_char(this->piece);
    if(piece_type(this->captured) != kPieceNone) str += 'x';
    str += pos_str(this->newPos);

    if(this->promotionType()){
        str += '=';
        str += piece_char(new_piece(this->promotionType(), piece_player(this->piece)));
    }

    return str;
}
std::string ChessMove::strUCI() const {
    std::string str;
    str += pos_str(this->oldPos);
    str += pos_str(this->newPos);
    if(this->isPromoting()){
        str += piece_char(new_piece(this->promotionType(), kPlayerBlack));
    }

    return str;
}
bool ChessMove::isCastlingKingside() const {
    return pos_file_char(this->newPos) == 'g';
}