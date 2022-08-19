#include "piece.h"

ChessPiece new_piece(PieceType type, Player player) {
    ChessPiece piece = type;
    piece |= player << 3;
    return piece;
}
char piece_char(ChessPiece piece, bool incPlayer) {
    char c;
    switch(piece_type(piece)) {
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
    if(incPlayer && piece_player(piece) == kPlayerBlack) c += 32;

    return c;
}