#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <bitset>

#include "chess_defs.h"
#include "piece.h"
#include "move.h"

using std::pair;
using std::vector;
using std::string;
using std::array;
using std::unordered_map;
using std::unordered_set;
using std::size_t;

extern size_t kZobristPieceNums[64][12];
extern size_t kZobristBlackToMoveNum;
extern size_t kZobristCastlingNums[16];
extern size_t kZobristEnPassantNums[9];

const Byte kBoardDataWhiteKingside = 0b1110;
const Byte kBoardDataWhiteQueenside = 0b1101;
const Byte kBoardDataBlackKingside = 0b1011;
const Byte kBoardDataBlackQueenside = 0b0111;

class ChessBoard {

    ChessPiece pieces_[64];
    ChessPos piecePositions_[64];
    int pieceNum_;

    vector<ChessMove> moves_;
    vector<unsigned short> boardData_;
    vector<size_t> boardHistory_;

    bool blackToMove_;
    ChessPos whiteKingPos_, blackKingPos_;

    void addNewKey(ChessMove move);

    public:

    ChessBoard(bool initBoard = true);
    ChessBoard(const ChessBoard &board);

    inline int pieceNum() const {
        return this->pieceNum_;
    }
    inline Player player() const {
        return this->blackToMove_;
    }
    inline Player opponent() const {
        return !this->blackToMove_;
    }
    inline bool hasPieceAtPos(ChessPos pos) const {
        return this->pieces_[pos.pos].data;
    }
    inline const ChessPiece &piece(ChessPos pos) const {
        return this->pieces_[pos.pos];
    }
    inline const ChessPiece *pieces() const {
        return this->pieces_;
    }
    inline const ChessPos *piecePositions() const {
        return this->piecePositions_;
    }
    inline const size_t &key() const {
        return this->boardHistory_.back();
    }
    inline const ChessMove &lastMove() const {
        return this->moves_.back();
    }
    inline const Byte movesSinceLastCapture() const {
        return this->boardData_.back() >> 8;
    }
    inline ChessPos kingPos(Player player) const {
        return (player == kPlayerWhite ? this->whiteKingPos_ : this->blackKingPos_);
    }
    inline char enPassantFile() const {
        return this->boardData_.back() >> 4;
    }

    int turnNum() const;
    int playerScore(Player player) const;

    void printBoard() const;
    void printMoves() const;

    //WARNING: if it's an invalid position, it will not work!
    void addPiece(ChessPos pos, ChessPiece piece);
    //WARNING: if it's an invalid position, it will not work!
    void removePiece(ChessPos pos);
    //WARNING: if either of the positions are invalid, it will not work!
    void movePiece(ChessPos oldPos, ChessPos newPos);
    void resetKeys();

    void doMove(ChessMove move, bool update = true);
    void undoMove(ChessMove move, bool update = true);
    void undoLastMove();
};

void init_zobrist_nums();

#endif