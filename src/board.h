#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include "piece.h"
#include "move.h"

using std::pair;
using std::vector;
using std::string;
using std::array;
using std::unordered_map;
using std::unordered_set;
using std::size_t;

using namespace chess_defs;

using ChessPosSet = unordered_set<ChessPos, ChessPosHash>;

class ChessBoard {

    //friend class ChessBoardHash;

    unordered_map<ChessPos, ChessPiece, ChessPosHash> pieces_;
    vector<ChessMove> moves_;
    unordered_map<size_t, int> seenBoards_;
    
    //For use with the zobrist hash
    //First bit is if there's an en passant file
    //Second 3 bits are the en passant file
    //Last 4 bits are the castling rights of the current board
    vector<Byte> gameData_; 
    vector<size_t> boardHashHistory_;
    //size_t hash_;
    array<array<size_t,12>,64> zobristPieces_;
    size_t zobristBlackToMove_;
    array<size_t, 16> zobristCastlingRights_;
    array<size_t, 8> zobristEnPassantFile_;

    //void updateHash();
    void updateZobrist(ChessMove move);
    void addBoardToSeen();

    public:

    ChessBoard(bool fill = true);
    ChessBoard(vector<ChessMove> moves);
    ChessBoard(const ChessBoard &oldBoard);
    bool operator==(const ChessBoard &other) const;
    bool operator!=(const ChessBoard &other) const;

    int turnNum() const;
    Player player() const;
    Player opponent() const;
    int playerScore(Player player) const;
    ChessMove lastMove() const;
    bool hasPiece(ChessPos pos) const;
    const ChessPiece &piece(ChessPos pos) const;
    const unordered_map<ChessPos, ChessPiece, ChessPosHash> &pieces() const;
    const vector<ChessMove> &moves() const;
    const size_t &zobristKey() const;
    const unordered_map<size_t, int> &seenBoards() const;
    //std::size_t hash() const;

    void printBoard() const;
    void printAttacked() const;
    //void printAttackedDict() const;
    void printMoves() const;

    void resetZobrist();
    void addPiece(ChessPos pos, ChessPiece piece);
    void removePiece(ChessPos pos);

    //vector<ChessMove> getPossibleMoves() const;
    void doMove(ChessMove move, bool updateHash = true);
    void undoMove(ChessMove move, bool updateHash = true);
    void undoLastMove(bool updateHash = true);

};

class ChessBoardHash {
    public:
    std::size_t operator()(ChessBoard const& board) const;
};

#endif