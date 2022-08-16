#include <iostream>
#include <sys/types.h>

#include "bitboard.h"
#include "ray.h"

BitBoard kRankMasks[8];
BitBoard kFileMasks[8];
BitBoard kDiagMasks[64];
BitBoard kAntiDiagMasks[64];
BitBoard kPosMasks[64];
BitBoard kKingAttackMasks[64];
BitBoard kKnightAttackMasks[64];
BitBoard kWhitePawnAttackMasks[64];
BitBoard kBlackPawnAttackMasks[64];


static const BitBoard kVert1 = 0x00FF00FF00FF00FF;
static const BitBoard kVert2 = 0x0000FFFF0000FFFF;
static const BitBoard kHor1 = 0x5555555555555555;
static const BitBoard kHor2 = 0x3333333333333333;
static const BitBoard kHor4 = 0x0f0f0f0f0f0f0f0f;

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

        for(int dir = 0; dir < 8; dir++) if(kRaySizes[start][dir]) set_bit(kKingAttackMasks[start],kRays[start][dir][0]);

        if(kRaySizes[start][kRayDirNW]) set_bit(kWhitePawnAttackMasks[start],kRays[start][kRayDirNW][0]);
        if(kRaySizes[start][kRayDirSW]) set_bit(kBlackPawnAttackMasks[start],kRays[start][kRayDirSW][0]);
        if(kRaySizes[start][kRayDirSE]) set_bit(kBlackPawnAttackMasks[start],kRays[start][kRayDirSE][0]);
        if(kRaySizes[start][kRayDirNE]) set_bit(kWhitePawnAttackMasks[start],kRays[start][kRayDirNE][0]);

        for(int i = 0; i < kKnightPositionTableSize[start]; i++) set_bit(kKnightAttackMasks[start],kKnightPositionTable[start][i]);
    }
    
}