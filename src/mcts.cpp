#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "mcts.h"

using std::unordered_map;
using std::vector;
using std::pair;

MCTSNode::MCTSNode(){
    this->val = ChessBoard();
    this->parent = ChessBoard();
    this->children = vector<ChessBoard>();
    this->sims = 0;
    this->wins = 0;
}
MCTSNode::MCTSNode(ChessBoard val, ChessBoard parent){
    this->val = val;
    this->parent = parent;
    this->children = vector<ChessBoard>();
    this->sims = 0;
    this->wins = 0;

    for(ChessMove move : val.getPossibleMoves()){
        ChessBoard child = ChessBoard(val);
        child.doMove(move);
        this->children.push_back(child);
    }
}

double MCTSNode::calcUCT(double parentSims){
    return ((double)this->wins / (double)this->sims) + sqrt(2) * sqrt(log(parentSims) / (double)this->sims);
}

ChessBoard MCTS::select(ChessBoard root){
    double UCT;
    double highestVal = 0;
    ChessBoard best = root;
    for(ChessBoard child : this->tree_[root].children){
        if(this->tree_.count(child)){
            MCTSNode node = this->tree_[child];
            UCT = node.calcUCT(this->tree_[root].sims);
            if(UCT > highestVal) {
                best = node.val;
                highestVal = UCT;
            }
        }
    }

    if(best == root) return root;
    else return select(best);
}

//Create a new child
ChessBoard MCTS::expand(ChessBoard leaf) {
    for(ChessBoard child : this->tree_[leaf].children){
        if(!this->tree_.count(child)){
            this->tree_.insert(pair<ChessBoard,MCTSNode>(child,MCTSNode(child, leaf)));
            return child;
        }
    }
    return leaf;

}
//Select a random child, and do the simulation
int MCTS::simulate(ChessBoard leaf) {

    vector<ChessMove> moves;

    for(int i = 0; i < this->maxSimDepth_; i++){
        if(leaf.hasWon(kPlayerWhite)) return 1;
        else if(leaf.hasWon(kPlayerBlack)) return -1;
        moves = leaf.getPossibleMoves();
        leaf.doMove(moves[rand() % moves.size()]);
    }

    return 0;
}
void MCTS::backpropogate(ChessBoard leaf, ChessBoard root, int win){

    if((win == 1 && leaf.currentPlayer() == kPlayerWhite) || (win == -1 && leaf.currentPlayer() == kPlayerBlack)) this->tree_[leaf].wins++;
    this->tree_[leaf].sims++;
    
    if(!(leaf == root)) backpropogate(this->tree_[leaf].parent, root, win);
}

MCTS::MCTS(int times, int maxSimDepth){
    this->times_ = times;
    this->maxSimDepth_ = maxSimDepth;
}

ChessMove MCTS::findOptimalPath(ChessBoard board){

    srand(time(NULL));

    ChessBoard leaf;

    ChessMove retMove;
    int maxSims = 0;

    if(!this->tree_.count(board)){
        this->tree_.insert(pair<ChessBoard,MCTSNode>(board,MCTSNode(board,board)));
    }

    for(int x = 0; x < this->times_; x++){
        //std::cout << "Selecting..." << std::endl;
        ChessBoard leaf = this->select(board);
        //std::cout << "Expanding..." << std::endl;
        ChessBoard child = this->expand(leaf);
        //std::cout << "Simulating and doing backprop..." << std::endl;
        this->backpropogate(child, board, this->simulate(child));
        //std::cout << "Finished round " << x << std::endl;
    }

    //This does redundant work, but whatever
    for(ChessMove move : board.getPossibleMoves()){
        ChessBoard newBoard = ChessBoard(board);
        newBoard.doMove(move);
        if(this->tree_.count(newBoard) && this->tree_[newBoard].sims > maxSims){
            retMove = move;
            maxSims = this->tree_[newBoard].sims;
        }
    }
    return retMove;
}
