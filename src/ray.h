#ifndef RAY_H_
#define RAY_H_

#include <array>
#include <vector>

const char kRayDirN = 0;
const char kRayDirW = 1;
const char kRayDirS = 2;
const char kRayDirE = 3;
const char kRayDirNW = 4;
const char kRayDirSW = 5;
const char kRayDirSE = 6;
const char kRayDirNE = 7;

extern std::array<std::array<std::vector<int>,8>,64> gRays;
extern std::array<std::vector<int>,64> gKnightPositionTable;

void initRayTable();
void initKnightPositionTable();

#endif