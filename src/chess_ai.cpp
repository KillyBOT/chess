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

vector<ChessBoard> &ChessAI::getChildren(ChessBoard &board){
    //board.printBoard();

    if(!this->boardChildren_.count(board)){
        mg_.setBoard(board);
        vector<ChessBoard> children;
        for(ChessMove move : mg_.getMoves()){
            board.doMove(move);
            //child.printBoard();
            children.push_back(board);
            board.undoLastMove();
        }
        this->boardChildren_.insert(pair<ChessBoard,vector<ChessBoard>>(board,children));
    }

    //std::cout << this->boardChildren_.at(board).size() << std::endl;

    return this->boardChildren_.at(board);
    // vector<ChessBoard> children;
    // MoveGenerator mg = MoveGenerator(board);

    // for(ChessMove move : mg.getMoves()){
    //     ChessBoard child = board;
    //     child.doMove(move);
    //     children.push_back(child);
    // }

    // return children;
}