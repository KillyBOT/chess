#include <iostream>

#include "types.h"
#include "bitboard.h"

using namespace StupidFish;

int main()
{
    InitBitboards();

    std::cout << GenerateMagic(kSquareA1, kRook) << '\n';
}