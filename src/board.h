#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include <string>
#include <array>
#include <unordered_map>

#include "piece.h"
#include "move.h"

using std::pair;
using std::vector;
using std::string;
using std::array;
using std::unordered_map;

using namespace chess_defs;

class ChessBoard {

    friend class ChessBoardHash;

    unordered_map<ChessPos, ChessPiece, ChessPosHash> pieces_;
    vector<ChessMove> moves_;
    vector<string> moveStrings_;

    array<unsigned short, 2> score_;
    array<bool, 2> inCheck_;

    void updateScore();
    bool calcIfInCheck(Player player);
    void addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow);
    bool willMoveCapture(ChessMove move);

    vector<ChessMove> getMovesForPiece(ChessPiece piece, ChessPos pos);
    vector<ChessMove> getMovesForPlayer(Player player);

    public:
    ChessBoard(bool fill = true);
    ChessBoard(vector<ChessMove> moves);
    ChessBoard(const ChessBoard &oldBoard);
    bool operator==(const ChessBoard &other) const;

    int currentTurnNum();
    Player currentPlayer();
    int playerScore(Player player);
    bool inCheck(Player player);
    bool hasWon(Player player);
    std::size_t getHash();

    void printBoard();
    void printMoves();
    void addPiece(ChessPos pos, ChessPiece piece);

    vector<ChessMove> getPossibleMoves();
    void doMove(ChessMove move);
};

class ChessBoardHash {
    public:
    std::size_t operator()(ChessBoard const& board) const;
};

#endif