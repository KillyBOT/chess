#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

#include "chess_ai.h"
#include "board.h"
#include "move_generator.h"

using std::unordered_map;
using std::vector;
using std::string;

ChessAI::ChessAI(string name){
    this->name_ = name;
}

string ChessAI::name() const{
    return this->name_;
}

const vector<ChessMove> &ChessAI::getMoves(ChessBoard &board){

    if(!this->moveMap_.count(board.zobristKey())) this->moveMap_.emplace(board.zobristKey(),this->mg_.getMoves(board));

    return this->moveMap_.at(board.zobristKey());
}