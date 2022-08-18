#include <iostream>
#include <sys/types.h>

#include "bitboard.h"
#include "ray.h"

BitBoard kRankMasks[8];
BitBoard kFileMasks[8];
BitBoard kDiagMasks[64];
BitBoard kAntiDiagMasks[64];
BitBoard kPosMasks[64];
BitBoard kRayMasks[64][8];
BitBoard kKingAttackMasks[64];
BitBoard kKnightAttackMasks[64];
BitBoard kWhitePawnAttackMasks[64];
BitBoard kBlackPawnAttackMasks[64];

static const BitBoard kVert1 = 0x00FF00FF00FF00FF;
static const BitBoard kVert2 = 0x0000FFFF0000FFFF;
static const BitBoard kHor1 = 0x5555555555555555;
static const BitBoard kHor2 = 0x3333333333333333;
static const BitBoard kHor4 = 0x0f0f0f0f0f0f0f0f;

//This is the Walter Faxon magic bitscan, taken directly from the chess programming wiki
static const char kLSBTable[154] =
{
#define __ 0
   22,__,__,__,30,__,__,38,18,__, 16,15,17,__,46, 9,19, 8, 7,10,
   0, 63, 1,56,55,57, 2,11,__,58, __,__,20,__, 3,__,__,59,__,__,
   __,__,__,12,__,__,__,__,__,__, 4,__,__,60,__,__,__,__,__,__,
   __,__,__,__,21,__,__,__,29,__, __,37,__,__,__,13,__,__,45,__,
   __,__, 5,__,__,61,__,__,__,53, __,__,__,__,__,__,__,__,__,__,
   28,__,__,36,__,__,__,__,__,__, 44,__,__,__,__,__,27,__,__,35,
   __,52,__,__,26,__,43,34,25,23, 24,33,31,32,42,39,40,51,41,14,
   __,49,47,48,__,50, 6,__,__,62, __,__,__,54
#undef __
};

static const char kMSBTable[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

void print_bitboard(BitBoard board){
    for(int rank = 7; rank >= 0; rank--){
        for(int file = 0; file < 8; file++){
            std::cout << (bit(board, rank * 8 + file) ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
}

BitBoard flip_vertical(BitBoard board){
    //print_bitboard(board);
    //std::cout << std::endl;
    return __bswap_64(board);
    // board = ((board >>  8) & kVert1) | ((board & kVert1) <<  8);
    // board = ((board >> 16) & kVert2) | ((board & kVert2) << 16);
    // board = ( board >> 32) | ( board << 32);
    // //print_bitboard(board);
    // //std::cout << std::endl << std::endl;
    // return board;
}
BitBoard flip_horizontal (BitBoard board) {
   board = ((board >> 1) & kHor1) +  2*(board & kHor1);
   board = ((board >> 2) & kHor2) +  4*(board & kHor2);
   board = ((board >> 4) & kHor4) + 16*(board & kHor4);
   return board;
}
int bitscan_forward(BitBoard board){
    //return __builtin_ffsll(board);
    unsigned int tmp;
    board  ^= board - 1;
    board  = (int)board ^ (int)(board >> 32);
    board ^= 0x01C5FC81;
    board +=  board >> 16;
    board -= (board >> 8) + 51;
    return kLSBTable [board & 255]; // 0..63
}
int bitscan_reverse(BitBoard board) {
   const BitBoard debruijn64 = 0x03f79d71b4cb0a89;
   board |= board >> 1; 
   board |= board >> 2;
   board |= board >> 4;
   board |= board >> 8;
   board |= board >> 16;
   board |= board >> 32;
   return kMSBTable[(board * debruijn64) >> 58];
}

//TODO: write all of these into a txt file and just load them that way
void init_masks() {
    for(int i = 0; i < 64; i++) kPosMasks[i] = kOne << i;

    kRankMasks[0] = (BitBoard)0b11111111;
    for(int i = 1; i < 8; i++) kRankMasks[i] = kRankMasks[i-1] << 8;

    kFileMasks[0] = (BitBoard)0b0000000100000001000000010000000100000001000000010000000100000001;
    for(int i = 1; i < 8; i++) kFileMasks[i] = kFileMasks[i-1] << 1;

    for(ChessPos start = 0; start < 64; start++){
        set_bit(kDiagMasks[start],start);
        set_bit(kAntiDiagMasks[start],start);
        for(int i = 0; i < kRaySizes[start][kRayDirNE]; i++) set_bit(kDiagMasks[start],kRays[start][kRayDirNE][i]);
        for(int i = 0; i < kRaySizes[start][kRayDirSW]; i++) set_bit(kDiagMasks[start],kRays[start][kRayDirSW][i]);
        for(int i = 0; i < kRaySizes[start][kRayDirNW]; i++) set_bit(kAntiDiagMasks[start],kRays[start][kRayDirNW][i]);
        for(int i = 0; i < kRaySizes[start][kRayDirSE]; i++) set_bit(kAntiDiagMasks[start],kRays[start][kRayDirSE][i]);

        for(int dir = 0; dir < 8; dir++) {
            if(kRaySizes[start][dir]) set_bit(kKingAttackMasks[start],kRays[start][dir][0]);
            for(int i = 0; i < kRaySizes[start][dir]; i++) set_bit(kRayMasks[start][dir], kRays[start][dir][i]);
        }

        if(kRaySizes[start][kRayDirNW]) set_bit(kWhitePawnAttackMasks[start],kRays[start][kRayDirNW][0]);
        if(kRaySizes[start][kRayDirSW]) set_bit(kBlackPawnAttackMasks[start],kRays[start][kRayDirSW][0]);
        if(kRaySizes[start][kRayDirSE]) set_bit(kBlackPawnAttackMasks[start],kRays[start][kRayDirSE][0]);
        if(kRaySizes[start][kRayDirNE]) set_bit(kWhitePawnAttackMasks[start],kRays[start][kRayDirNE][0]);

        for(int i = 0; i < kKnightPositionTableSize[start]; i++) set_bit(kKnightAttackMasks[start],kKnightPositionTable[start][i]);
    }
}