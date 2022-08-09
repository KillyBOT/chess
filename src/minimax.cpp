// #include <iostream>
// #include <vector>
// #include <chrono>

// #include "minimax.h"
// #include "move_generator.h"
// #include "board.h"

// using std::pair;
// using std::vector;

// int heuristic_basic(ChessBoard &board, Player maxPlayer) {

//     MoveGenerator mg = MoveGenerator(board);
//     Player opponent = (maxPlayer == kPlayerWhite) ? kPlayerBlack : kPlayerWhite;
//     int score = 0;

//     if(mg.hasLost()) score -= 1000;
//     else if(mg.inCheck()) score -= 100;
    
//     score += board.playerScore(opponent) - board.playerScore(maxPlayer);

//     if(board.player() == maxPlayer) score *= -1;

//     return score;
// }

// int Minimax::evalBoard(ChessBoard &board){
//     if(!this->boardScores_.count(board)) {
//         int boardEval = this->heuristicFunc_(board, kPlayerWhite);
//         this->boardScores_.insert(pair<ChessBoard,pair<int,int>>(board, pair<int,int>(boardEval,-boardEval)));
//     }

//     if(this->maxPlayer_ == kPlayerWhite) return this->boardScores_[board].first;
//     else return this->boardScores_[board].second;
// }

// int Minimax::evalHelpMinimax(ChessBoard &board, int depth){

//     if(this->getChildren(board).empty() || depth <= 0) return this->evalBoard(board);

//     int val;

//     if(board.player() == this->maxPlayer_){

//         val = -2147483647;

//         for(ChessBoard &child : this->getChildren(board)) val = std::max(evalHelpMinimax(child,depth-1),val);

//     } else {

//         val = 2147483647;

//         for(ChessBoard &child : this->getChildren(board)) val = std::min(evalHelpMinimax(child,depth-1),val);

//     }

//     return val;
// }
// int Minimax::evalHelpAB(ChessBoard &board, int depth, int alpha, int beta){

//     //std::cout << depth << std::endl;

//     if(this->getChildren(board).empty() || depth <= 0) return this->evalBoard(board);

//     int val;

//     if(board.player() == this->maxPlayer_){

//         val = -2147483647;

//         for(ChessBoard &child : this->getChildren(board)){
//             val = std::max(evalHelpAB(child, depth-1, alpha, beta), val);
//             alpha = std::max(alpha, val);
//             if(beta <= alpha) break;
//         }

//     } else {

//         val = 2147483647;

//         for(ChessBoard &child : this->getChildren(board)){
//             val = std::min(evalHelpAB(child, depth-1, alpha, beta), val);
//             beta = std::min(beta, val);
//             if(beta <= alpha) break;
//         }

//     }

//     return val;
// }

// Minimax::Minimax(int(*heuristicFunc)(ChessBoard&, Player), int depth, bool doABPruning) : ChessAI("Minimax"){
//     this->heuristicFunc_ = heuristicFunc;
//     this->depth_ = depth;
//     this->doABPruining_ = doABPruning;
// }

// ChessMove Minimax::findOptimalMove(ChessBoard board){

//     int score;
//     int bestScore = -2147483647;
//     ChessMove bestMove;
//     this->maxPlayer_ = board.player();

//     //board.printBoard();
//     //std::cout << board.player() << std::endl;

//     for(ChessBoard &child : this->getChildren(board)){
//         //std::cout << child.player() << std::endl;
//         //child.printBoard();

//         //if(this->doABPruining_) score = evalHelpAB(child, this->depth_, -2147483647, 2147483647, board.player());
//         //else score = evalHelpMinimax(child, this->depth_, board.player());
//         if(this->doABPruining_) score = evalHelpAB(child, this->depth_ - 1, -2147483647, 2147483647);
//         else score = this->evalHelpMinimax(child, this->depth_-1);

//         // child.printBoard();
//         // std::cout << score << std::endl;

//         if(score > bestScore){
//             bestScore = score;
//             bestMove = child.lastMove();
//         }
//     }

//     return bestMove;
// }

// bool Minimax::setABPruning(){
//     this->doABPruining_ = !this->doABPruining_;
//     return this->doABPruining_;
// }