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
}

double MCTSNode::calcUCT(double parentSims){
    //std::cout << this->wins << ' ' << this->sims << ' ' << parentSims << std::endl;
    return ((double)this->wins / (double)this->sims) + sqrt2 * sqrt(log(parentSims) / (double)this->sims);
}

ChessBoard MCTS::select(ChessBoard root){
    double UCT;
    double highestVal = -1;
    ChessBoard best;
    for(ChessBoard child : this->getChildren(root)){
        if(this->tree_.count(child)){
            UCT = this->tree_[child].calcUCT(this->tree_[root].sims);
            if(UCT > highestVal) {
                best = child;
                highestVal = UCT;
            }
        } else {
            return root;
        }
    }

    return select(best);
}

//Create a new child
ChessBoard MCTS::expand(ChessBoard leaf) {
    for(ChessBoard child : this->getChildren(leaf)){
        if(!this->tree_.count(child)){
            this->tree_.insert(pair<ChessBoard,MCTSNode>(child,MCTSNode(child, leaf)));
            return child;
        }
    }
    return leaf;

}
//Select a random child, and do the simulation
Player MCTS::simulate(ChessBoard leaf) {

    vector<ChessMove> moves;
    MoveGenerator mg;

    while (true){
        mg.setBoard(leaf);
        if(mg.hasLost()) return leaf.opponent();
        else if(mg.stalemate()) return kPlayerNone;
        moves = mg.getMoves();
        leaf.doMove(moves[rand() % moves.size()]);
        //leaf = this->getChildren(leaf)[rand() % this->getChildren(leaf).size()];
    }

    return kPlayerNone;
}
void MCTS::backpropogate(ChessBoard leaf, ChessBoard root, Player win){

    while(leaf != this->tree_[leaf].parent){
        //leaf.printBoard();
        if(win == kPlayerNone) this->tree_[leaf].wins++;
        else if(leaf.player() == win) this->tree_[leaf].wins+=2;
        this->tree_[leaf].sims++;
        leaf = this->tree_[leaf].parent;
    }

    if(win == kPlayerNone) this->tree_[root].wins++;
    else if(leaf.player() == win) this->tree_[root].wins+=2;
    this->tree_[root].sims++;
    
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

    ChessBoard leaf;

    ChessMove retMove;
    int maxSims = 0;

    // if(!this->tree_.count(board)){
    //     this->tree_.insert(pair<ChessBoard,MCTSNode>(board,MCTSNode(board,board)));
    // }
    this->tree_.clear();
    this->tree_.insert(pair<ChessBoard,MCTSNode>(board,MCTSNode(board, board)));

    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    for(int x = 0; x < this->times_; x++){
        //std::cout << "Selecting..." << std::endl;
        ChessBoard leaf = this->select(board);
        //std::cout << "Expanding..." << std::endl;
        ChessBoard child = this->expand(leaf);
        //std::cout << "Simulating..." << std::endl;
        Player winner = this->simulate(child);
        //std::cout << "Doing backprop..." << std::endl;
        this->backpropogate(child, board, winner);
        //std::cout << "Finished round " << x+1 << std::endl;
    }

    auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << endTime - startTime << std::endl;

    //This does redundant work, but whatever
    MoveGenerator mg = MoveGenerator(board);
    for(ChessMove move : mg.getMoves()){
        ChessBoard newBoard = ChessBoard(board);
        newBoard.doMove(move);
        if(this->tree_.count(newBoard) && this->tree_[newBoard].sims > maxSims){
            retMove = move;
            maxSims = this->tree_[newBoard].sims;
        }
    }
    return retMove;
}
