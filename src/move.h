#ifndef MOVE_H_
#define MOVE_H_

#include <string>
#include <vector>

#include "chess_defs.h"
#include "piece.h"

struct ChessPos{
    char col, row;

    ChessPos(char col = 'a', char row = 1);

    bool isInBounds();
    std::string str();

    bool operator==(const ChessPos &other) const;
};

struct ChessPosHash{
    std::size_t operator() (const ChessPos &pos) const;
};

struct ChessMove{
    ChessPos pos;
    ChessPos newPos;
    ChessPiece promotion;

    ChessMove(ChessPos pos = ChessPos(), ChessPos newPos = ChessPos(), ChessPiece promotion = ChessPiece());

    bool isInBounds();

    //bool check_move_validity(ChessBoard &board);
    std::string basicStr();
};

#endif