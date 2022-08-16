#include <array>
#include <vector>
#include <iostream>

#include "bitboard.h"
#include "ray.h"
#include "pos.h"

ChessPos kRays[64][8][8];
ChessPos kKnightPositionTable[64][8];
int kRaySizes[64][8];
int kKnightPositionTableSize[64];

//N W S E NW SW SE NE
static const int kDeltaDir[8] = {8, -1, -8, 1, 7, -9, -7, 9};

void init_ray_table() {
    int i;
    int distArray[8];
    for(ChessPos start = 0; start < 64; start++){
        distArray[kRayDirN] = 7 - (start >> 3);
        distArray[kRayDirW] = start & 0b111;
        distArray[kRayDirS] = start >> 3;
        distArray[kRayDirE] = 7 - (start & 0b111);
        distArray[kRayDirNW] = std::min(distArray[kRayDirN],distArray[kRayDirW]);
        distArray[kRayDirSW] = std::min(distArray[kRayDirS],distArray[kRayDirW]);
        distArray[kRayDirSE] = std::min(distArray[kRayDirS],distArray[kRayDirE]);
        distArray[kRayDirNE] = std::min(distArray[kRayDirN],distArray[kRayDirE]);

        for(int dir = 0; dir < 8; dir++){
            //std::cout << start << '\t' << dir << '\t' << distArray[dir] << std::endl;
            for(int i = 1; i <= distArray[dir]; i++) kRays[start][dir][i-1] = start + i * kDeltaDir[dir];
            kRaySizes[start][dir] = distArray[dir];
        }
    }
}

void init_knight_position_table() {
    int file, rank, tmp;
    int dRank = 1;
    int dFile = 2;
    for(ChessPos start = 0; start < 64; start++){
        int size = 0;
        for(int rot = 0; rot < 4; rot++){
            file = start & 0b111;
            rank = start >> 3;
            //std::cout << rank + dRank << '\t' << file + dFile << std::endl;
            if(file + dFile >= 0 && file + dFile < 8 && rank + dRank >= 0 && rank + dRank < 8) {
                kKnightPositionTable[start][size] = ((rank + dRank) << 3) + (file + dFile);
                size++;
            }
            if(file + dRank >= 0 && file + dRank < 8 && rank + dFile >= 0 && rank + dFile < 8) {
                kKnightPositionTable[start][size] = ((rank + dFile) << 3) + (file + dRank);
                size++;
            }

            tmp = dRank;
            dRank = -dFile;
            dFile = tmp;
        }
        kKnightPositionTableSize[start] = size;
    }
}

char opposite_dir(char dir) {
    switch(dir){
        case kRayDirN:
        return kRayDirS;
        case kRayDirS:
        return kRayDirN;
        case kRayDirE:
        return kRayDirW;
        case kRayDirW:
        return kRayDirE;
        case kRayDirNE:
        return kRayDirSW;
        case kRayDirSW:
        return kRayDirNE;
        case kRayDirNW:
        return kRayDirSE;
        case kRayDirSE:
        return kRayDirNW;
    }
    return dir;
}