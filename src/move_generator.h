#ifndef MOVE_GENERATOR_
#define MOVE_GENERATOR_

#include <vector>
#include <unordered_map>

#include "pos.h"
#include "move.h"
#include "board.h"

using std::vector;
using std::unordered_map;

class MoveGenerator {

    Player player_;
    Player opponent_;
    const ChessBoard *board_;
    ChessPos kingPos_;
    bool doEnPassantCheck_, hasForced_, cannotMove_;

    bool attacked_[64];
    bool pinned_[64][9];
    bool forced_[64];

    static unordered_map<std::size_t, vector<ChessMove>> knownBoards_;

    bool willMoveCapture(ChessMove &move) const;
    bool enPassantCheck(ChessMove &move) const;

    void addPiecesInDir(vector<ChessPos> &positions, ChessPos start, int dir) const;

    void setAttackedForPiece(ChessPiece piece, ChessPos pos);
    void genMovesForPiece(vector<ChessMove> &moves, ChessPiece piece, ChessPos start) const;
    
    void genPawnAttacks();
    void genKnightAttacks();
    void genKingAttacks();
    void genSlidingAttacks();

    void genKingMoves(vector<ChessMove> &moves) const;
    void genPawnMoves(vector<ChessMove> &moves) const;
    void genKnightMoves(vector<ChessMove> &moves) const;
    void genSlidingMoves(vector<ChessMove> &moves) const;

    void setPinned();
    void setAttacked();
    void setForced();

    public:

    MoveGenerator();

    bool fiftyMoveRuleStalemate() const;
    bool fiftyMoveRuleStalemate(ChessBoard &board) const;

    bool stalemate();
    bool stalemate(ChessBoard &board);

    bool inCheck() const;
    bool inCheck(ChessBoard &board);

    bool hasLost();
    bool hasLost(ChessBoard &board);

    vector<ChessMove> getMoves();
    vector<ChessMove> getMoves(ChessBoard &board);
    // const vector<ChessMove> &getMoves() const;
    // const vector<ChessMove> &getMoves(ChessBoard &board);

    void setBoard(ChessBoard &board);
    void printAttacked() const;
    void printForced() const;

};

#endif