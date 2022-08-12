#ifndef MOVE_GENERATOR_
#define MOVE_GENERATOR_

#include <vector>

#include "pos.h"
#include "move.h"
#include "board.h"

using std::vector;
using std::unordered_set;

class MoveGenerator {

    Player player_, opponent_;
    int pieceNum_;
    int movesSinceLastCapture_;
    Byte enPassantFile_;
    ChessPiece *pieces_;
    ChessPos *piecePositions_;
    ChessPos kingPos_;

    bool attacked_[64];
    bool pinned_[64];

    bool willMoveCapture(ChessMove &move) const;
    bool enPassantCheck(ChessMove &move) const;

    void setAttackedForPiece(ChessPiece piece, ChessPos pos);

    vector<ChessMove> genMovesForPiece(ChessPiece piece, ChessPos pos) const;

    void setPinned();
    void setAttacked();

    public:

    MoveGenerator();

    bool stalemate() const;
    bool stalemate(ChessBoard &board);

    bool inCheck() const;
    bool inCheck(ChessBoard &board);

    bool hasLost() const;
    bool hasLost(ChessBoard &board);

    vector<ChessMove> getMoves();
    vector<ChessMove> getMoves(ChessBoard &board);

    void setBoard(ChessBoard &board);
    void printAttacked();

};

#endif