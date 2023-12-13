#ifndef STUPIDFISH_TYPES_H_
#define STUPIDFISH_TYPES_H_

#include <cstdint>
#include <cassert>

namespace StupidFish {

using uint = unsigned int;
using u8 = uint8_t;
using u16 = uint16_t;
using i8 = int8_t;
using u64 = uint64_t;

using Bitboard = u64;

constexpr uint kMaxMoves = 256;
constexpr uint kMaxPly = 246; // Got this from stockfish, need to see if it's right

// Taken from Stockfish
// bits 0-5 are destination
// bits 6-11 are origin
// bits 12-13 are promotion type
// bits 14-15 are special moves
// (0 = normal, 1 = promotion, 2 = en passant (holy hell), 3 = castling)
using Move = uint;

constexpr Move kMoveNone = 0;
constexpr Move kMoveNull = 65;

enum MoveType : Move {
    kNormal,
    kPromotion= 1 << 14,
    kEnPassant = 2 << 14,
    kCastling = 3 << 14
};

enum Color {
    kWhite,
    kBlack,
    kColorNum = 2
};

enum CastlingRights {
    kNoCastling,
    kWhiteKingAllowed,
    kWhiteQueenAllowed = kWhiteKingAllowed << 1,
    kBlackKingAllowed = kWhiteKingAllowed << 2,
    kBlackQueenAllowed = kWhiteKingAllowed << 3,
    kAnyKingAllowed = kWhiteKingAllowed | kBlackKingAllowed,
    kAnyQueenAllowed = kWhiteQueenAllowed | kBlackQueenAllowed,
    kAnyWhiteAllowed = kWhiteKingAllowed | kWhiteQueenAllowed,
    kAnyBlackAllowed = kBlackKingAllowed | kBlackQueenAllowed,
    kAnyCastlingAllowed = kAnyWhiteAllowed | kAnyBlackAllowed,
    kCastlingRightsNum = 16
};

enum PieceType {
    kNoPieceType,
    kPawn,
    kKnight,
    kBishop,
    kRook,
    kQueen,
    kKing,
    kAnyPieceType = 0,
    kPieceTypeNum = 8
};

enum Piece {
    kNoPiece,
    kWhitePawn = kPawn,
    kWhiteKnight,
    kWhiteBishop,
    kWhiteRook,
    kWhiteQueen,
    kWhiteKing,
    kBlackPawn = kPawn + 8,
    kBlackKnight,
    kBlackBishop,
    kBlackRook,
    kBlackQueen,
    kBlackKing,
    kPieceNum = 16
};

// It has to be backwards because going up = addition = moving down, if that makes sense
enum Square : int {
    kSquareA1, kSquareB1, kSquareC1, kSquareD1, kSquareE1, kSquareF1, kSquareG1, kSquareH1,
    kSquareA2, kSquareB2, kSquareC2, kSquareD2, kSquareE2, kSquareF2, kSquareG2, kSquareH2,
    kSquareA3, kSquareB3, kSquareC3, kSquareD3, kSquareE3, kSquareF3, kSquareG3, kSquareH3,
    kSquareA4, kSquareB4, kSquareC4, kSquareD4, kSquareE4, kSquareF4, kSquareG4, kSquareH4,
    kSquareA5, kSquareB5, kSquareC5, kSquareD5, kSquareE5, kSquareF5, kSquareG5, kSquareH5,
    kSquareA6, kSquareB6, kSquareC6, kSquareD6, kSquareE6, kSquareF6, kSquareG6, kSquareH6,
    kSquareA7, kSquareB7, kSquareC7, kSquareD7, kSquareE7, kSquareF7, kSquareG7, kSquareH7,
    kSquareA8, kSquareB8, kSquareC8, kSquareD8, kSquareE8, kSquareF8, kSquareG8, kSquareH8,
    kSquareNone,

    kSquareZero = 0,
    kSquareNum  = 64
};

enum Direction : int {
    kUp = 8,
    kRight = 1,
    kDown = -kUp,
    kLeft = -kRight,
    kUpLeft = kUp + kLeft,
    kUpRight = kUp + kRight,
    kDownLeft = kDown + kLeft,
    kDownRight = kDown + kRight
};

enum File : int {
    kFileA,
    kFileB,
    kFileC,
    kFileD,
    kFileE,
    kFileF,
    kFileG,
    kFileH,
    kFileNum
};

enum Rank : int {
    kRank1,
    kRank2,
    kRank3,
    kRank4,
    kRank5,
    kRank6,
    kRank7,
    kRank8,
    kRankNum
};

// I took this from stockfish, just a bunch of useful operator overloads
    #define ENABLE_BASE_OPERATORS_ON(T) \
        constexpr T operator+(T d1, int d2) { return T(int(d1) + d2); } \
        constexpr T operator-(T d1, int d2) { return T(int(d1) - d2); } \
        constexpr T operator-(T d) { return T(-int(d)); } \
        inline T&   operator+=(T& d1, int d2) { return d1 = d1 + d2; } \
        inline T&   operator-=(T& d1, int d2) { return d1 = d1 - d2; }

    #define ENABLE_INCR_OPERATORS_ON(T) \
        inline T& operator++(T& d) { return d = T(int(d) + 1); } \
        inline T& operator--(T& d) { return d = T(int(d) - 1); }

    #define ENABLE_FULL_OPERATORS_ON(T) \
        ENABLE_BASE_OPERATORS_ON(T) \
        constexpr T   operator*(int i, T d) { return T(i * int(d)); } \
        constexpr T   operator*(T d, int i) { return T(int(d) * i); } \
        constexpr T   operator/(T d, int i) { return T(int(d) / i); } \
        constexpr int operator/(T d1, T d2) { return int(d1) / int(d2); } \
        inline T&     operator*=(T& d, int i) { return d = T(int(d) * i); } \
        inline T&     operator/=(T& d, int i) { return d = T(int(d) / i); }

ENABLE_FULL_OPERATORS_ON(Direction)

ENABLE_INCR_OPERATORS_ON(PieceType)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)

    #undef ENABLE_FULL_OPERATORS_ON
    #undef ENABLE_INCR_OPERATORS_ON
    #undef ENABLE_BASE_OPERATORS_ON

// Most of these are things I took from stockfish, if I don't need them I'll get
// rid of them

constexpr Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
inline Square&   operator+=(Square& s, Direction d) { return s = s + d; }
inline Square&   operator-=(Square& s, Direction d) { return s = s - d; }

// Flip color
constexpr Color operator~(Color c) { return Color(c ^ kBlack); }

// Flip rank or file
constexpr Square FlipRank(Square s) { return Square(s ^ kSquareA8); }
constexpr Square FlipFile(Square s) { return Square(s ^ kSquareH1); }

// Flip color of piece
constexpr Piece operator~(Piece p) { return Piece(p ^ 8); }

// Get castling rights for a color
constexpr CastlingRights operator&(Color c, CastlingRights cr) {
    return CastlingRights((c == Color::kWhite ? CastlingRights::kAnyWhiteAllowed : CastlingRights::kAnyBlackAllowed) & cr);
}

// I'll implement these later
// constexpr Value mate_in(int ply) { return VALUE_MATE - ply; }
// constexpr Value mated_in(int ply) { return -VALUE_MATE + ply; }

constexpr Square MakeSquare(File f, Rank r) { return Square((r << 3) + f); }
constexpr Piece MakePiece(Color c, PieceType pt) { return Piece((c << 3) + pt); }

constexpr PieceType TypeOfPiece(Piece pc) { return PieceType(pc & 7); }
inline Color ColorOfPiece(Piece pc) {
    assert(pc != Piece::kNoPiece);
    return Color(pc >> 3);
}

constexpr bool ValidMove(Move m) { return m != kMoveNone && m != kMoveNull; }
constexpr bool ValidSquare(Square s) { return s >= kSquareA1 && s <= kSquareH8; }

constexpr File SquareFile(Square s) { return File(s & 7); }
constexpr Rank SquareRank(Square s) { return Rank(s >> 3); }
constexpr Square OppSquareOfSquare(Color c, Square s) { return Square(s ^ (c * 56)); }
constexpr Rank OppRankOfRank(Color c, Rank r) { return Rank(r ^ (c * 7)); }
constexpr Rank OppRankOfSquare(Color c, Square s) { return OppRankOfRank(c, SquareRank(s)); }

constexpr Direction GetPawnDir(Color c) { return c == Color::kWhite ? kUp : kDown; }

constexpr Square MoveSrc(Move m) {
    assert(ValidMove(m));
    return Square((m >> 6) & 0x3F);
}
constexpr Square MoveDest(Move m) {
    assert(ValidMove(m));
    return Square(m & 0x3F);
}
constexpr int MoveSrcAndDest(Move m) { return m & 0xFFF; }

constexpr MoveType TypeOfMove(Move m) { return MoveType(m & (3 << 14)); }

constexpr PieceType PromotionTypeOfMove(Move m) { return PieceType(((m >> 12) & 3) + kKnight); }

// constexpr Move MakeMoveSrcDest(Square src, Square dst) { return Move((src << 6) + dst); }
template<MoveType T>
constexpr Move MakeMove(Square src, Square dst, PieceType pt = kKnight, MoveType mt = kNormal) {
    return Move(T + ((pt - kKnight) << 12) + (src << 6) + dst);
}

// Based on a congruential pseudo-random number generator
// constexpr Key make_key(uint64_t seed) {
//     return seed * 6364136223846793005ULL + 1442695040888963407ULL;
// }

} // namespace ChessAI

#endif // STUPIDFISH_TYPES_H_