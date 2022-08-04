#include <unordered_map>
#include <vector>
#include <string>

#include "chess_ai.h"
#include "board.h"

using std::unordered_map;
using std::vector;
using std::string;

ChessAI::ChessAI(string name){
    this->name_ = name;
}

string ChessAI::name() const{
    return this->name_;
}

vector<ChessBoard> &ChessAI::getChildren(ChessBoard &board){
    if(!this->boardChildren_.count(board)){
        vector<ChessBoard> children;
        for(ChessMove move : board.getPossibleMoves()){
            ChessBoard child = ChessBoard(board);
            child.doMove(move);
            children.push_back(child);
        }

        this->boardChildren_.insert(pair<ChessBoard,vector<ChessBoard>>(board,children));
    }

    return this->boardChildren_[board];
}