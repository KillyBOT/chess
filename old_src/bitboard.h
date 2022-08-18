#ifndef BITBOARD_H_
#define BITBOARD_H_

using BitBoard = unsigned long long;
using U64 = unsigned long long;

const BitBoard kOne = 1;
extern BitBoard kRankMasks[8];
extern BitBoard kFileMasks[8];
extern BitBoard kDiagMasks[64];
extern BitBoard kAntiDiagMasks[64];
extern BitBoard kPosMasks[64];
extern BitBoard kRayMasks[64][8];
extern BitBoard kKingAttackMasks[64];
extern BitBoard kKnightAttackMasks[64];
extern BitBoard kWhitePawnAttackMasks[64];
extern BitBoard kBlackPawnAttackMasks[64];

inline bool bit(const BitBoard &board, int ind) {
    return board & kPosMasks[ind];
}
inline void set_bit(BitBoard &board, int ind) {
    board |= kPosMasks[ind];
}
inline void reset_bit(BitBoard &board, int ind) {
    board &= ~kPosMasks[ind];
}

BitBoard flip_vertical(BitBoard board);
BitBoard flip_horizontal(BitBoard board);
int bitscan_forward(BitBoard board);
int bitscan_reverse(BitBoard board);

void print_bitboard(BitBoard board);

void init_masks();

#endif