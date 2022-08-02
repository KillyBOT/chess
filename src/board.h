#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include <string>
#include <unordered_map>

#include "piece.h"
#include "move.h"

using std::pair;
using std::vector;
using std::string;
using std::unordered_map;

using namespace chess_defs;

class ChessBoard {

    unordered_map<ChessPos, ChessPiece, ChessPosHash> pieces_;
    vector<ChessMove> moves_;
    vector<string> moveStrings_;

    void addMovesInDir(vector<ChessMove> &moves, ChessPos startPos, char dCol, char dRow);
    bool willMoveCapture(ChessMove move);

    vector<ChessMove> getMovesForPiece(ChessPiece piece, ChessPos pos);

    public:
    ChessBoard();
    ChessBoard(vector<ChessMove> moves);
    ChessBoard(const ChessBoard &oldBoard);

    int currentTurnNum();
    Player currentPlayer();
    bool inCheck(Player player);
    bool hasWon(Player player);

    void printBoard();
    void printMoves();

    vector<ChessMove> getPossibleMoves();
    void doMove(ChessMove move);
};

#endif