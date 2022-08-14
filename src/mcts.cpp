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

MoveGenerator mg;

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
    double parentSims;

    if(this->nodes_.count(root.key())) parentSims = this->nodes_.at(root.key()).sims;
    //root.printBoard();

    for(ChessMove move : mg.getMoves(root)){
        //std::cout << root.zobristKey() << std::endl;
        //root.printBoard();
        root.doMove(move);

        if(this->nodes_.count(root.key())){
            UCT = this->nodes_.at(root.key()).calcUCT(parentSims);

            if(UCT > highestVal) {
                best = move;
                highestVal = UCT;
            }

        } else {
            //std::cout << root.zobristKey() << std::endl;
            //root.printBoard();
            root.undoLastMove();
            //std::cout << root.zobristKey() << std::endl;
            //root.printBoard();
            return;
        }

        root.undoLastMove();
    }

    root.doMove(best);
    select(root);
}

//Create a new child
size_t MCTS::expand(ChessBoard &leaf) {
    size_t parentKey = leaf.key();
    for(ChessMove move : mg.getMoves(leaf)){
        leaf.doMove(move);
        if(!this->nodes_.count(leaf.key())){
            //std::cout << leaf.zobristKey() << '\t' << parentKey << std::endl;
            //leaf.printBoard();
            this->nodes_.emplace(leaf.key(),MCTSNode(leaf.key(), leaf.player(), parentKey));
            return leaf.key();
        }
        leaf.undoLastMove();
    }

    return parentKey;

}
//Select a random child, and do the simulation
Player MCTS::simulate(ChessBoard &leaf) {

    //MoveGenerator mg;
    //mg.setBoard(leaf);

    srand(time(NULL));
    while (true){
        //leaf.printBoard();
        mg.setBoard(leaf);
        //std::cout << leaf.pieceNum() << std::endl;
        if(mg.fiftyMoveRuleStalemate(leaf)) return 2;
        const vector<ChessMove> &moves = mg.getMoves();
        //for(const ChessMove &move : moves) std::cout << move.str() << std::endl;
        if(moves.empty()) {
            if(mg.inCheck()) return leaf.opponent();
            else return 2;
        }
        leaf.doMove(moves[rand() % moves.size()]);
        //leaf = this->getChildren(leaf)[rand() % this->getChildren(leaf).size()];
    }

    return 2;
}
void MCTS::backpropogate(size_t key, Player win){

    while(!this->nodes_.at(key).isRoot){
        //if(win == kPlayerNone) this->nodes_.at(key).wins++;
        if(win == this->nodes_.at(key).player) this->nodes_.at(key).wins ++;
        this->nodes_.at(key).sims++;
        key = this->nodes_.at(key).parent;
    }
    if(win == this->nodes_.at(key).player) this->nodes_.at(key).wins ++;
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
    size_t rootKey = board.key();

    // if(!this->nodes_.count(board)){
    //     this->nodes_.insert(pair<ChessBoard,MCTSNode>(board,MCTSNode(board,board)));
    // }
    this->nodes_.clear();
    this->nodes_.emplace(rootKey, MCTSNode(rootKey, board.player(), rootKey));
    this->nodes_.at(rootKey).isRoot = true;

    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

    for(int x = 0; x < this->times_; x++){
        ChessBoard root(board);
        //std::cout << "Selecting..." << std::endl;
        this->select(root);
        //std::cout << "Expanding..." << std::endl;
        size_t key = this->expand(root);
        //std::cout << "Simulating..." << std::endl;
        Player winner = this->simulate(root);
        //std::cout << "Doing backprop..." << std::endl;
        this->backpropogate(key, winner);
        //std::cout << "Finished round " << x+1 << std::endl;
    }

    for(ChessMove move : mg.getMoves(board)){
        board.doMove(move);
        if(this->nodes_.count(board.key()) && this->nodes_.at(board.key()).sims > maxSims){
            bestMove = move;
            maxSims = this->nodes_[board.key()].sims;
        }
        board.undoLastMove();
    }

    auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << "Found move in [" << endTime - startTime << "] ms" << std::endl;

    return bestMove;
}
