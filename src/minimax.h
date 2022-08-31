#ifndef MINIMAX_H_
#define MINIMAX_H_

#include <unordered_map>

#include "chess_ai.h"
#include "board.h"

int heuristic_basic(ChessBoard &board, Player maxPlayer);
int heuristic_complex(ChessBoard &board, Player maxPlayer);

using std::unordered_map;
using std::pair;
using std::vector;

struct TranspositionTableEntry {
    int val;
    int depth;

    TranspositionTableEntry(int val = 0, int depth = 0);
};

class Minimax : public ChessAI{

    unordered_map<size_t,int> boardScores_;
    unordered_map<size_t, TranspositionTableEntry> transpositionTable_;

    int depth_;
    bool doABPruining_;
    bool doQuiescence_;
    Player maxPlayer_;
    int (*heuristicFunc_)(ChessBoard&, Player);

    int evalBoard(ChessBoard &board);

    int evalHelpMinimax(ChessBoard &board, int depth);
    int evalHelpAB(ChessBoard &board, int depth, int alpha, int beta);
    int evalHelpQuiescence(ChessBoard &board, int alpha, int beta);

    public:
    
    Minimax(int(*heuristicFunc)(ChessBoard&, Player) = heuristic_complex, int depth = 5, bool doABPruning = true, bool doQuiescence = true);
    ChessMove findOptimalMove(ChessBoard &board);

    bool setABPruning();
};

#endif