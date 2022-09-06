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

    U64 occupied_, opponentOccupied_, validPositions_;
    U64 attacked_;
    // U64 pinned_;
    // bool pinnedDirs_[64][8];
    // U64 forced_;

    //static unordered_map<std::size_t, vector<ChessMove>> knownBoards_;

    bool hasPieceAtPos(ChessPos pos) const;
    bool willMoveCapture(ChessMove &move) const;
    bool enPassantCheck(ChessMove &move) const;
    
    //U64 dirAttacks(ChessPos start, int dir, U64 occupied) const;

    // inline U64 rookAttacks(ChessPos start, U64 occupied) const {
    //     U64 blockers = occupied & kRookAttackMasks[start];
    //     if(blockers) return kRookAttacks[start][bitboard_ind(blockers, kRookMagics[start], kMagicRookIndBits[start])];
    //     return kRookAttackMasks[start];
    // }
    // inline U64 bishopAttacks(ChessPos start, U64 occupied) const {
    //     U64 blockers = occupied & kBishopAttackMasks[start];
    //     print_bitboard(kBishopAttackMasks[start]);
    //     if(blockers) return kBishopAttacks[start][bitboard_ind(blockers, kBishopMagics[start], kMagicBishopIndBits[start])];
    //     return kBishopAttackMasks[start];
    // }
    void setAttacked(const ChessBoard *board);
    void genPawnAttacks(const ChessBoard *board);
    void genKnightAttacks(const ChessBoard *board);
    void genKingAttacks(const ChessBoard *board);
    void genSlidingAttacks(const ChessBoard *board);

    // inline void addMove(vector<ChessMove> &moves, ChessMove &move) const {
    //     if(!this->hasForced_ || bit(this->forced_, move.newPos)) moves.push_back(move);
    // }

    void genPseudoLegalMoves(vector<ChessMove> &moves) const;
    void genLegalMoves(vector<ChessMove> &legalMoves, vector<ChessMove> &moves, bool incQuiet);
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

    bool hasLost();
    bool hasLost(ChessBoard &board);

    vector<ChessMove> getMoves(bool incQuiet = true, bool orderMoves = false);
    vector<ChessMove> getMoves(ChessBoard &board, bool incQuiet = true, bool orderMoves = false);
    
    bool isNodeQuiet();
    bool isNodeQuiet(ChessBoard &board);
    // const vector<ChessMove> &getMoves() const;
    // const vector<ChessMove> &getMoves(ChessBoard &board);

    void setBoard(ChessBoard &board);
    void printAttacked() const;
    // void printForced() const;

};

extern MoveGenerator gMoveGenerator;

#endif