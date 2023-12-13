#ifndef STUPIDFISH_POSITION_H_
#define STUPIDFISH_POSITION_H_

#include "types.h"
#include "bitboard.h"

#include <cassert>
#include <string>

namespace StupidFish {

class Position {
    // A simple array of Piece-s that tells you the piece at a given square.
    // This is not used for move generation because it would be too slow.
    Piece board_pieces_[kSquareNum];

    // Bitboards for each piece type
    Bitboard piece_bitboards_[kPieceTypeNum];

    // Bitboards for each color
    Bitboard color_bitboards_[kColorNum];

    // The number of each piece on the board
    int piece_nums_[kPieceNum];



    //
    int castling_rights_masks_[kSquareNum];
    // 
    Square castling_rook_squares_[kCastlingRightsNum];
    // 
    Bitboard castling_paths_[kCastlingRightsNum];



    // Number of plys in the game is
    int plies_;
    // The current player
    Color player_;

    // TODO: these following in Stockfish are in a separate StateInfo struct,
    // I know I'll need to do this later since I did it before, but for now
    // we can just leave these here.
    int castling_rights_;
    Square en_passant_square_;

    // This info is kept in the StateInfo struct, but apparently it's not
    // copied, so idk...
    Bitboard checkers_;
    Bitboard meat_shields_[kColorNum];
    Bitboard pinners_[kColorNum];
    Bitboard checked_[kPieceTypeNum];

    // Helper functions
    void SetCastlingRight_(Color c, Square rfrom);
    void SetCheckInfo_();
    
    void DoCastling_(Color c, Square k_src, Square k_dest, Square r_src, Square r_dest);
    void MovePiece_(Square dest, Square src);

    public:

    Position() = default;
    Position(const Position&) = delete;
    Position& operator=(const Position&) = delete;

    // Set based on FEN string
    Position& SetFromFen(const std::string& fen);
    // Create Fen string
    std::string GetFenStr() const;

    // Piece related functions
    Bitboard pieces(PieceType pt = kAnyPieceType) const;
    Bitboard pieces(Color c, PieceType = kAnyPieceType) const;
    Piece PieceOnSquare(Square sq) const;
    Square en_passant_square() const;
    bool IsSquareEmpty(Square sq) const;
    int piece_num(Color c, PieceType = kAnyPieceType) const;
    int piece_num(PieceType pt) const;
    Square GetPiecePosition(Color c, PieceType pt) const; // There can be only one of the piece, obviously

    CastlingRights castling_rights(Color c) const;
    bool CanCastle(CastlingRights cr) const;
    bool CannotCastle(CastlingRights cr) const;
    Square CastlingRookSquare(CastlingRights cr) const;

    // Checking
    Bitboard checkers() const;
    Bitboard meat_shields(Color c) const;
    Bitboard checked(PieceType pt) const;
    Bitboard pinners(Color c) const;

    // Attacking
    Bitboard AttackersOfSquare(Square sq, Bitboard occupied = 0) const;
    void UpdateSliderBlockers(Color c) const;
    Bitboard AttacksByPlayer(Color c, PieceType pt = kAnyPieceType) const;

    // Placing pieces
    void PutPiece(Square sq, Piece p);
    void RemovePiece(Square sq);

    // Move properties
    bool IsMoveLegal(Move m) const;
    bool IsMovePseudoLegal(Move m) const;
    bool DoesMoveCapture(Move m) const;
    // bool IsMoveCaptureStage(Move m) const; // Look into this one
    bool DoesMoveGiveCheck(Move m) const;
    Piece PieceMovedByMove(Move m) const;
    // Piece PieceCapturedByMove(Move m) const;

    // The actual move functions
    void DoMove(Move m, bool gives_check = false);
    void UntoMove(Move m);
    void DoNullMove();
    void UndoNullMove();

    Color SideToMove() const;
    int Plies() const;
};

inline Color Position::SideToMove() const {
    return player_;
}

inline Piece Position::PieceOnSquare(Square sq) const {
    assert(ValidSquare(sq));
    return board_pieces_[sq];
}

inline bool Position::IsSquareEmpty(Square sq) const {
    return PieceOnSquare(sq) == kNoPiece;
}

inline Bitboard Position::pieces(PieceType pt) const {
    return piece_bitboards_[pt];
}

inline Bitboard Position::pieces(Color c, PieceType pt) const {
    return color_bitboards_[c] & piece_bitboards_[pt];
}

inline int Position::piece_num(Color c, PieceType pt) const {
    return piece_nums_[MakePiece(c, pt)];
}

inline int Position::piece_num(PieceType pt) const {
    return piece_num(kWhite, pt) + piece_num(kBlack, pt);
}

inline Square Position::GetPiecePosition(Color c, PieceType pt) const {
    assert(piece_num(c, pt) == 1);
    return LSBSquare(pieces(c, pt));
}

inline Square Position::en_passant_square() const {
    return en_passant_square_;
}



inline bool Position::CanCastle(CastlingRights cr) const {
    return castling_rights_ & cr; 
}

inline CastlingRights Position::castling_rights(Color c) const {
    return c & CastlingRights(st->castlingRights);
}

inline bool Position::castling_impeded(CastlingRights cr) const {
    assert(cr == WHITE_OO || cr == WHITE_OOO || cr == BLACK_OO || cr == BLACK_OOO);

    return pieces() & castlingPath[cr];
}

inline Square Position::castling_rook_square(CastlingRights cr) const {
    assert(cr == WHITE_OO || cr == WHITE_OOO || cr == BLACK_OO || cr == BLACK_OOO);

    return castlingRookSquare[cr];
}

inline Bitboard Position::attackers_to(Square s) const { return attackers_to(s, pieces()); }

inline Bitboard Position::AttacksByPlayer(Color c, PieceType pt) const {

    if (pt == kPawn)
        return pawn_attacks_bitboard[pieces(c, kPawn)]; // ???
    else
    {
        Bitboard threats   = 0;
        Bitboard attackers = pieces(c, Pt);
        while (attackers)
            threats |= attacks_bb<Pt>(pop_lsb(attackers), pieces());
        return threats;
    }
}

// This function just returns a game of checkers
inline Bitboard Position::checkers() const {
    return checkers_;
}

inline Bitboard Position::meat_shields(Color c) const {
    return meat_shields_[c];
}

inline Bitboard Position::pinners(Color c) const {
    return pinners_[c];
}

inline Bitboard Position::checked(PieceType pt) const
{
    return checked_[pt];
}

inline int Position::Plies() const { return plies_; }

inline bool Position::DoesMoveCapture(Move m) const {
    assert(ValidMove(m));
    return (!IsSquareEmpty(MoveDest(m)) && TypeOfMove(m) != kCastling) || TypeOfMove(m) == kEnPassant;
}

// Returns true if a move is generated from the capture stage, having also
// queen promotions covered, i.e. consistency with the capture stage move generation
// is needed to avoid the generation of duplicate moves.
// inline bool Position::capture_stage(Move m) const {
//     assert(is_ok(m));
//     return capture(m) || promotion_type(m) == QUEEN;
// }

// inline Piece Position::captured_piece() const { return st->capturedPiece; }

inline void Position::PutPiece(Square sq, Piece p) {

    board_pieces_[sq] = p;
    piece_bitboards_[TypeOfPiece(p)] |= sq;
    piece_bitboards_[kAnyPieceType] |= sq;
    color_bitboards_[ColorOfPiece(p)] |= sq;
    piece_nums_[p]++;
    piece_nums_[MakePiece(ColorOfPiece(p), kAnyPieceType)]++;
}

inline void Position::RemovePiece(Square sq) {

    Piece p = board_pieces_[sq];
    piece_bitboards_[kAnyPieceType] ^= sq;
    piece_bitboards_[TypeOfPiece(p)] ^= sq;
    color_bitboards_[ColorOfPiece(p)] ^= sq;
    board_pieces_[sq] = kNoPiece;
    piece_nums_[p]--;
    piece_nums_[MakePiece(ColorOfPiece(p), kAnyPieceType)]--;
}

inline void Position::MovePiece_(Square src, Square dest) {

    Piece    p     = board_pieces_[src];
    Bitboard src_dest = src | dest;
    piece_bitboards_[kAnyPieceType] ^= src_dest;
    piece_bitboards_[TypeOfPiece(p)] ^= src_dest;
    color_bitboards_[ColorOfPiece(p)] ^= src_dest;
    board_pieces_[src] = kNoPiece;
    board_pieces_[dest] = p;
}


} // namespace Stupidfish

#endif // ifndef STUPIDFISH_POSITION_H_