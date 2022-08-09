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
    size_t val;
    size_t parent;
    Player player;
    int wins, sims;
    bool isRoot;

    MCTSNode(size_t val = 0, Player player = kPlayerWhite, size_t parent = 0);
    double calcUCT(double parentSims);
};
class MCTS : public ChessAI{
    unordered_map<size_t, MCTSNode> nodes_;
    int times_;

    void select(ChessBoard &root);
    size_t expand(ChessBoard &leaf);
    Player simulate(ChessBoard &leaf);
    void backpropogate(size_t key, Player win);

    public:
    MCTS(int times = 100);
    ChessMove findOptimalMove(ChessBoard &board);
};

#endif