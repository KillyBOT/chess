#ifndef CHESS_AI_H_
#define CHESS_AI_H_

#include <unordered_map>
#include <vector>
#include <string>

#include "board.h"

using std::unordered_map;
using std::vector;
using std::string;

class ChessAI {
    string name_;
    unordered_map<ChessBoard,vector<ChessBoard>,ChessBoardHash> boardChildren_;

    public:

    ChessAI(string name);

    string name() const;

    vector<ChessBoard> &getChildren(ChessBoard &board);

    virtual ChessMove findOptimalMove(ChessBoard board) = 0;

};

#endif