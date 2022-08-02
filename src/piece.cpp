#include <iostream>

#include "piece.h"

using namespace std;

ChessPiece::ChessPiece(PieceType pieceType, Player player){
    this->pieceType = pieceType;
    this->player = player;
    this->captured = false;
}
char ChessPiece::pieceChar(){

    char c = 0;
    if(this->player == kPlayerBlack){
        c += 32; //Go from uppercase to lowercase
    }

    switch(this->pieceType){
        case kPiecePawn:
        default:
        c += 'P';
        break;
        case kPieceRook:
        c += 'R';
        break;
        case kPieceKnight:
        c += 'N';
        break;
        case kPieceBishop:
        c += 'B';
        break;
        case kPieceQueen:
        c += 'Q';
        break;
        case kPieceKing:
        c += 'K';
        break;
    }

    return c;
}