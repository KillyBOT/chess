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
    ChessPos kingPos_, enPassantSquare_;
    // bool hasForced_, cannotMove_;
    // bool doEnPassantCheck_;

    U64 occupied_, playerOccupied_, opponentOccupied_;
    U64 attacked_;
    // U64 pinned_;
    // bool pinnedDirs_[64][8];
    // U64 forced_;

    //static unordered_map<std::size_t, vector<ChessMove>> knownBoards_;

    bool hasPieceAtPos(int ind) const;
    bool willMoveCapture(ChessMove &move) const;
    bool enPassantCheck(ChessMove &move) const;
    
    //U64 dirAttacks(ChessPos start, int dir, U64 occupied) const;

    inline U64 rookAttacks(ChessPos start, U64 occupied) const {
        return kRookAttacks[start][bitboard_ind(occupied & kRookAttackMasks[start], kRookMagics[start], kMagicRookIndBits[start])];
    }
    inline U64 bishopAttacks(ChessPos start, U64 occupied) const {
        return kBishopAttacks[start][bitboard_ind(occupied & kBishopAttackMasks[start], kBishopMagics[start], kMagicBishopIndBits[start])];
    }
    void setAttacked(const ChessBoard *board);
    void genPawnAttacks(const ChessBoard *board);
    void genKnightAttacks(const ChessBoard *board);
    void genKingAttacks(const ChessBoard *board);
    void genSlidingAttacks(const ChessBoard *board);

    // inline void addMove(vector<ChessMove> &moves, ChessMove &move) const {
    //     if(!this->hasForced_ || bit(this->forced_, move.newPos)) moves.push_back(move);
    // }

    void genPseudoLegalMoves(vector<ChessMove> &moves) const;
    void genLegalMoves(vector<ChessMove> &legalMoves, vector<ChessMove> &moves);
    void genKingMoves(vector<ChessMove> &moves) const;
    void genPawnMoves(vector<ChessMove> &moves) const;
    void genKnightMoves(vector<ChessMove> &moves) const;
    void genSlidingMoves(vector<ChessMove> &moves) const;

    // void setPinned();
    // void setForced();
    // void setPinnedAndForced();


    public:

    MoveGenerator();

    bool fiftyMoveRuleStalemate() const;
    bool fiftyMoveRuleStalemate(ChessBoard &board) const;

    bool stalemate();
    bool stalemate(ChessBoard &board);

    bool inCheck() const;
    bool inCheck(ChessBoard &board, Player player) const;

    bool hasLost();
    bool hasLost(ChessBoard &board);

    vector<ChessMove> getMoves();
    vector<ChessMove> getMoves(ChessBoard &board);
    // const vector<ChessMove> &getMoves() const;
    // const vector<ChessMove> &getMoves(ChessBoard &board);

    void setBoard(ChessBoard &board);
    void printAttacked() const;
    // void printForced() const;

};

extern MoveGenerator gMoveGenerator;

#endif