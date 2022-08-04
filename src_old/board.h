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

    unordered_map<ChessPos, ChessPiece, ChessPosHash> pieces_;
    unordered_map<ChessPos, vector<ChessPos>, ChessPosHash> threatened_;
    vector<ChessMove> moves_;
    vector<string> moveStrings_;
    array<unsigned short, 2> score_;
    BoardState state_;
    int moveNum_;
    Byte movesSinceLastCapture_; //Shouldn't ever be more than 100
    bool recordMoves_;
    ChessMove lastMove_;

    std::size_t hash_;

    bool hasPieceAtPos(ChessPos pos) const;

    void updateScore();
    void updateHash();
    void updateState();
    void addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow) const;
    void addPosInDir(vector<ChessPos> &positions, ChessPos pos, char dCol, char dRow) const;
    bool willMoveCapture(ChessMove &move) const;

    vector<ChessPos> getThreatenedByPiece(ChessPos pos) const;
    vector<ChessMove> getMovesForPiece(ChessPiece piece, ChessPos pos) const;
    vector<ChessMove> getMovesForPlayer(Player player) const;

    void movePiece(ChessMove move);
    void resetThreatened();
    void addPieceThreatening(ChessPos pos);
    void removePieceThreatening(ChessPos pos);
    void updatePieceThreatening(ChessPos pos);

    public:
    ChessBoard(bool fill = true, bool recordMoves = false);
    ChessBoard(vector<ChessMove> moves);
    ChessBoard(const ChessBoard &oldBoard);
    bool operator==(const ChessBoard &other) const;

    int currentTurnNum() const;
    Player currentPlayer() const;
    int playerScore(Player player) const;
    ChessMove lastMove() const;
    vector<ChessPos> getThreatened(Player player) const;
    bool inCheck(Player player) const;
    bool hasWon(Player player) const;
    bool stalemate() const;
    std::size_t hash() const;
    BoardState state() const;

    void printBoard() const;
    void printMoves() const;

    void addPiece(ChessPos pos, ChessPiece piece);

    vector<ChessMove> getPossibleMoves() const;
    void doMove(ChessMove move);
};

class ChessBoardHash {
    public:
    std::size_t operator()(ChessBoard const& board) const;
};

#endif