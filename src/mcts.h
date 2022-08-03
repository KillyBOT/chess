#ifndef MCTS_H_
#define MCTS_H_

#include <unordered_map>
#include <vector>

#include "move.h"
#include "board.h"

using std::unordered_map;
using std::vector;

struct MCTSNode{
    ChessBoard val;
    vector<ChessBoard> children;
    ChessBoard parent;
    int wins, sims;

    MCTSNode();
    MCTSNode(ChessBoard val, ChessBoard parent);
    double calcUCT(double parentSims);
};
class MCTS{
    unordered_map<ChessBoard, MCTSNode, ChessBoardHash> tree_;
    int times_, maxSimDepth_;

    ChessBoard select(ChessBoard root);
    ChessBoard expand(ChessBoard leaf);
    int simulate(ChessBoard leaf);
    void backpropogate(ChessBoard leaf, ChessBoard root, int win);

    public:
    MCTS(int times = 100, int maxSimDepth = 50);
    ChessMove findOptimalPath(ChessBoard board);
};

#endif