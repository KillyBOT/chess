#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <ctime>

#include "mcts.h"
#include "move_generator.h"

using std::unordered_map;
using std::vector;
using std::pair;

double sqrt2 = sqrt(2);

MCTSNode::MCTSNode(ChessBoard val, ChessBoard parent){
    this->val = val;
    this->parent = parent;
    this->sims = 0;
    this->wins = 0;
    this->isRoot = false;
}

double MCTSNode::calcUCT(double parentSims){
    //std::cout << this->wins << ' ' << this->sims << ' ' << parentSims << std::endl;
    return ((double)this->wins / (double)this->sims) + sqrt2 * sqrt(log(parentSims) / (double)this->sims);
}

ChessBoard MCTS::select(ChessBoard &root){
    double UCT;
    double highestVal = -1;
    int best;
    for(int i = 0; i < this->getChildren(root).size(); i++){
        ChessBoard &child = this->getChildren(root)[i];
        if(this->nodes_.count(child)){
            UCT = this->nodes_[child].calcUCT(this->nodes_[root].sims);
            if(UCT > highestVal) {
                best = i;
                highestVal = UCT;
            }
        } else {
            return root;
        }
    }

    return select(this->getChildren(root)[best]);
}

//Create a new child
ChessBoard MCTS::expand(ChessBoard &leaf) {
    for(ChessBoard &child : this->getChildren(leaf)){
        if(!this->nodes_.count(child)){
            this->nodes_.insert(pair<ChessBoard,MCTSNode>(child,MCTSNode(child, leaf)));
            return child;
        }
    }
    return leaf;

}
//Select a random child, and do the simulation
Player MCTS::simulate(ChessBoard leaf) {

    vector<ChessMove> moves;
    MoveGenerator mg;

    srand(time(NULL));
    while (true){
        mg.setBoard(leaf);
        moves = mg.getMoves();
        if(moves.empty()) return leaf.opponent();
        else if(mg.stalemate()) return kPlayerNone;
        leaf.doMove(moves[rand() % moves.size()]);
        //leaf = this->getChildren(leaf)[rand() % this->getChildren(leaf).size()];
    }

    return kPlayerNone;
}
void MCTS::backpropogate(ChessBoard &leaf, Player win){

    if(win == kPlayerNone) this->nodes_.at(leaf).wins++;
    else if(win == leaf.player()) this->nodes_.at(leaf).wins += 2;
    this->nodes_.at(leaf).sims++;
    
    if(!this->nodes_.at(leaf).isRoot) backpropogate(this->nodes_.at(leaf).parent, win);
    
}

MCTS::MCTS(int times) : ChessAI("Monte Carlo Tree Search"){
    this->times_ = times;
}

ChessMove MCTS::findOptimalMove(ChessBoard board){

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    srand(time(NULL));

    ChessMove retMove;
    int maxSims = 0;

    // if(!this->nodes_.count(board)){
    //     this->nodes_.insert(pair<ChessBoard,MCTSNode>(board,MCTSNode(board,board)));
    // }
    this->root_ = board;
    this->nodes_.clear();
    this->nodes_.insert(pair<ChessBoard,MCTSNode>(this->root_,MCTSNode(this->root_, this->root_)));
    this->nodes_.at(this->root_).isRoot = true;

    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    for(int x = 0; x < this->times_; x++){
        //std::cout << "Selecting..." << std::endl;
        ChessBoard leaf = this->select(board);
        //std::cout << "Expanding..." << std::endl;
        ChessBoard child = this->expand(leaf);
        //std::cout << "Simulating..." << std::endl;
        Player winner = this->simulate(child);
        //std::cout << "Doing backprop..." << std::endl;
        this->backpropogate(child, winner);
        //std::cout << "Finished round " << x+1 << std::endl;
    }

    auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << endTime - startTime << std::endl;

    //This does redundant work, but whatever
    for(ChessBoard &child : this->getChildren(this->root_)){
        if(this->nodes_.count(child) && this->nodes_[child].sims > maxSims){
            retMove = child.lastMove();
            maxSims = this->nodes_[child].sims;
        }
    }
    return retMove;
}
