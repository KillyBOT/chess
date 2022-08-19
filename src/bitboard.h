#ifndef BITBOARD_H_
#define BITBOARD_H_

using U64 = unsigned long long;

const U64 kOne = 1;
// extern U64 kRankMasks[8];
// extern U64 kFileMasks[8];
// extern U64 kDiagMasks[64];
// extern U64 kAntiDiagMasks[64];
extern U64 kPosMasks[64];
extern U64 kPosResetMasks[64];
extern U64 kRayMasks[64][8];
extern U64 kKingAttackMasks[64];
extern U64 kKnightAttackMasks[64];
extern U64 kWhitePawnAttackMasks[64];
extern U64 kBlackPawnAttackMasks[64];

inline bool bit(const U64 &bb, int ind) {
    return bb & kPosMasks[ind];
}
inline void set_bit(U64 &bb, int ind) {
    bb |= kPosMasks[ind];
}
inline void reset_bit(U64 &bb, int ind) {
    bb &= kPosResetMasks[ind];
}

U64 flip_horizontal(U64 bb);

inline U64 flip_vertical(U64 bb){

    return __bswap_64(bb);
}
inline int bitscan_forward(U64 bb){
    if(bb == 0) return -1;
    else return __builtin_ffsll(bb) - 1;
    // unsigned int tmp;
    // bb  ^= bb - 1;
    // bb  = (int)bb ^ (int)(bb >> 32);
    // bb ^= 0x01C5FC81;
    // bb +=  bb >> 16;
    // bb -= (bb >> 8) + 51;
    // return kLSBTable [bb & 255]; // 0..63
}
inline int bitscan_forward_iter(U64 &bb){
    int ind = bitscan_forward(bb);
    if(ind != -1) reset_bit(bb, ind);
    return ind;
}
inline int bitscan_reverse(U64 bb) {
    if(bb == 0) return -1;
    else return 63 - __builtin_clzll(bb);
    // const U64 debruijn64 = 0x03f79d71b4cb0a89;
    // bb |= bb >> 1; 
    // bb |= bb >> 2;
    // bb |= bb >> 4;
    // bb |= bb >> 8;
    // bb |= bb >> 16;
    // bb |= bb >> 32;
    // return kMSBTable[(bb * debruijn64) >> 58];
}

void print_bitboard(U64 bb);

void init_masks();

#endif