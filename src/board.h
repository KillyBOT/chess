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

using ChessPosSet = unordered_set<ChessPos, ChessPosHash>;

class ChessBoard {

    friend class ChessBoardHash;

    unordered_map<ChessPos, ChessPiece, ChessPosHash> pieces_;
    ChessPosSet attackedByWhite_, attackedByBlack_, pinned_;

    vector<ChessMove> moves_;
    ChessPos blackKingPos_, whiteKingPos_;
    bool whiteInCheck_, blackInCheck_;
    BoardState state_;
    std::size_t hash_;

    bool hasPieceAtPos(ChessPos pos) const;
    ChessPiece pieceAtPos(ChessPos pos) const;
    Player playerAtPos(ChessPos pos) const;

    void updateHash();
    void updateState();
    void setKingPos();
    void setPinned();
    void resetAttacked();

    vector<pair<ChessPos,ChessPiece>> piecesInDir(ChessPos start, char dCol, char dRow) const;
    ChessPosSet untilFirstInDir(ChessPos start, char dCol, char dRow, bool countFirst = true) const;
    ChessPos pinnedInDir(ChessPos start, char dCol ,char dRow) const;
    void addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow) const;
    void addPosInDir(ChessPosSet &positions, ChessPos pos, char dCol, char dRow) const;
    bool willMoveCapture(ChessMove &move) const;

    ChessPosSet getAttackedByPiece(ChessPos pos) const;
    vector<ChessMove> getMovesForPiece(ChessPos pos) const;
    vector<ChessMove> getMovesForPlayer(Player player) const;
    unordered_set<ChessPos,ChessPosHash> forcedNewPositions(Player player) const;

    void resetAttackedForPlayer(Player player);
    void makePosAttacked(ChessPos attacked, ChessPos attackedBy);
    void makePosUnattacked(ChessPos attacked, ChessPos attackedBy);
    void addPieceAttacking(ChessPos pos);
    //void removePieceAttacking(ChessPos pos);
    //void updatePieceAttacking(ChessPos pos);

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
    //bool posAttacked(ChessPos pos) const;
    bool inCheck(Player player) const;
    bool hasWon(Player player) const;
    bool stalemate() const;
    std::size_t hash() const;
    BoardState state() const;

    void printBoard() const;
    void printAttacked() const;
    //void printAttackedDict() const;
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