#ifndef MOVE_GENERATOR_
#define MOVE_GENERATOR_

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "pos.h"
#include "move.h"
#include "board.h"

using std::vector;
using std::unordered_set;
using ChessPosSet = unordered_set<ChessPos, ChessPosHash>;

class MoveGenerator {

    ChessBoard *board_;
    Player player_, opponent_;
    ChessPos kingPos_;
    ChessPosSet attacked_;
    ChessPosSet pinned_;

    vector<pair<ChessPos,ChessPiece>> piecesInDir(ChessPos start, char dCol, char dRow) const;
    ChessPosSet untilFirstInDir(ChessPos start, char dCol, char dRow, bool countFirst = true) const;
    ChessPos pinnedInDir(ChessPos start, char dCol ,char dRow) const;
    void addMovesInDir(vector<ChessMove> &moves, ChessPiece piece, ChessPos startPos, char dCol, char dRow) const;
    void addAttacksInDir(ChessPos pos, char dCol, char dRow);
    bool willMoveCapture(ChessMove &move) const;
    void setMoveScore(ChessMove &move) const;
    bool compareMoves(const ChessMove &a, const ChessMove &b) const;

    void addPieceAttacks(ChessPos pos, ChessPiece piece);
    vector<ChessMove> addPieceMoves(ChessPos pos, ChessPiece piece) const;
    ChessPosSet forcedPositions() const;
    bool enPassantCheck(ChessMove move) const;

    void setKingPos();
    void setAttacked();
    void setPinned();

    public:
    MoveGenerator();
    MoveGenerator(ChessBoard &board);

    void setBoard(ChessBoard &board);

    bool inCheck() const;
    bool inCheck(ChessBoard &board);
    bool hasLost() const;
    bool hasLost(ChessBoard &board);
    bool stalemate() const;
    bool stalemate(ChessBoard &board);

    const ChessPosSet &pinned() const;
    const ChessPosSet &attacked() const;
    ChessPosSet forced() const;

    vector<ChessMove> getMoves() const;
    vector<ChessMove> getMoves(ChessBoard &board);
    vector<ChessMove> getMovesOrdered() const;
    vector<ChessMove> getMovesOrdered(ChessBoard &board);

    void printAttacked() const;

    friend int heuristic_complex(ChessBoard &board, Player maxPlayer);
};

#endif