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
    double highestVal = -99999999;
    ChessMove best;
    double parentSims;

    if(this->nodes_.count(root.key())) parentSims = this->nodes_.at(root.key()).sims;
    //root.printBoard();

    for(ChessMove move : gMoveGenerator.getMoves(root)){
        ChessBoard newBoard(root);
        newBoard.doMove(move);
        //std::cout << root.zobristKey() << std::endl;
        //root.printBoard();

        if(this->nodes_.count(newBoard.key())){
            UCT = this->nodes_.at(newBoard.key()).calcUCT(parentSims);

            if(UCT > highestVal) {
                best = move;
                highestVal = UCT;
            }

        } else {
            //std::cout << root.zobristKey() << std::endl;
            //root.printBoard();
            //std::cout << root.zobristKey() << std::endl;
            //root.printBoard();
            return;
        }

    }

    root.doMove(best);
    select(root);
}

//Create a new child
size_t MCTS::expand(ChessBoard &leaf) {
    size_t parentKey = leaf.key();
    for(ChessMove move : gMoveGenerator.getMoves(leaf)){
        ChessBoard child(leaf);
        child.doMove(move);
        if(!this->nodes_.count(child.key())){
            //std::cout << leaf.zobristKey() << '\t' << parentKey << std::endl;
            leaf = child;
            this->nodes_.emplace(child.key(),MCTSNode(leaf.key(), leaf.player(), parentKey));
            return leaf.key();
        }
    }

    return parentKey;

}
//Select a random child, and do the simulation
int MCTS::simulate(ChessBoard &leaf) {

    //MoveGenerator gMoveGenerator;
    //gMoveGenerator.setBoard(leaf);

    srand(time(NULL));
    int val = 0;
    Player player = leaf.player();

    for(int i = 0; i < this->rolloutTimes_; i++){
        ChessBoard simulationBoard(leaf);
        while (true){
            //leaf.printBoard();
            //std::cout << leaf.pieceNum() << std::endl;
            if(simulationBoard.fiftyMoveStalemate()) break;
            vector<ChessMove> moves = gMoveGenerator.getMoves(simulationBoard);
            //for(const ChessMove &move : moves) std::cout << move.str() << std::endl;
            if(moves.empty()) {
                if(simulationBoard.inCheck(simulationBoard.player())){
                    if(simulationBoard.player() == player) val--;
                    else val++;
                }
                break;
            }
            simulationBoard.doMove(moves[rand() % moves.size()]);
            //leaf = this->getChildren(leaf)[rand() % this->getChildren(leaf).size()];
        }
    }
    
    return val;
}
void MCTS::backpropogate(size_t key, int reward, Player player){

    while(!this->nodes_.at(key).isRoot){
        //if(win == kPlayerNone) this->nodes_.at(key).wins++;
        if(player == this->nodes_.at(key).player) this->nodes_.at(key).wins += reward;
        this->nodes_.at(key).sims += this->rolloutTimes_;
        key = this->nodes_.at(key).parent;
    }

    if(player == this->nodes_.at(key).player) this->nodes_.at(key).wins += reward;
    this->nodes_.at(key).sims += this->rolloutTimes_;
}

MCTS::MCTS(int times, int rolloutTimes) : ChessAI("Monte Carlo Tree Search"){
    this->times_ = times;
    this->rolloutTimes_ = rolloutTimes;
}

ChessMove MCTS::findOptimalMove(ChessBoard &board){

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::system_clock;

    srand(time(NULL));

    ChessMove bestMove;
    double maxUCT = -99999999;
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
        Player player = root.player();
        int reward = this->simulate(root);
        //std::cout << "Doing backprop..." << std::endl;
        this->backpropogate(key, reward, player);
        //std::cout << "Finished round " << x+1 << std::endl;
    }

    double parentSims = this->nodes_.at(rootKey).sims;
    for(ChessMove move : gMoveGenerator.getMoves(board)){
        ChessBoard newBoard(board);
        newBoard.doMove(move);
        if(this->nodes_.count(newBoard.key())){
            double UCT = this->nodes_.at(newBoard.key()).calcUCT(parentSims);
            if(UCT > maxUCT){
                bestMove = move;
                maxUCT = UCT;
            }
        }
        
    }

    auto endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::cout << "Found move in [" << endTime - startTime << "] ms" << std::endl;

    return bestMove;
}
