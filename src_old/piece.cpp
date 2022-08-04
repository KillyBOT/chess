#include <iostream>

#include "piece.h"

using namespace std;

ChessPiece::ChessPiece(PieceType pieceType, Player player){
    this->pieceType = pieceType;
    this->player = player;
    this->hasMoved = false;
}
ChessPiece::ChessPiece(const ChessPiece &piece){
    this->pieceType = piece.pieceType;
    this->player = piece.player;
}
bool ChessPiece::operator==(const ChessPiece &other) const{
    return this->pieceType == other.pieceType && this->player == other.player;
}
char ChessPiece::pieceChar() const{

    char c;

    switch(this->pieceType){
        case kPiecePawn:
        c = 'P';
        break;
        case kPieceRook:
        c = 'R';
        break;
        case kPieceKnight:
        c = 'N';
        break;
        case kPieceBishop:
        c = 'B';
        break;
        case kPieceQueen:
        c = 'Q';
        break;
        case kPieceKing:
        c = 'K';
        break;
        case kPieceNone:
        default:
        c = 'X';
        break;
    }

    if(this->player == kPlayerBlack){
        c += 32; //Go from uppercase to lowercase
    }

    return c;
}