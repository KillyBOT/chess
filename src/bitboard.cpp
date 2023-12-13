#include "types.h"
#include "bitboard.h"

#include <bitset>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <type_traits>
#include <random>
#include <iostream>

namespace StupidFish {

u8 pop_counts_16[1 << 16];
u8 max_square_distances[kSquareNum][kSquareNum];

Bitboard forced_moves_bitboards[kSquareNum][kSquareNum];
Bitboard line_bitboards[kSquareNum][kSquareNum];
Bitboard attacks_bitboards[kPieceTypeNum][kSquareNum];
Bitboard pawn_attacks_bitboards[kColorNum][kSquareNum];

Magic rook_magics[kSquareNum];
Magic bishop_magics[kSquareNum];

std::string PrintBitboard(Bitboard bb) {
    const std::string hline = "+===+===+===+===+===+===+===+===+\n";
    std::string str = hline;

    for (Rank r = kRank8; r >= kRank1; --r)
    {
        for (File f = kFileA; f <= kFileH; ++f)
            str += bb & SquareBitboard(file_rank_square(f, r)) ? "| X " : "|   ";

        str += "| " + std::to_string(r + 1) + '\n' + hline;
    }
    str += " a   b   c   d   e   f   g   h\n";

    return str;
}

// File-specific functions and globals
namespace {

// constexpr int kMagicSearchIters = 100000000;

Bitboard rook_attacks[0x19000];
Bitboard bishop_attacks[0x1480];

std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<u64> dist(
    std::numeric_limits<u64>::min(),
    std::numeric_limits<u64>::max()
);

inline u64 GenerateRandomU64() {
    return dist(rng) & dist(rng) & dist(rng);
}

inline int MagicTransform(Bitboard bb, u64 magic, int bits)
{
    return int((bb * magic) >> (64 - bits));
}

inline Bitboard WithinBoard(Square src, int dir) {
    Square dest = Square(src + dir);
    return ValidSquare(dest) && MaxSquareDistance(src, dest) <= 2
            ? SquareBitboard(dest)
            : Bitboard(0ULL);
}

// Generate the sliding attacks (Rook and bishop attacks) from square sq for
// piece type pt. Used in the InitMagics
Bitboard GenerateSlidingAttacks(Square src, PieceType pt, Bitboard occupied = 0) {
    Bitboard sliding_attacks = 0;
    Direction rook_dirs[4] = {kUp, kLeft, kDown, kRight};
    Direction bishop_dirs[4] = {kUpLeft, kUpRight, kDownLeft, kDownRight};

    for (Direction dir : (pt == kRook ? rook_dirs : bishop_dirs))
    {
        Square sq = src;
        while (WithinBoard(sq, dir) && !(occupied & SquareBitboard(sq)))
        {
            sq += dir;
            sliding_attacks |= SquareBitboard(sq);
        }
    }

    return sliding_attacks;
}

// Initialize magics for a specific piece type
void InitMagics(PieceType pt, Bitboard attacks[], Magic magics[])
{
    Bitboard occupancy_permutation_bitboards[4096], attack_reference_bitboards[4096], border, bb;
    int epoch[4096], count = 0, permutations = 0;

    for (Square sq = kSquareA1; sq <= kSquareH8; ++sq)
    {
        border = 0;
        border |= (kRank1Bitboard | kRank8Bitboard) & ~RankBitboard(sq);
        border |= (kFileABitboard | kFileHBitboard) & ~FileBitboard(sq);

        // Initialize the magics for this square
        Magic& magic = magics[sq];
        
        // Set the magic's mask to the sliding attacks without worrying about
        // occupation. Also make sure to get rid of any redundant bits (i.e.
        // bits on the borders), since those will always be attacked regardless
        // of if a piece is there or not
        magic.mask = GenerateSlidingAttacks(sq, pt) & ~border;

        // This is the number of occupied spaces to check
        magic.bits = popcount(magic.mask);

        // All of our attacks tables will be next to each other in memory, so
        // ours should start exactly where the previous one ended.
        magic.attacks = (sq == kSquareA1) ? attacks : magics[sq - 1].attacks + permutations;

        // Get each possible permutation of occupancies for magic struct's mask
        
        bb = 0;
        permutations = 0;

        do
        {
            occupancy_permutation_bitboards[permutations] = bb;
            attack_reference_bitboards[permutations] = GenerateSlidingAttacks(sq, pt, bb);

            permutations++;
            // This uses a clever Carry-Rippler effect to iterate through all
            // the necessary bits
            bb = (bb - magic.mask) & magic.mask;
        }
        while (bb);

        // Find the magic number for this square, now that we have all the
        // necessary information
        for (int perm = 0; perm < permutations; )
        {
            // Firstly, keep generating random numbers until we find one that
            // meets the minumum criteria. In this case, the minimum criteria is
            // that, when the magic transform is perfomred on the base mask, the
            // resulting permutation should have at least 6 bits, since that's
            // the minimum amount of bits the mask can have
            for (magic.magic = 0; MagicTransform(magic.mask, magic.magic, 8) < 6;)
                magic.magic = GenerateRandomU64();

            for (++count, perm = 0; perm < permutations; perm++)
            {
                uint ind = magic.index(occupancy_permutation_bitboards[perm]);

                if (epoch[ind] < count)
                {
                    epoch[ind] = count;
                    magic.attacks[ind] = attack_reference_bitboards[ind];
                }
                else if (magic.attacks[ind] != attack_reference_bitboards[perm])
                    break;
            }
        }


    }
}

} // namespace

void InitBitboards() {

    // King and knight steps
    const int king_steps[8] = {kUp, kUpLeft, kLeft, kDownLeft, kDown,
                                kDownRight, kRight, kUpRight};
    const int knight_steps[8] = {kUpLeft + kUp, kUpLeft + kLeft,
                                kDownLeft + kLeft, kDownLeft + kDown,
                                kDownRight + kDown, kDownRight + kRight,
                                kUpRight + kRight, kUpRight + kUp};
    
    // Initialize pop counts cache
    for (uint i = 0; i < (1 << 16); i++)
        pop_counts_16[i] = u8(std::bitset<16>(i).count());
    
    // std::cout << "Popcount cache initialized\n";

    // Init maximum distances cache
    for (Square s1 = kSquareA1; s1 <= kSquareH8; ++s1)
        for (Square s2 = kSquareA1; s2 <= kSquareH8; ++s2)
            max_square_distances[s1][s2] = std::max(FileDistance(s1, s2), RankDistance(s2, s1));

    // std::cout << "Max distances cache initialized\n";

    // Init magic structs
    InitMagics(kRook, rook_attacks, rook_magics);
    // std::cout << "Rook magics initialized\n";
    InitMagics(kBishop, bishop_attacks, bishop_magics);
    // std::cout << "Bishop magics initialized\n";

    // Initialize attacks bitboards for all pieces
    for (Square src = kSquareA1; src <= kSquareH8; ++src)
    {
        // Initialize pawn attack bitboards
        pawn_attacks_bitboards[kWhite][src] = MakePawnAttacksBitboard(src, kWhite);
        pawn_attacks_bitboards[kBlack][src] = MakePawnAttacksBitboard(src, kBlack);
        // Generate king attack bitboards
        for (int step : king_steps)
            attacks_bitboards[kKing][src] = step;
        for (int step : knight_steps)
            attacks_bitboards[kKnight][src] = step;

        // Get the queen, bishop, and rook attack bitboards from the magics
        attacks_bitboards[kRook][src] = attacks_bitboard(src, kRook);
        attacks_bitboards[kBishop][src] = attacks_bitboard(src, kBishop);
        attacks_bitboards[kQueen][src] = attacks_bitboards[kRook][src] | attacks_bitboards[kBishop][src];

        // Generate lines and forced moves bitboards
        for (PieceType pt : {kRook, kBishop})
        {
            for (Square dest = kSquareA1; dest <= kSquareH8; ++dest)
            {
                if (attacks_bitboards[pt][src] & SquareBitboard(dest))
                {
                    // Can't I just use attacks_bitboards? Maybe magics have
                    // some things I need, idk I should look into it
                    line_bitboards[src][dest] =
                      (attacks_bitboard(src, pt) & attacks_bitboard(dest, pt))
                    | SquareBitboard(src)
                    | SquareBitboard(dest);

                    forced_moves_bitboards[src][dest] =
                          attacks_bitboard(src, pt, SquareBitboard(src))
                        & attacks_bitboard(dest, pt, SquareBitboard(dest));
                }
                forced_moves_bitboards[src][dest] |= SquareBitboard(dest);
            }
        }
    }

}

u64 GenerateMagic(Square src, PieceType pt)
{
    assert((pt == kBishop || pt == kRook) && ValidSquare(src));
    
    u64 magic;
    Bitboard occupancy_permutation_bitboards[4096], attack_reference_bitboards[4096], attacks[4096], mask, border, bb;
    int epoch[4096], count = 0, permutations = 0, bits;

    border = 0;
    border |= (kRank1Bitboard | kRank8Bitboard) & ~RankBitboard(src);
    border |= (kFileABitboard | kFileHBitboard) & ~FileBitboard(src);
    mask = GenerateSlidingAttacks(src, pt) & ~border;
    bits = popcount(mask);


    bb = 0;
    permutations = 0;

    do
    {
        occupancy_permutation_bitboards[permutations] = bb;
        attack_reference_bitboards[permutations] = GenerateSlidingAttacks(src, pt, bb);

        permutations++;
        // This uses a clever Carry-Rippler effect to iterate through all
        // the necessary bits
        bb = (bb - mask) & mask;
    }
    while (bb);

    for (int perm = 0; perm < permutations; )
    {
        // Firstly, keep generating random numbers until we find one that
        // meets the minumum criteria. In this case, the minimum criteria is
        // that, when the magic transform is perfomred on the base mask, the
        // resulting permutation should have at least 6 bits, since that's
        // the minimum amount of bits the mask can have
        for (magic = 0; MagicTransform(mask, magic, 8) < 6;)
            magic = GenerateRandomU64();

        for (++count, perm = 0; perm < permutations; perm++)
        {
            uint ind = MagicTransform(occupancy_permutation_bitboards[perm], magic, bits);

            if (epoch[ind] < count)
            {
                epoch[ind] = count;
                attacks[ind] = attack_reference_bitboards[ind];
            }
            else if (attacks[ind] != attack_reference_bitboards[perm])
                break;
        }
    }

    return magic;
}

} // namespace ChessAI