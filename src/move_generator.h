#ifndef MOVE_GENERATOR_
#define MOVE_GENERATOR_

#include <vector>

#include "pos.h"
#include "move.h"
#include "board.h"

using std::vector;
using std::unordered_set;

class MoveGenerator {

    Player player_;
    Player opponent_;
    const ChessBoard *board_;
    bool doEnPassantCheck_;

    bool attacked_[64];
    bool pinned_[64][9];
    bool forced_[65];

    bool willMoveCapture(ChessMove &move) const;
    bool enPassantCheck(ChessMove &move) const;

    void addPiecesInDir(vector<ChessPos> &positions, ChessPos start, int dir) const;

    void setAttackedForPiece(ChessPiece piece, ChessPos pos);
    void genMovesForPiece(vector<ChessMove> &moves, ChessPiece piece, ChessPos start) const;

    void setPinned();
    void setAttacked();
    void setForced();

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