#include <array>
#include <vector>
#include <iostream>

#include "ray.h"

std::array<std::array<std::vector<int>,8>,64> gRays;
std::array<std::vector<int>,64> gKnightPositionTable;
//N W S E NW SW SE NE
static const int kDeltaDir[8] = {8, -1, -8, 1, 7, -9, -7, 9};

void initRayTable() {
    int i;
    int distArray[8];
    for(int start = 0; start < 64; start++){
        distArray[kRayDirN] = 7 - (start >> 3);
        distArray[kRayDirW] = start & 0b111;
        distArray[kRayDirS] = start >> 3;
        distArray[kRayDirE] = 7 - (start & 0b111);
        distArray[kRayDirNW] = std::min(distArray[kRayDirN],distArray[kRayDirW]);
        distArray[kRayDirSW] = std::min(distArray[kRayDirS],distArray[kRayDirW]);
        distArray[kRayDirSE] = std::min(distArray[kRayDirS],distArray[kRayDirE]);
        distArray[kRayDirNE] = std::min(distArray[kRayDirN],distArray[kRayDirE]);

        //North
        for(int dir = 0; dir < 8; dir++){
            gRays[start][dir] = std::vector<int>();
            //std::cout << start << '\t' << dir << '\t' << distArray[dir] << std::endl;
            for(int i = 1; i <= distArray[dir]; i++){
                gRays[start][dir].push_back(start + i * kDeltaDir[dir]);
            }
        }
    }
}

void initKnightPositionTable() {
    int file, rank, tmp;
    int dRank = 1;
    int dFile = 2;
    for(int start = 0; start < 64; start++){
        gKnightPositionTable[start] = std::vector<int>();
        for(int rot = 0; rot < 4; rot++){
            file = start & 0b111;
            rank = start >> 3;
            //std::cout << rank + dRank << '\t' << file + dFile << std::endl;
            if(file + dFile >= 0 && file + dFile < 8 && rank + dRank >= 0 && rank + dRank < 8) gKnightPositionTable[start].push_back(((rank + dRank) << 3) + (file + dFile));
            if(file + dRank >= 0 && file + dRank < 8 && rank + dFile >= 0 && rank + dFile < 8) gKnightPositionTable[start].push_back(((rank + dFile) << 3) + (file + dRank));

            tmp = dRank;
            dRank = -dFile;
            dFile = tmp;
        }
    }
}