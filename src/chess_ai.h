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

    ChessAI(string name) : name_{name} {};

    inline string name() const {
        return this->name_;
    }

    virtual ChessMove findOptimalMove(ChessBoard &board) = 0;

};

#endif