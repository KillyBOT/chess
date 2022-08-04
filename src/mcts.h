#ifndef MCTS_H_
#define MCTS_H_

#include <unordered_map>
#include <vector>

#include "move.h"
#include "board.h"
#include "chess_ai.h"

using std::unordered_map;
using std::vector;

struct MCTSNode{
    ChessBoard val;
    ChessBoard parent;
    int wins, sims;

    MCTSNode(ChessBoard val = ChessBoard(), ChessBoard parent = ChessBoard());
    double calcUCT(double parentSims);
};
class MCTS : public ChessAI{
    unordered_map<ChessBoard, MCTSNode, ChessBoardHash> tree_;
    int times_;

    ChessBoard select(ChessBoard root);
    ChessBoard expand(ChessBoard leaf);
    Player simulate(ChessBoard leaf);
    void backpropogate(ChessBoard leaf, ChessBoard root, Player win);

    public:
    MCTS(int times = 100);
    ChessMove findOptimalMove(ChessBoard board);
};

#endif