#ifndef MOVE_GENERATOR_
#define MOVE_GENERATOR_

#include <vector>
#include <unordered_map>

#include "pos.h"
#include "move.h"
#include "board.h"
#include "bitboard.h"

using std::vector;
using std::unordered_map;

class MoveGenerator {

    Player player_;
    Player opponent_;
    const ChessBoard *board_;
    ChessPos kingPos_;
    bool hasForced_, cannotMove_;
    bool doEnPassantCheck_;

    BitBoard occupied_;
    BitBoard opponentOccupied_;
    BitBoard attacked_;
    BitBoard pinned_;
    bool pinnedDirs_[64][8];
    BitBoard forced_;

    //static unordered_map<std::size_t, vector<ChessMove>> knownBoards_;

    bool willMoveCapture(ChessMove &move) const;
    bool enPassantCheck(ChessMove &move) const;

    void addPiecesInDir(vector<ChessPos> &positions, ChessPos start, int dir) const;
    
    BitBoard dirAttacks(ChessPos start, BitBoard occupied, int dir) const;
    BitBoard diagAttacks(ChessPos start) const;
    BitBoard antiDiagAttacks(ChessPos start) const;
    BitBoard rankAttacks(ChessPos start) const;
    BitBoard fileAttacks(ChessPos start) const;

    void genPawnAttacks();
    void genKnightAttacks();
    void genKingAttacks();
    void genSlidingAttacks();

    inline void addMove(vector<ChessMove> &moves, ChessMove &move) const {
        if(!this->hasForced_ || bit(this->forced_, move.newPos)) moves.push_back(move);
    }
    void genKingMoves(vector<ChessMove> &moves) const;
    void genPawnMoves(vector<ChessMove> &moves) const;
    void genKnightMoves(vector<ChessMove> &moves) const;
    void genSlidingMoves(vector<ChessMove> &moves) const;

    void setAttacked();
    void setPinned();
    void setForced();
    void setPinnedAndForced();

    vector<ChessMove> genPseudoLegalMoves() const;
    vector<ChessMove> genLegalMoves(vector<ChessMove> &moves);

    public:

    MoveGenerator();

    bool fiftyMoveRuleStalemate() const;
    bool fiftyMoveRuleStalemate(ChessBoard &board) const;

    bool stalemate();
    bool stalemate(ChessBoard &board);

    bool inCheck() const;
    bool inCheck(ChessBoard &board);

    bool hasLost() const;
    bool hasLost(ChessBoard &board);

    vector<ChessMove> getMoves() const;
    vector<ChessMove> getMoves(ChessBoard &board);
    // const vector<ChessMove> &getMoves() const;
    // const vector<ChessMove> &getMoves(ChessBoard &board);

    void setBoard(ChessBoard &board);
    void printAttacked() const;
    void printForced() const;

};

extern MoveGenerator gMoveGenerator;

#endif