#ifndef BOARD_H_
#define BOARD_H_

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

const Byte kBoardDataWhiteKingside = 0b1110;
const Byte kBoardDataWhiteQueenside = 0b1101;
const Byte kBoardDataBlackKingside = 0b1011;
const Byte kBoardDataBlackQueenside = 0b0111;

class ChessBoard {

    ChessPiece pieces_[64];
    ChessPieceList pieceLists_[16];
    BitBoard occupied_;

    vector<ChessMove> moves_;
    vector<unsigned int> boardData_;
    vector<size_t> boardHistory_;
    
    bool blackToMove_;
    ChessPos whiteKingPos_, blackKingPos_;

    void addNewKey(ChessMove move);

    public:

    ChessBoard(bool initBoard = true);
    ChessBoard(std::string fenStr);
    ChessBoard(const ChessBoard &board);

    inline Player player() const {
        return this->blackToMove_ ? kPlayerBlack : kPlayerWhite;
    }
    inline Player opponent() const {
        return this->player() == kPlayerWhite ? kPlayerBlack : kPlayerWhite;
    }
    inline BitBoard occupied() const {
        return this->occupied_;
    }
    inline ChessPiece piece(ChessPos pos) const {
        return this->pieces_[pos];
    }
    inline const ChessPieceList &pieceList(ChessPiece piece) const {
        return this->pieceLists_[piece];
    }
    inline const size_t &key() const {
        return this->boardHistory_.back();
    }
    inline const ChessMove &lastMove() const {
        return this->moves_.back();
    }
    inline const Byte movesSinceLastCapture() const {
        return this->boardData_.back() >> 11;
    }
    inline ChessPos kingPos(Player player) const {
        return (player == kPlayerWhite ? this->whiteKingPos_ : this->blackKingPos_);
    }
    inline ChessPos enPassantSquare() const {
        return (this->boardData_.back() >> 4) & 0b1111111;
    }
    inline bool canCastle(Player player, bool kingside) const {
        return this->boardData_.back() & (1 << (player << 1 + !kingside));
    }

    int turnNum() const;
    int playerScore(Player player) const;
    std::string fen() const;

    void printBoard() const;
    void printMoves() const;
    void printPieces() const;

    //WARNING: if it's an invalid position, it will not work!
    void addPiece(ChessPos pos, ChessPiece piece);
    //WARNING: if it's an invalid position, it will not work!
    ChessPiece removePiece(ChessPos pos);
    //WARNING: if either of the positions are invalid, it will not work!
    void movePiece(ChessPos oldPos, ChessPos newPos);
    void resetKeys(unsigned int data);
    void fromFen(std::string fen);

    void doMove(ChessMove move, bool update = true);
    void undoMove(ChessMove move, bool update = true);
    void undoLastMove();
};

void init_zobrist_nums();

#endif