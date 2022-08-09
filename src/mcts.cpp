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
using std::size_t;

double sqrt2 = sqrt(2);

MCTSNode::MCTSNode(size_t val, Player player, size_t parent){
    this->val = val;
    this->parent = parent;
    this->player = player;
    this->sims = 0;
    this->wins = 0;
    this->isRoot = false;
}

double MCTSNode::calcUCT(double parentSims){
    //std::cout << this->wins << ' ' << this->sims << ' ' << parentSims << std::endl;
    return ((double)this->wins / (double)this->sims) + sqrt2 * sqrt(log(parentSims) / (double)this->sims);
}

void MCTS::select(ChessBoard &root){
    double UCT;
    double highestVal = -1;
    ChessMove best;
    for(ChessMove move : this->getMoves(root)){
        root.doMove(move);

        if(this->nodes_.count(root.zobristKey())){

            UCT = this->nodes_.at(root.zobristKey()).calcUCT(this->nodes_.at(root.zobristKey()).sims);

            if(UCT > highestVal) {
                best = move;
                highestVal = UCT;
            }

        } else {
            return;
        }

        root.undoLastMove();
    }

    root.doMove(best);
    select(root);
}

//Create a new child
size_t MCTS::expand(ChessBoard &leaf) {
    size_t parentKey = leaf.zobristKey();
    for(ChessMove move : this->getMoves(leaf)){
        leaf.doMove(move);
        if(!this->nodes_.count(leaf.zobristKey())){
            this->nodes_.emplace(leaf.zobristKey(),MCTSNode(leaf.zobristKey(), leaf.player(), parentKey));
            return leaf.zobristKey();
        }
        leaf.undoLastMove();
    }

    return parentKey;

}
//Select a random child, and do the simulation
Player MCTS::simulate(ChessBoard &leaf) {

    vector<ChessMove> moves;

    srand(time(NULL));
    while (true){
        moves = this->getMoves(leaf);
        if(moves.empty()) {
            MoveGenerator mg(leaf);
            if(mg.stalemate()) return kPlayerNone;
            else return leaf.opponent();
        }
        leaf.doMove(moves[rand() % moves.size()]);
        //leaf = this->getChildren(leaf)[rand() % this->getChildren(leaf).size()];
    }

    return kPlayerNone;
}
void MCTS::backpropogate(size_t key, Player win){

    while(!this->nodes_.at(key).isRoot){
        if(win == kPlayerNone) this->nodes_.at(key).wins++;
        else if(win == this->nodes_.at(key).player) this->nodes_.at(key).wins += 2;
        this->nodes_.at(key).sims++;
        key = this->nodes_.at(key).parent;
    }
    if(win == kPlayerNone) this->nodes_.at(key).wins++;
    else if(win == this->nodes_.at(key).player) this->nodes_.at(key).wins += 2;
    this->nodes_.at(key).sims++;
}

MCTS::MCTS(int times) : ChessAI("Monte Carlo Tree Search"){
    this->times_ = times;
}

ChessMove MCTS::findOptimalMove(ChessBoard &board){

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    srand(time(NULL));

    ChessMove bestMove;
    int maxSims = 0;
    size_t rootKey = board.zobristKey();

    // if(!this->nodes_.count(board)){
    //     this->nodes_.insert(pair<ChessBoard,MCTSNode>(board,MCTSNode(board,board)));
    // }

    this->nodes_.clear();
    this->nodes_.emplace(rootKey, MCTSNode(rootKey, board.player(), rootKey));
    this->nodes_.at(rootKey).isRoot = true;

    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    for(int x = 0; x < this->times_; x++){
        ChessBoard root(board);
        std::cout << "Selecting..." << std::endl;
        this->select(root);
        std::cout << "Expanding..." << std::endl;
        size_t key = this->expand(root);
        std::cout << "Simulating..." << std::endl;
        Player winner = this->simulate(board);
        std::cout << "Doing backprop..." << std::endl;
        this->backpropogate(key, winner);
        //std::cout << "Finished round " << x+1 << std::endl;
    }

    auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << endTime - startTime << std::endl;

    for(ChessMove move : this->getMoves(board)){
        board.doMove(move);
        if(this->nodes_.count(board.zobristKey()) && this->nodes_.at(board.zobristKey()).sims > maxSims){
            bestMove = move;
            maxSims = this->nodes_[board.zobristKey()].sims;
        }
        board.undoLastMove();
    }
    return bestMove;
}
