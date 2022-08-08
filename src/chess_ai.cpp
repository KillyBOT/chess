#include <unordered_map>
#include <vector>
#include <string>

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

vector<ChessBoard> ChessAI::getChildren(ChessBoard board){
    //board.printBoard();

    if(!this->boardChildren_.count(board)){
        MoveGenerator mg = MoveGenerator(board);
        vector<ChessBoard> children;
        for(ChessMove move : mg.getMoves()){
            ChessBoard child = ChessBoard(board);
            child.doMove(move);
            //child.printBoard();
            children.push_back(child);
        }

        this->boardChildren_.insert(pair<ChessBoard,vector<ChessBoard>>(board,children));
    }

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