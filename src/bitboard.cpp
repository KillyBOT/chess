#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>

#include "bitboard.h"
#include "ray.h"

// U64 kRankMasks[8];
// U64 kFileMasks[8];
// U64 kDiagMasks[64];
// U64 kAntiDiagMasks[64];
U64 kPosMasks[64];
U64 kPosResetMasks[64];
U64 kRayMasks[64][8];
U64 kRookCheckMasks[64];
U64 kBishopCheckMasks[64];
U64 kRookAttackMasks[64];
U64 kBishopAttackMasks[64];
U64 kKingAttackMasks[64];
U64 kKnightAttackMasks[64];
U64 kWhitePawnAttackMasks[64];
U64 kBlackPawnAttackMasks[64];

U64 kRookMagics[64] = {
    0xa8002c000108020ULL, 0x6c00049b0002001ULL, 0x100200010090040ULL, 0x2480041000800801ULL, 0x280028004000800ULL,
    0x900410008040022ULL, 0x280020001001080ULL, 0x2880002041000080ULL, 0xa000800080400034ULL, 0x4808020004000ULL,
    0x2290802004801000ULL, 0x411000d00100020ULL, 0x402800800040080ULL, 0xb000401004208ULL, 0x2409000100040200ULL,
    0x1002100004082ULL, 0x22878001e24000ULL, 0x1090810021004010ULL, 0x801030040200012ULL, 0x500808008001000ULL,
    0xa08018014000880ULL, 0x8000808004000200ULL, 0x201008080010200ULL, 0x801020000441091ULL, 0x800080204005ULL,
    0x1040200040100048ULL, 0x120200402082ULL, 0xd14880480100080ULL, 0x12040280080080ULL, 0x100040080020080ULL,
    0x9020010080800200ULL, 0x813241200148449ULL, 0x491604001800080ULL, 0x100401000402001ULL, 0x4820010021001040ULL,
    0x400402202000812ULL, 0x209009005000802ULL, 0x810800601800400ULL, 0x4301083214000150ULL, 0x204026458e001401ULL,
    0x40204000808000ULL, 0x8001008040010020ULL, 0x8410820820420010ULL, 0x1003001000090020ULL, 0x804040008008080ULL,
    0x12000810020004ULL, 0x1000100200040208ULL, 0x430000a044020001ULL, 0x280009023410300ULL, 0xe0100040002240ULL,
    0x200100401700ULL, 0x2244100408008080ULL, 0x8000400801980ULL, 0x2000810040200ULL, 0x8010100228810400ULL,
    0x2000009044210200ULL, 0x4080008040102101ULL, 0x40002080411d01ULL, 0x2005524060000901ULL, 0x502001008400422ULL,
    0x489a000810200402ULL, 0x1004400080a13ULL, 0x4000011008020084ULL, 0x26002114058042ULL
};
U64 kBishopMagics[64] = {
    0x89a1121896040240ULL, 0x2004844802002010ULL, 0x2068080051921000ULL, 0x62880a0220200808ULL, 0x4042004000000ULL,
    0x100822020200011ULL, 0xc00444222012000aULL, 0x28808801216001ULL, 0x400492088408100ULL, 0x201c401040c0084ULL,
    0x840800910a0010ULL, 0x82080240060ULL, 0x2000840504006000ULL, 0x30010c4108405004ULL, 0x1008005410080802ULL,
    0x8144042209100900ULL, 0x208081020014400ULL, 0x4800201208ca00ULL, 0xf18140408012008ULL, 0x1004002802102001ULL,
    0x841000820080811ULL, 0x40200200a42008ULL, 0x800054042000ULL, 0x88010400410c9000ULL, 0x520040470104290ULL,
    0x1004040051500081ULL, 0x2002081833080021ULL, 0x400c00c010142ULL, 0x941408200c002000ULL, 0x658810000806011ULL,
    0x188071040440a00ULL, 0x4800404002011c00ULL, 0x104442040404200ULL, 0x511080202091021ULL, 0x4022401120400ULL,
    0x80c0040400080120ULL, 0x8040010040820802ULL, 0x480810700020090ULL, 0x102008e00040242ULL, 0x809005202050100ULL,
    0x8002024220104080ULL, 0x431008804142000ULL, 0x19001802081400ULL, 0x200014208040080ULL, 0x3308082008200100ULL,
    0x41010500040c020ULL, 0x4012020c04210308ULL, 0x208220a202004080ULL, 0x111040120082000ULL, 0x6803040141280a00ULL,
    0x2101004202410000ULL, 0x8200000041108022ULL, 0x21082088000ULL, 0x2410204010040ULL, 0x40100400809000ULL,
    0x822088220820214ULL, 0x40808090012004ULL, 0x910224040218c9ULL, 0x402814422015008ULL, 0x90014004842410ULL,
    0x1000042304105ULL, 0x10008830412a00ULL, 0x2520081090008908ULL, 0x40102000a0a60140ULL,
};

int kMagicRookIndBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

int kMagicBishopIndBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

U64 kRookAttacks[64][4096];
U64 kBishopAttacks[64][512];

static const U64 kVert1 = 0x00FF00FF00FF00FF;
static const U64 kVert2 = 0x0000FFFF0000FFFF;
static const U64 kHor1 = 0x5555555555555555;
static const U64 kHor2 = 0x3333333333333333;
static const U64 kHor4 = 0x0f0f0f0f0f0f0f0f;

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

void print_bitboard(U64 board){
    for(int rank = 7; rank >= 0; rank--){
        for(int file = 0; file < 8; file++){
            std::cout << (bit(board, rank * 8 + file) ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
}

U64 flip_horizontal (U64 bb) {
   bb = ((bb >> 1) & kHor1) +  2*(bb & kHor1);
   bb = ((bb >> 2) & kHor2) +  4*(bb & kHor2);
   bb = ((bb >> 4) & kHor4) + 16*(bb & kHor4);
   return bb;
}

U64 random_bitboard() {
    U64 u1, u2, u3, u4;
    u1 = (U64)(rand()) & 0xFFFF; u2 = (U64)(rand()) & 0xFFFF;
    u3 = (U64)(rand()) & 0xFFFF; u4 = (U64)(rand()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}
U64 random_bitboard_fewbits() {
    return random_bitboard() & random_bitboard() & random_bitboard();
}

U64 rmask(ChessPos pos) {
  U64 result = 0ULL;
  ChessPos rank = pos_rank(pos), file = pos_file(pos), r, f;
  for(r = rank+1; r <= 6; r++) result |= (1ULL << (file + r*8));
  for(r = rank-1; r >= 1; r--) result |= (1ULL << (file + r*8));
  for(f = file+1; f <= 6; f++) result |= (1ULL << (f + rank*8));
  for(f = file-1; f >= 1; f--) result |= (1ULL << (f + rank*8));
  return result;
}

U64 bmask(ChessPos pos) {
  U64 result = 0ULL;
  ChessPos rank = pos_rank(pos), file = pos_file(pos), r, f;
  for(r=rank+1, f=file+1; r<=6 && f<=6; r++, f++) result |= (1ULL << (f + r*8));
  for(r=rank+1, f=file-1; r<=6 && f>=1; r++, f--) result |= (1ULL << (f + r*8));
  for(r=rank-1, f=file+1; r>=1 && f<=6; r--, f++) result |= (1ULL << (f + r*8));
  for(r=rank-1, f=file-1; r>=1 && f>=1; r--, f--) result |= (1ULL << (f + r*8));
  return result;
}

U64 blockers_from_perm(int perm, int bits, U64 mask) {
  int i, j, n;
  U64 result = 0ULL;
  n = 1;
  for(i = 0; i < bits; i++) {
    j = bitscan_forward_iter(mask);
    if(perm & n) result |= kPosMasks[j];
    n <<= 1;
  }
  return result;
}

U64 find_magic(ChessPos pos, int bits, bool bishop) {
    U64 mask, b[4096], a[4096], used[4096], magic;
    int i, j, k, n, p;
    bool fail;

    mask = bishop ? bmask(pos) : rmask(pos);
    n = __builtin_popcountll(mask);
    p = 1 << n;

    for(i = 0; i < p; i++) {
        b[i] = blockers_from_perm(i, n, mask);
        //print_bitboard(b[i]);
        //std::cout << i << std::endl;
        //print_bitboard(b[i]);
        if(bishop){
            for(int dir = 4; dir < 8; dir++) a[i] |= dir_attacks(pos, dir, b[i]);
        } else {
            for(int dir = 0; dir < 4; dir++) a[i] |= dir_attacks(pos, dir, b[i]);
        }
    }
    for(k = 0; k < 100000000; k++) {
        magic = random_bitboard_fewbits();
        if(__builtin_popcountll((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
        memset(used, 0, 4096 * sizeof(U64));
        for(i = 0, fail = false; !fail && i < p; i++) {
            j = bitboard_ind(b[i], magic, bits);
            if(used[j] == 0ULL) used[j] = a[i];
            else if(used[j] != a[i]) fail = true;
        }
        if(!fail) return magic;
    }

    //for(i = 0; i < 4096; i++) print_bitboard(used[i]);

    std::cout << "Failed!" << std::endl;
    return 0ULL;
}

//TODO: write all of these into a txt file and just load them that way
void init_masks() {
    for(int i = 0; i < 64; i++) {
        kPosMasks[i] = kOne << i;
        kPosResetMasks[i] = ~kPosMasks[i];
    }

    // kRankMasks[0] = (U64)0b11111111;
    // for(int i = 1; i < 8; i++) kRankMasks[i] = kRankMasks[i-1] << 8;

    // kFileMasks[0] = (U64)0b0000000100000001000000010000000100000001000000010000000100000001;
    // for(int i = 1; i < 8; i++) kFileMasks[i] = kFileMasks[i-1] << 1;

    for(ChessPos start = 0; start < 64; start++){
        // set_bit(kDiagMasks[start],start);
        // set_bit(kAntiDiagMasks[start],start);
        // for(int i = 0; i < kRaySizes[start][kRayDirNE]; i++) set_bit(kDiagMasks[start],kRays[start][kRayDirNE][i]);
        // for(int i = 0; i < kRaySizes[start][kRayDirSW]; i++) set_bit(kDiagMasks[start],kRays[start][kRayDirSW][i]);
        // for(int i = 0; i < kRaySizes[start][kRayDirNW]; i++) set_bit(kAntiDiagMasks[start],kRays[start][kRayDirNW][i]);
        // for(int i = 0; i < kRaySizes[start][kRayDirSE]; i++) set_bit(kAntiDiagMasks[start],kRays[start][kRayDirSE][i]);

        for(int dir = 0; dir < 8; dir++) {
            if(kRaySizes[start][dir]) set_bit(kKingAttackMasks[start],kRays[start][dir][0]);
            for(int i = 0; i < kRaySizes[start][dir]; i++) set_bit(kRayMasks[start][dir], kRays[start][dir][i]);
        }
        for(int dir = 0; dir < 4; dir++) kRookAttackMasks[start] |= kRayMasks[start][dir];
        for(int dir = 4; dir < 8; dir++) kBishopAttackMasks[start] |= kRayMasks[start][dir];

        kRookCheckMasks[start] = rmask(start);
        kBishopCheckMasks[start] = bmask(start);

        if(kRaySizes[start][kRayDirNW]) set_bit(kWhitePawnAttackMasks[start],kRays[start][kRayDirNW][0]);
        if(kRaySizes[start][kRayDirSW]) set_bit(kBlackPawnAttackMasks[start],kRays[start][kRayDirSW][0]);
        if(kRaySizes[start][kRayDirSE]) set_bit(kBlackPawnAttackMasks[start],kRays[start][kRayDirSE][0]);
        if(kRaySizes[start][kRayDirNE]) set_bit(kWhitePawnAttackMasks[start],kRays[start][kRayDirNE][0]);

        for(int i = 0; i < kKnightPositionTableSize[start]; i++) set_bit(kKnightAttackMasks[start],kKnightPositionTable[start][i]);
    }
}

void create_magic_databases() {
    U64 bishopMask, rookMask, blockers, bishopMagic, rookMagic;
    U64 bishopAttacks[64][512];
    U64 rookAttacks[64][4096];
    int i, j, k, n, p, ind;
    bool fail;

    memset(rookAttacks, 0, 4096 * 64 * sizeof(U64));
    memset(bishopAttacks, 0, 512 * 64 * sizeof(U64));

    std::cout << "Finding all blocker-attacker pairs..." << std::endl;

    for(ChessPos pos = 0; pos < 64; pos++){

        rookMagic = kRookMagics[pos];
        bishopMagic = kBishopMagics[pos];

        bishopMask = bmask(pos);
        rookMask = rmask(pos);

        n = kMagicRookIndBits[pos];
        p = 1 << n;
        for(i = 0; i < p; i++) {
            blockers = blockers_from_perm(i, n, rookMask);
            //print_bitboard(blockers);
            //std::cout << std::endl;
            ind = bitboard_ind(blockers, rookMagic, kMagicRookIndBits[pos]);
            for(int dir = 0; dir < 4; dir++) rookAttacks[pos][ind] |= dir_attacks(pos, dir, blockers);
            //print_bitboard(rookAttacks[pos][ind]);
            //std::cout << std::endl << std::endl;
        }

        n = kMagicBishopIndBits[pos];
        p = 1 << n;
        for(i = 0; i < p; i++) {
            blockers = blockers_from_perm(i, n, bishopMask);
            ind = bitboard_ind(blockers, bishopMagic, kMagicBishopIndBits[pos]);
            for(int dir = 4; dir < 8; dir++) bishopAttacks[pos][ind] |= dir_attacks(pos, dir, blockers);
            //print_bitboard(bishopAttacks[pos][ind]);
        }
    }

    std::cout << "All found! Writing to database..." << std::endl;

    std::ofstream rookDatabase("../data/rook_attack_data", std::ios::binary);
    std::ofstream bishopDatabase("../data/bishop_attack_data", std::ios::binary);

    rookDatabase.write((char*)rookAttacks, 4096 * 64 * sizeof(U64));
    bishopDatabase.write((char*)bishopAttacks, 512 * 64 * sizeof(U64));

    rookDatabase.close();
    bishopDatabase.close();

    std::cout << "Writing complete!" << std::endl;
}

void init_magic_databases() {
    std::ifstream rookDatabase("../data/rook_attack_data", std::ios::binary);
    std::ifstream bishopDatabase("../data/bishop_attack_data", std::ios::binary);

    rookDatabase.read((char*)kRookAttacks, 4096 * 64 * sizeof(U64));
    bishopDatabase.read((char*)kBishopAttacks, 512 * 64 * sizeof(U64));

    rookDatabase.close();
    bishopDatabase.close();
}