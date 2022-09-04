#ifndef BITBOARD_H_
#define BITBOARD_H_

#include <string>

#include "pos.h"
#include "ray.h"

using U64 = unsigned long long;

const U64 kOne = 1;
const U64 kZero = 0;
// extern U64 kRankMasks[8];
// extern U64 kFileMasks[8];
// extern U64 kDiagMasks[64];
// extern U64 kAntiDiagMasks[64];
extern U64 kPosMasks[64];
extern U64 kPosResetMasks[64];
extern U64 kRayMasks[64][8];
extern U64 kRookAttackMasks[64];
extern U64 kBishopAttackMasks[64];
extern U64 kRookCheckMasks[64];
extern U64 kBishopCheckMasks[64];
extern U64 kKingAttackMasks[64];
extern U64 kKnightAttackMasks[64];
extern U64 kWhitePawnAttackMasks[64];
extern U64 kBlackPawnAttackMasks[64];

extern U64 kRookMagics[64];
extern U64 kBishopMagics[64];

extern U64 kRookAttacks[64][4096];
extern U64 kBishopAttacks[64][512];

extern int kMagicRookIndBits[64];
extern int kMagicBishopIndBits[64];

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
    if(bb == kZero) return -1;
    return __builtin_ffsll(bb) - 1;
    // unsigned int tmp;
    // bb  ^= bb - 1;
    // bb  = (int)bb ^ (int)(bb >> 32);
    // bb ^= 0x01C5FC81;
    // bb +=  bb >> 16;
    // bb -= (bb >> 8) + 51;
    // return kLSBTable [bb & 255]; // 0..63
}
inline int bitscan_forward_iter(U64 &bb){
    int ind = __builtin_ffsll(bb) - 1;
    bb &= bb - 1;
    return ind;
}
inline int bitscan_reverse(U64 bb) {
    if(bb == kZero) return -1;
    return 63 - __builtin_clzll(bb);
    // const U64 debruijn64 = 0x03f79d71b4cb0a89;
    // bb |= bb >> 1; 
    // bb |= bb >> 2;
    // bb |= bb >> 4;
    // bb |= bb >> 8;
    // bb |= bb >> 16;
    // bb |= bb >> 32;
    // return kMSBTable[(bb * debruijn64) >> 58];
}

inline U64 dir_attacks(ChessPos start, int dir, U64 occupied) {
    int first;
    switch(dir){
        case kRayDirN:
        case kRayDirE:
        case kRayDirNE:
        case kRayDirNW:
        first = bitscan_forward(occupied & kRayMasks[start][dir]);
        break;
        default:
        first = bitscan_reverse(occupied & kRayMasks[start][dir]);
        break;
    }
    if(first < 0) return kRayMasks[start][dir];
    else return kRayMasks[start][dir] & ~kRayMasks[first][dir];
}

U64 random_bitboard();
U64 random_bitboard_fewbits();

U64 blockers_from_perm(int perm, int bits, U64 mask);

inline int bitboard_ind(U64 blockers, U64 magic, int bits){
    return (blockers * magic) >> (64 - bits);
}

U64 find_magic(ChessPos pos, int bits, bool bishop);

void print_bitboard(U64 bb);

void init_masks();
void create_magic_databases();
void init_magic_databases();

#endif