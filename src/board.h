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

using namespace chess_defs;

class ChessBoard {

    friend class ChessBoardHash;

    unordered_map<ChessPos, ChessPiece, ChessPosHash> pieces_;
    unordered_map<ChessPos, unordered_set<ChessPos, ChessPosHash>, ChessPosHash> attacked_;
    unordered_set<ChessPos, ChessPosHash> pinned_;

    vector<ChessMove> moves_;
    ChessPos blackKingPos_, whiteKingPos_;
    bool whiteInCheck_, blackInCheck_;
    BoardState state_;
    std::size_t hash_;

    bool hasPieceAtPos(ChessPos pos) const;
    ChessPiece pieceAtPos(ChessPos pos) const;
    Player playerAtPos(ChessPos pos) const;
    bool hasThreats(ChessPos pos) const;

    void updateHash();
    void updateState();
    void setKingPos();
    void setPinned();

    vector<pair<ChessPos,ChessPiece>> piecesInDir(ChessPos start, char dCol, char dRow) const;
    ChessPos pinnedInDir(ChessPos start, char dCol ,char dRow) const;
    void addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow) const;
    void addPosInDir(unordered_set<ChessPos, ChessPosHash> &positions, ChessPos pos, char dCol, char dRow) const;
    bool willMoveCapture(ChessMove &move) const;

    unordered_set<ChessPos, ChessPosHash> getAttackedByPiece(ChessPos pos) const;
    vector<ChessMove> getMovesForPiece(ChessPos pos) const;
    vector<ChessMove> getMovesForPlayer(Player player) const;

    void resetAttacked();
    void makePosAttacked(ChessPos attacked, ChessPos attackedBy);
    void makePosUnattacked(ChessPos attacked, ChessPos attackedBy);
    void addPieceAttacking(ChessPos pos);
    void removePieceAttacking(ChessPos pos);
    void updatePieceAttacking(ChessPos pos);

    public:
    ChessBoard(bool fill = true, bool recordMoves = false);
    ChessBoard(vector<ChessMove> moves);
    ChessBoard(const ChessBoard &oldBoard);
    bool operator==(const ChessBoard &other) const;

    int currentTurnNum() const;
    Player currentPlayer() const;
    int playerScore(Player player) const;
    ChessMove lastMove() const;
    ChessPos kingPos(Player player) const;
    vector<ChessPos> attacksToPos(ChessPos pos) const;
    unordered_set<ChessPos,ChessPosHash> attackedByPlayer(Player player) const;
    vector<ChessPos> pinned() const;
    bool posAttacked(ChessPos pos) const;
    bool inCheck(Player player) const;
    bool hasWon(Player player) const;
    bool stalemate() const;
    std::size_t hash() const;
    BoardState state() const;

    void printBoard() const;
    void printAttacked() const;
    void printAttackedDict() const;
    void printMoves() const;

    void addPiece(ChessPos pos, ChessPiece piece);
    void removePiece(ChessPos pos);

    vector<ChessMove> getPossibleMoves() const;
    void doMove(ChessMove move, bool updateHash = true);
    void undoMove(ChessMove move, bool updateHash = true);
    void undoLastMove();

};

class ChessBoardHash {
    public:
    std::size_t operator()(ChessBoard const& board) const;
};

#endif