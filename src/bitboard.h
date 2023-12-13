#ifndef CHESS_AI_BITBOARD_H_
#define CHESS_AI_BITBOARD_H_

#include <string>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "types.h"

namespace StupidFish {

constexpr Bitboard kFileABitboard = 0x0101010101010101ULL;
constexpr Bitboard kFileBitboard = kFileABitboard << 1;
constexpr Bitboard kFileCBitboard = kFileABitboard << 2;
constexpr Bitboard kFileDBitboard = kFileABitboard << 3;
constexpr Bitboard kFileEBitboard = kFileABitboard << 4;
constexpr Bitboard kFileFBitboard = kFileABitboard << 5;
constexpr Bitboard kFileGBitboard = kFileABitboard << 6;
constexpr Bitboard kFileHBitboard = kFileABitboard << 7;

constexpr Bitboard kRank1Bitboard = 0xFF;
constexpr Bitboard kRank2Bitboard = kRank1Bitboard << (8 * 1);
constexpr Bitboard kRank3Bitboard = kRank1Bitboard << (8 * 2);
constexpr Bitboard kRank4Bitboard = kRank1Bitboard << (8 * 3);
constexpr Bitboard kRank5Bitboard = kRank1Bitboard << (8 * 4);
constexpr Bitboard kRank6Bitboard = kRank1Bitboard << (8 * 5);
constexpr Bitboard kRank7Bitboard = kRank1Bitboard << (8 * 6);
constexpr Bitboard kRank8Bitboard = kRank1Bitboard << (8 * 7);

// Cached values for population count
extern u8 pop_counts_16[1 << 16];

// Maximum distance between two squares: either horizontal or vertical
extern u8 max_square_distances[kSquareNum][kSquareNum];

extern Bitboard forced_moves_bitboards[kSquareNum][kSquareNum];
extern Bitboard line_bitboards[kSquareNum][kSquareNum];
extern Bitboard attacks_bitboards[kPieceTypeNum][kSquareNum];
extern Bitboard pawn_attacks_bitboards[kColorNum][kSquareNum];

struct Magic {
    Bitboard mask;
    u64 magic;
    Bitboard* attacks;
    uint bits;

    // Calculate which attacks using magic number  
    inline uint index(Bitboard occupied) const {
        return uint(((occupied & mask) * magic) >> (64 - bits));
    }
};

extern Magic rook_magics[kSquareNum];
extern Magic bishop_magics[kSquareNum];

constexpr Bitboard SquareBitboard(Square sq) {
    assert(ValidSquare(sq));
    return (1ULL << sq);
}
constexpr Bitboard RankBitboard(Rank r) {
    return kRank1Bitboard << (r * 8);
}
constexpr Bitboard RankBitboard(Square sq) {
    return RankBitboard(SquareRank(sq));
}
constexpr Bitboard FileBitboard(File f) {
    return kFileABitboard << f;
}
constexpr Bitboard FileBitboard(Square sq) {
    return FileBitboard(SquareFile(sq));
}

// Clever trick to see if there's more than one piece on a given board
constexpr bool MoreThanOnePiece(Bitboard bb) { return bb & (bb - 1); }

constexpr Bitboard ShiftBitboard(Bitboard bb, Direction dir) {
    switch (dir)
    {
        case kUp:
        return bb << 8;
        case kDown:
        return bb >> 8;
        case kUp + kUp:
        return bb << 16;
        case kDown + kDown:
        return bb << 16;
        case kLeft:
        return (bb & ~kFileABitboard) >> 1;
        case kRight:
        return (bb & ~kFileHBitboard) << 1;
        case kUpLeft:
        return (bb & ~kFileABitboard) << 7;
        case kUpRight:
        return (bb & ~kFileHBitboard) << 9;
        case kDownLeft:
        return (bb & ~kFileABitboard) >> 9;
        case kDownRight:
        return (bb & ~kFileHBitboard) >> 7;
        default:
        return 0; 
    }
}

// TODO: I can maybe move these to bitboards.cpp
constexpr Bitboard MakePawnAttacksBitboard(Bitboard bb, Color c) {
    return c == kWhite ? ShiftBitboard(bb, kUpLeft) | ShiftBitboard(bb, kUpRight)
                        : ShiftBitboard(bb, kDownLeft) | ShiftBitboard(bb, kDownRight);
}
constexpr Bitboard MakePawnAttacksBitboard(Square sq, Color c) {
    return MakePawnAttacksBitboard(SquareBitboard(sq), c);
}

inline Bitboard pawn_attacks_bitboard(Square sq, Color c) {
    assert(ValidSquare(sq));
    return pawn_attacks_bitboards[c][sq];
}

// Return a line that intersects both s1 and s2. If one does not exist, return 0
inline Bitboard line_bitboard(Square s1, Square s2) {
    assert(ValidSquare(s1) && ValidSquare(s2));
    return line_bitboards[s1][s2];
}

// This is a clever way of speeding up the forced locations a piece must move if
// another piece is putting the player's king in check. If there is a ray
// between king_pos and attacker_pos, return the respective ray not including
// king_pos. Otherwise, return attacker_pos
inline Bitboard forced_moves_bitboard(Square king_pos, Square attacker_pos) {
    assert(ValidSquare(king_pos) && ValidSquare(attacker_pos));
    return forced_moves_bitboards[king_pos][attacker_pos];
}

// Used for a really annoying edge case with en passant
inline bool SquaresAreAligned(Square s1, Square s2, Square s3) {
    return line_bitboard(s1, s2) & s3;
}

inline int FileDistance(Square s1, Square s2) {
    return std::abs(SquareFile(s1) - SquareFile(s2));
}

inline int RankDistance(Square s1, Square s2) {
    return std::abs(SquareRank(s1) - SquareRank(s2));
}

inline int MaxSquareDistance(Square s1, Square s2) {
    return max_square_distances[s1][s2];
}

inline int FileDistanceFromEdge(File f) {
    return std::min(f, File(kFileH - f));
}

// Attacks for a piece at square sq with type pt
inline Bitboard attacks_bitboard(Square sq, PieceType pt, Bitboard occupied = 0ULL) {
    assert((pt != kPawn) && ValidSquare(sq));

    switch (pt) {
        case kBishop:
            return bishop_magics[sq].attacks[bishop_magics[sq].index(occupied)];
        case kRook:
            return rook_magics[sq].attacks[rook_magics[sq].index(occupied)];
        case kQueen:
            return attacks_bitboard(sq, kBishop, occupied) | attacks_bitboard(sq, kRook, occupied);
        default:
            return attacks_bitboards[pt][sq];
    }
}

// Return the popcount of the given bitboard
inline int popcount(Bitboard bb) {
    return __builtin_popcountll(bb);
}

// Get the least significant bit square of a nonzero bitboard
inline Square LSBSquare(Bitboard bb) {

    assert(bb);
    return Square(__builtin_ctzll(bb));

}

// Get the most significant bit square of a nonzero bitboard
inline Square MSBSquare(Bitboard bb) {

    assert(bb);

    return Square(63 ^ __builtin_clzll(bb));
}

// Get the least significant bit square of a nonzero bitboard
inline Bitboard LSBBitboard(Bitboard bb) {
    assert(bb);
    return bb & -bb;
}

// Remove the LSB square from a nonzero bitboard
inline Square RemoveLSBFromBitboard(Bitboard& bb) {
    assert(bb);
    Square sq = LSBSquare(bb);
    bb &= bb - 1;
    return sq;
}

// Initialize all the bitboards. Will also init the magics as well.
void InitBitboards();
// Return a bitboard as a string
std::string BitboardString(Bitboard bb);

} // namespace ChessAI

#endif // CHESS_AI_BITBOARD_H_