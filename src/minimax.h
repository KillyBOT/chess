#ifndef MINIMAX_H_
#define MINIMAX_H_

#include <unordered_map>

#include "chess_ai.h"
#include "board.h"

int heuristic_basic(ChessBoard &board, Player maxPlayer);

using std::unordered_map;
using std::pair;
using std::vector;

class Minimax : public ChessAI{

    unordered_map<ChessBoard,pair<int,int>,ChessBoardHash> boardScores_;

    int depth_;
    bool doABPruining_;
    Player maxPlayer_;
    int (*heuristicFunc_)(ChessBoard&, Player);

    int evalBoard(ChessBoard &board);

    int evalHelpMinimax(ChessBoard &board, int depth);
    int evalHelpAB(ChessBoard &board, int depth, int alpha, int beta);

    public:
    
    Minimax(int(*heuristicFunc)(ChessBoard&, Player) = heuristic_basic, int depth = 4, bool doABPruning = true);
    ChessMove findOptimalMove(ChessBoard board);

    bool setABPruning();
};

#endif