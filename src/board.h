#ifndef BOARD_H_
#define BOARD_H_

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "piece_list.h"
#include "chess_defs.h"
#include "piece.h"
#include "move.h"
#include "bitboard.h"

using std::pair;
using std::vector;
using std::string;
using std::array;
using std::unordered_map;
using std::unordered_set;
using std::size_t;

extern size_t kZobristPieceNums[64][16];
extern size_t kZobristBlackToMoveNum;
extern size_t kZobristCastlingNums[16];
extern size_t kZobristEnPassantNums[9];

const Byte kCastlingWhiteKingside = 0b0001;
const Byte kCastlingWhiteQueenside = 0b0010;
const Byte kCastlingBlackKingside = 0b0100;
const Byte kCastlingBlackQueenside = 0b1000;
 
const Byte kCastlingWhiteKingsideRemoveMask = 0b1110;
const Byte kCastlingWhiteQueensideRemoveMask = 0b1101;
const Byte kCastlingBlackKingsideRemoveMask = 0b1011;
const Byte kCastlingBlackQueensideRemoveMask = 0b0111;

const char kPieceListInd[2][7] = {0, 0b0001, 0b0010, 0b0011, 0b0100, 0b0101, 0b0110, 0, 0b1001, 0b1010, 0b1011, 0b1100, 0b1101, 0b1110};

class ChessBoard {

    ChessPiece pieces_[64];
    
    bool blackToMove_;
    ChessPos whiteKingPos_, blackKingPos_;

    size_t key_;
    unsigned short boardData_;
    int moveNum_;

    void initKey(unsigned int data);
    void setKey(ChessMove move);

    public:

    U64 occupied[16];
    U64 totalOccupied;

    ChessBoard(bool initBoard = true);
    ChessBoard(std::string fenStr);
    //ChessBoard(const ChessBoard &board);

    inline Player player() const {
        return this->blackToMove_ ? kPlayerBlack : kPlayerWhite;
    }
    inline Player opponent() const {
        return this->player() == kPlayerWhite ? kPlayerBlack : kPlayerWhite;
    }
    inline const ChessPiece &piece(ChessPos pos) const {
        return this->pieces_[pos];
    }
    inline const size_t &key() const {
        return this->key_;
    }
    inline const Byte movesSinceLastCapture() const {
        return this->boardData_ >> 11;
    }
    inline ChessPos kingPos(Player player) const {
        return (player == kPlayerWhite ? this->whiteKingPos_ : this->blackKingPos_);
    }
    inline ChessPos enPassantSquare() const {
        return (this->boardData_ >> 4) & 0b1111111;
    }
    inline bool canCastle(Player player, bool kingside) const {
        return this->boardData_ & (1 << (player << 1 + !kingside));
    }
    inline U64 rookAttacks(ChessPos start, U64 occupied) const {
        U64 blockers = occupied & kRookCheckMasks[start];
        if(blockers) return kRookAttacks[start][bitboard_ind(blockers, kRookMagics[start], kMagicRookIndBits[start])];
        return kRookAttackMasks[start];
    }
    inline U64 bishopAttacks(ChessPos start, U64 occupied) const {
        U64 blockers = occupied & kBishopCheckMasks[start];
        if(blockers) return kBishopAttacks[start][bitboard_ind(blockers, kBishopMagics[start], kMagicBishopIndBits[start])];
        return kBishopAttackMasks[start];
    }

    int turnNum() const;
    int playerScore(Player player) const;
    std::string fen() const;
    bool posAttacked(ChessPos pos, Player attacker) const;
    bool inCheck(Player player) const;

    void printBoard() const;
    void printMoves() const;
    void printPieces() const;

    //WARNING: if it's an invalid position, it will not work!
    void addPiece(ChessPos pos, ChessPiece piece);
    //WARNING: if it's an invalid position, it will not work!
    ChessPiece removePiece(ChessPos pos);
    //WARNING: if either of the positions are invalid, it will not work!
    void movePiece(ChessPos oldPos, ChessPos newPos);
    void fromFen(std::string fen);

    void doMove(ChessMove move, bool update = true);
    void undoMove(ChessMove move, bool update = true);
};

void init_zobrist_nums();

#endif