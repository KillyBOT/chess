#include <iostream>
#include <vector>
#include <chrono>
#include <exception>

#include "minimax.h"
#include "move_generator.h"
#include "board.h"

using std::pair;
using std::vector;

static MoveGenerator mg;

int heuristic_basic(ChessBoard &board, Player maxPlayer) {

    mg.setBoard(board);
    Player opponent = (maxPlayer == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
    int score = 0;

    if(mg.hasLost()) score -= 1000;
    else if(mg.inCheck()) score -= 100;
    
    score += board.playerScore(maxPlayer) - board.playerScore(opponent);

    if(board.player() == opponent) score *= -1;

    return score;
}

int Minimax::evalBoard(ChessBoard &board){
    if(!this->boardScores_.count(board.zobristKey())) {
        int boardEval = this->heuristicFunc_(board, this->maxPlayer_);
        this->boardScores_.emplace(board.zobristKey(), this->heuristicFunc_(board, this->maxPlayer_));
    }

    // if(this->boardScores_.at(board.zobristKey()) != 0) {
    //     std::cout << this->boardScores_.at(board.zobristKey()) << std::endl;
    //     board.printBoard();
    // }

    return this->boardScores_.at(board.zobristKey());
}

int Minimax::evalHelpMinimax(ChessBoard &board, int depth){

    if(this->getMoves(board).empty() || depth <= 0) return this->evalBoard(board);

    int val;

    if(board.player() == this->maxPlayer_){

        val = -2147483647;

        for(ChessMove move : this->getMoves(board)){
            board.doMove(move);
            val = std::max(evalHelpMinimax(board,depth-1),val);
            board.undoLastMove();
        } 

    } else {

        val = 2147483647;

        for(ChessMove move : this->getMoves(board)){
            board.doMove(move);
            val = std::min(evalHelpMinimax(board,depth-1),val);
            board.undoLastMove();
        }

    }

    return val;
}
int Minimax::evalHelpAB(ChessBoard &board, int depth, int alpha, int beta){

    //std::cout << depth << std::endl;

    if(this->getMoves(board).empty() || depth <= 0) return this->evalBoard(board);

    int val;

    if(board.player() == this->maxPlayer_){

        val = -2147483647;

        for(ChessMove move : this->getMoves(board)){
            board.doMove(move);
            //board.printBoard();
            val = std::max(evalHelpAB(board, depth-1, alpha, beta), val);
            board.undoLastMove();
            alpha = std::max(alpha, val);
            if(beta <= alpha) break;
        }

    } else {

        val = 2147483647;

        for(ChessMove move : this->getMoves(board)){
            board.doMove(move);
            //board.printBoard(); 
            val = std::min(evalHelpAB(board, depth-1, alpha, beta), val);
            board.undoLastMove();
            beta = std::min(beta, val);
            if(beta <= alpha) break;
        }

    }

    return val;
}

Minimax::Minimax(int(*heuristicFunc)(ChessBoard&, Player), int depth, bool doABPruning) : ChessAI("Minimax"){
    this->heuristicFunc_ = heuristicFunc;
    this->depth_ = depth;
    this->doABPruining_ = doABPruning;
}

ChessMove Minimax::findOptimalMove(ChessBoard &board){

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    int score;
    int bestScore = -2147483647;
    ChessMove bestMove;
    this->maxPlayer_ = board.player();

    //board.printBoard();
    //std::cout << board.player() << std::endl;

    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    for(ChessMove move: this->getMoves(board)){
        //std::cout << child.player() << std::endl;
        //child.printBoard();

        //if(this->doABPruining_) score = evalHelpAB(child, this->depth_, -2147483647, 2147483647, board.player());
        //else score = evalHelpMinimax(child, this->depth_, board.player());
        board.doMove(move);
        if(this->doABPruining_) score = evalHelpAB(board, this->depth_ - 1, -2147483647, 2147483647);
        else score = this->evalHelpMinimax(board, this->depth_-1);
        board.undoLastMove();

        // child.printBoard();
        // std::cout << score << std::endl;

        if(score > bestScore){
            //std::cout << move.str() << std::endl;
            //board.printBoard();
            bestScore = score;
            bestMove = move;
        }
    }

    auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << "Found move in [" << endTime - startTime << "] ms" << std::endl;

    return bestMove;
}

bool Minimax::setABPruning(){
    this->doABPruining_ = !this->doABPruining_;
    return this->doABPruining_;
}