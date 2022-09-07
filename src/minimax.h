#ifndef MINIMAX_H_
#define MINIMAX_H_

#include <unordered_map>

#include "chess_ai.h"
#include "board.h"

int heuristic_basic(ChessBoard &board);
int heuristic_complex(ChessBoard &board);

const int kExactEntry = 0;
const int kLowerEntry = 1;
const int kUpperEntry = 2;

using std::unordered_map;
using std::pair;
using std::vector;

struct TranspositionTableEntry {
    int val, depth, entryType;
    bool isValid;
    ChessMove bestMove;

    TranspositionTableEntry(ChessMove bestMove = ChessMove(), int val = 0, int depth = 0, int entryType = kExactEntry);
};

class TranspositionTable {
    unordered_map<size_t, TranspositionTableEntry> tt_;

    public:
    TranspositionTable();

    TranspositionTableEntry getTTEntry(ChessBoard &board, int depth, int alpha, int beta) const;
    int size() const;

    void addTTEntry(ChessBoard &board, ChessMove bestMove = ChessMove(), int val = 0, int depth = 0, int entryType = kExactEntry);
};

class Minimax : public ChessAI{

    unordered_map<size_t,int> boardScores_;
    TranspositionTable tt_;

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