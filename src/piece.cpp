#include "piece.h"

ChessPiece::ChessPiece(Byte data){
    this->data = data;
    this->hasMoved = false;
}
ChessPiece::ChessPiece(PieceType type, Player player) {
    this->hasMoved = false;
    this->data = player;
    this->data <<= 3;
    this->data |= type;
}
ChessPiece::ChessPiece(const ChessPiece &piece) {
    this->data = piece.data;
    this->hasMoved = piece.hasMoved;
}

bool ChessPiece::operator==(const ChessPiece &piece) const {
    return this->data == piece.data && this->hasMoved == piece.hasMoved;
}

char ChessPiece::pieceChar(bool usePlayer) const {
    char c;
    switch(this->type()) {
        case kPieceNone:
        c = 'X';
        break;
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
    }
    if(usePlayer && this->player() == kPlayerBlack) c += 32;

    return c;
}