#include <iostream>

#include "minimax.h"
#include "move_generator.h"
#include "board.h"

using std::pair;
using std::vector;

int heuristic_basic(ChessBoard board, Player maxPlayer) {

    MoveGenerator mg = MoveGenerator(board);
    Player other = (maxPlayer == kPlayerBlack) ? kPlayerWhite : kPlayerBlack;
    int score = 0;

    if(mg.hasLost()) score -= 1000;
    else if(mg.inCheck()) score -= 100;
    
    if(board.player() != maxPlayer) score *= -1;

    score += board.playerScore(maxPlayer) - board.playerScore(other);

    return score;
}

int Minimax::boardScore(ChessBoard board, Player player){
    if(!this->boardScores_.count(board)) this->boardScores_.insert(pair<ChessBoard,pair<int,int>>(board, pair<int,int>(this->heuristicFunc_(board, kPlayerWhite),this->heuristicFunc_(board, kPlayerBlack))));

    if(player == kPlayerWhite) return this->boardScores_[board].first;
    else return this->boardScores_[board].second;
}

int Minimax::evalHelpMinimax(ChessBoard board, int depth, Player maxPlayer){

    if(depth <= 0 || this->getChildren(board).empty()) return this->boardScore(board, maxPlayer);

    int val;

    if(board.player() == maxPlayer){

        val = -2147483647;

        for(ChessBoard child : this->getChildren(board)) val = std::max(evalHelpMinimax(child, depth-1, maxPlayer),val);

    } else {

        val = 2147483647;

        for(ChessBoard child : this->getChildren(board)) val = std::min(evalHelpMinimax(child, depth-1, maxPlayer),val);

    }

    return val;
}
int Minimax::evalHelpAB(ChessBoard board, int depth, int alpha, int beta, Player maxPlayer){

    //std::cout << depth << std::endl;

    if(depth <= 0 || this->getChildren(board).empty()) return this->boardScore(board, maxPlayer);

    int val;

    if(board.player() == maxPlayer){

        val = -2147483647;

        for(ChessBoard child : this->getChildren(board)){
            val = std::max(evalHelpAB(child, depth-1, alpha, beta, maxPlayer),val);
            alpha = std::max(alpha,val);
            if(beta <= alpha) break;
        }

    } else {

        val = 2147483647;

        for(ChessBoard child : this->getChildren(board)){
            val = std::min(evalHelpAB(child, depth-1, alpha, beta, maxPlayer),val);
            beta = std::min(beta, val);
            if(beta <= alpha) break;
        }

    }

    return val;
}

Minimax::Minimax(int(*heuristicFunc)(ChessBoard, Player), int depth, bool doABPruning) : ChessAI("Minimax"){
    this->heuristicFunc_ = heuristicFunc;
    this->depth_ = depth;
    this->doABPruining_ = doABPruning;
}

ChessMove Minimax::findOptimalMove(ChessBoard board){

    int score;
    int bestScore = -2147483647;
    ChessMove bestMove;

    //board.printBoard();
    //std::cout << board.player() << std::endl;

    for(ChessBoard child : this->getChildren(board)){
        //std::cout << child.player() << std::endl;
        //child.printBoard();

        if(this->doABPruining_) score = evalHelpAB(child, this->depth_, -2147483647, 2147483647, board.player());
        else score = evalHelpMinimax(child, this->depth_, board.player());

        if(score > bestScore){
            bestScore = score;
            bestMove = child.lastMove();
        }
    }

    return bestMove;
}

bool Minimax::setABPruning(){
    this->doABPruining_ = !this->doABPruining_;
    return this->doABPruining_;
}