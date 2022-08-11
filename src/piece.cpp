#include "piece.h"

ChessPiece::ChessPiece(PieceType type, Player player) {
    this->type = type;
    this->player = player;
    this->hasMoved = false;
    this->justMoved = false;
}
ChessPiece::ChessPiece(const ChessPiece &piece) {
    this->type = piece.type;
    this->player = piece.player;
    this->hasMoved = piece.hasMoved;
    this->justMoved = piece.justMoved;
}

char ChessPiece::pieceChar(bool usePlayer) const {
    char c;
    switch(this->type) {
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
    if(usePlayer && this->player == kPlayerBlack) c += 32;

    return c;
}