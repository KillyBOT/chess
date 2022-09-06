#ifndef MINIMAX_H_
#define MINIMAX_H_

#include <unordered_map>

#include "chess_ai.h"
#include "board.h"

int heuristic_basic(ChessBoard &board);
int heuristic_complex(ChessBoard &board);

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

    int searchTime_;
    bool doABPruning_;
    bool doQuiescence_;
    Player maxPlayer_;
    int (*heuristicFunc_)(ChessBoard&);

    int evalBoard(ChessBoard &board);

    int evalHelpMinimax(ChessBoard &board, int depth, bool isQuiet);
    int evalHelpAB(ChessBoard &board, int depth, int alpha, int beta);
    int evalHelpQuiescence(ChessBoard &board, int alpha, int beta);

    public:
    
    Minimax(int(*heuristicFunc)(ChessBoard&) = heuristic_complex, int searchTime = 4096, bool doABPruning = true, bool doQuiescence = true);
    ChessMove findOptimalMove(ChessBoard &board);

    bool setABPruning();
};

#endif