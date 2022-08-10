#ifndef CHESS_AI_H_
#define CHESS_AI_H_

#include <unordered_map>
#include <vector>
#include <string>

#include "board.h"
#include "move_generator.h"

using std::unordered_map;
using std::vector;
using std::string;

class ChessAI {
    string name_;
    unordered_map<size_t, vector<ChessMove>> moveMap_;
    MoveGenerator mg_;

    public:

    ChessAI(string name);

    string name() const;

    inline const vector<ChessMove> &getMoves(ChessBoard &board) {
        if(!this->moveMap_.count(board.zobristKey())) this->moveMap_.emplace(board.zobristKey(),this->mg_.getMovesOrdered(board));
        return this->moveMap_.at(board.zobristKey());
    }

    virtual ChessMove findOptimalMove(ChessBoard &board) = 0;

};

#endif