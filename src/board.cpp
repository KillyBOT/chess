#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cstdlib>

#include "board.h"

using std::vector;
using std::size_t;

size_t kZobristPieceNums[64][16];
size_t kZobristBlackToMoveNum;
size_t kZobristCastlingNums[16];
size_t kZobristEnPassantNums[9];

void init_zobrist_nums() {
    srand(43252003); //The first 8 digits of the number of possible rubik's cube configurations

    for(int pos = 0; pos < 64; pos++){
        for(int player = 0; player < 12; player++){
            kZobristPieceNums[pos][player] = 0;
            kZobristPieceNums[pos][player + kPieceKing + 1] = 0;
            for(int type = kPiecePawn; type <= kPieceKing; type++){
                kZobristPieceNums[pos][player+type] = rand();
            }
        }
    }

    kZobristBlackToMoveNum = rand();
    for(int i = 0; i < 16; i++) kZobristCastlingNums[i] = rand();
    for(int i = 0; i < 9; i++) kZobristEnPassantNums[i] = rand();
}

ChessBoard::ChessBoard(bool initBoard){

    this->blackToMove_ = false;
    this->whiteKingPos_ = -1;
    this->blackKingPos_ = -1;

    memset(this->pieces_, 0, 64);
    for(int i = 0; i < 16; i++) this->pieceLists_[i] = ChessPieceList();
    this->occupied_ = 0;

    if(initBoard){
        this->fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    } else {
        this->resetKeys(0b10000001111);
    }

}
ChessBoard::ChessBoard(std::string fen){
    this->blackToMove_ = false;
    this->whiteKingPos_ = -1;
    this->blackKingPos_ = -1;

    memset(this->pieces_, 0, 64 * sizeof(ChessPiece) );
    for(int i = 0; i < 16; i++) this->pieceLists_[i] = ChessPieceList();
    this->occupied_ = 0;

    this->fromFen(fen);
}
ChessBoard::ChessBoard(const ChessBoard &board) {

    memcpy(this->pieces_,board.pieces_, 64);
    memcpy(this->pieceLists_, board.pieceLists_, 16 * sizeof(ChessPieceList));
    this->occupied_ = board.occupied_;

    this->boardData_ = board.boardData_;
    this->boardHistory_ = board.boardHistory_;
    this->moves_ = board.moves_;

    this->blackToMove_ = board.blackToMove_;
    this->whiteKingPos_ = board.whiteKingPos_;
    this->blackKingPos_ = board.blackKingPos_;
}

void ChessBoard::fromFen(std::string fen) {

    memset(this->pieces_, 0, 64);
    for(int i = 0; i < 16; i++) this->pieceLists_[i] = ChessPieceList();

    using std::string;

    std::size_t spacePos = fen.find_first_of(' ');

    string piecePlacement = fen.substr(0, spacePos);
    std::size_t slashPos;
    string piecesInRank;

    char file;
    unsigned int data = 0;
    Player player;

    for(char rank = '8'; rank >= '1'; rank--){
        file = 'a';
        slashPos = piecePlacement.find_first_of('/');
        piecesInRank = piecePlacement.substr(0, slashPos);
        for(char piece : piecesInRank){

            if(piece >= '1' && piece <= '8'){
                for(char c = '0'; c < piece; c++) file++;
            }
            else{
                if(piece >= 'a') {
                    player = kPlayerBlack;
                    piece -= 32;
                } else {
                    player = kPlayerWhite;
                }

                switch(piece){
                    case 'P':
                    this->addPiece(new_pos(file,rank),new_piece(kPiecePawn,player));
                    break;
                    case 'R':
                    this->addPiece(new_pos(file, rank),new_piece(kPieceRook,player));
                    break;
                    case 'B':
                    this->addPiece(new_pos(file, rank),new_piece(kPieceBishop,player));
                    break;
                    case 'N':
                    this->addPiece(new_pos(file, rank),new_piece(kPieceKnight,player));
                    break;
                    case 'K':
                    this->addPiece(new_pos(file, rank),new_piece(kPieceKing,player));
                    break;
                    case 'Q':
                    this->addPiece(new_pos(file, rank),new_piece(kPieceQueen,player));
                    break;
                }

                file++;
            }
        }
        piecePlacement.erase(0,slashPos+1);
    }

    fen.erase(0,spacePos + 1);
    spacePos = fen.find_first_of(' ');
    
    if(fen[0] == 'b') this->blackToMove_ = true;
    else this->blackToMove_ = false;

    fen.erase(0,spacePos + 1);
    spacePos = fen.find_first_of(' ');

    if(fen[0] != '-'){
        string castlingRights = fen.substr(0,spacePos);
        for(char c : castlingRights){
            switch(c){
                case 'K':
                data |= 0b0001;
                break;
                case 'Q':
                data |= 0b0010;
                break;
                case 'k':
                data |= 0b0100;
                break;
                case 'q':
                data |= 0b1000;
                break;
            }
        }
    }

    fen.erase(0,spacePos + 1);
    spacePos = fen.find_first_of(' ');

    if(fen[0] != '-'){
        string enPassantSquare = fen.substr(0,spacePos);
        data |= (unsigned int)(new_pos(enPassantSquare)) << 4;
    } else {
        data |= (1 << 10);
    }

    fen.erase(0, spacePos + 1);
    spacePos = fen.find_first_of(' ');

    data |= std::stoi(fen.substr(0,spacePos)) << 11;

    fen.erase(0, spacePos + 1);

    this->resetKeys(data);
}
void ChessBoard::addNewKey(ChessMove move) {
    size_t newKey = this->key();
    Byte castlingInfo = this->boardData_.back() & 0b1111;
    unsigned int newData = 0;

    if((this->boardData_.back() >> 10) & 1){ //If it's an invalid space, then unhash the empty file
        newKey ^= kZobristEnPassantNums[8];
    } else { //Otherwise, unhash the file of the previous en passant file
        newKey ^= kZobristEnPassantNums[(this->boardData_.back() >> 4) & 0b111];
    }
    newKey ^= kZobristCastlingNums[castlingInfo];

    //If castling, remove the rook from the old position and add it at the new position
    if(move.moveData == kMoveIsCastling){
        if(move.isCastlingKingside()){
            newKey ^= kZobristPieceNums[new_pos('h',pos_rank_char(move.oldPos))][new_piece(kPieceRook, piece_player(move.piece))];
            newKey ^= kZobristPieceNums[new_pos('f',pos_rank_char(move.oldPos))][new_piece(kPieceRook, piece_player(move.piece))];
        } else {
            newKey ^= kZobristPieceNums[new_pos('a',pos_rank_char(move.oldPos))][new_piece(kPieceRook, piece_player(move.piece))];
            newKey ^= kZobristPieceNums[new_pos('c',pos_rank_char(move.oldPos))][new_piece(kPieceRook, piece_player(move.piece))];
        }
    }

    //If capturing, remove the captured piece
    if(move.moveData == kMoveEnPassant){
        ChessPos capturePos = move.newPos;
        if(piece_player(move.piece) == kPlayerWhite) capturePos -= 8;
        else capturePos += 8;

        newKey ^= kZobristPieceNums[capturePos][move.captured];
    } else if (move.isCapturing()) newKey ^= kZobristPieceNums[move.newPos][move.captured];

    //Remove the piece at oldPos and add the piece at newPos
    newKey ^= kZobristPieceNums[move.oldPos][move.piece];
    newKey ^= kZobristPieceNums[move.newPos][move.piece];

    //Generate the new data
    if(!move.isCapturing()) newData = this->movesSinceLastCapture() + 1;
    newData <<= 7;
    if(move.moveData == kMoveEnPassantEligible) {
        if(piece_player(move.piece) == kPlayerWhite) {
            move.newPos -= 8;
            newData |= move.newPos;
            move.newPos += 8;
        } else {
            move.newPos += 8;
            newData |= move.newPos;
            move.newPos -= 8;
        }
    } else {
        newData |= 0b1000000;
    }
    newData <<= 4;
    // if(move.moveData == kMoveIsCastling){
    //     if(move.isCastlingKingside()){
    //         if(move.piece.player() == kPlayerWhite) castlingInfo &= kBoardDataWhiteKingside;
    //         else castlingInfo &= kBoardDataBlackKingside;
    //     } else {
    //         if(move.piece.player() == kPlayerWhite) castlingInfo &= kBoardDataWhiteQueenside;
    //         else castlingInfo &= kBoardDataBlackQueenside;
    //     }
    // }
    if(piece_type(move.piece) == kPieceKing){
        if(piece_player(move.piece) == kPlayerWhite){
            castlingInfo &= kBoardDataWhiteKingside;
            castlingInfo &= kBoardDataWhiteQueenside;
        } else {
            castlingInfo &= kBoardDataBlackKingside;
            castlingInfo &= kBoardDataBlackQueenside;
        }
    } else if(piece_type(move.piece) == kPieceRook){
        if(move.oldPos == 0) castlingInfo &= kBoardDataWhiteQueenside;
        else if(move.oldPos == 0b000111) castlingInfo &= kBoardDataWhiteKingside;
        else if(move.oldPos == 0b111000) castlingInfo &= kBoardDataBlackQueenside;
        else if(move.oldPos == 0b111111) castlingInfo &= kBoardDataBlackKingside;
    }
    newData |= castlingInfo;

    if(move.moveData == kMoveEnPassantEligible) {
        newKey ^= kZobristEnPassantNums[((newData >> 4) & 0b111)];
    } else {
        newKey ^= kZobristEnPassantNums[8];
    }
    newKey ^= kZobristCastlingNums[newKey & 0b1111];
    newKey ^= kZobristBlackToMoveNum;

    this->boardHistory_.push_back(newKey);
    this->boardData_.push_back(newData);
}
void ChessBoard::resetKeys(unsigned int data) {

    this->boardData_.clear();
    this->boardHistory_.clear();

    this->boardData_.push_back(data);

    size_t key = 0;

    for(int i = 0; i < 64; i++){
        key ^= kZobristPieceNums[i][this->piece(i)];
    }

    key ^= kZobristCastlingNums[data & 0b1111];

    if((data >> 10) & 1){
        key ^= kZobristEnPassantNums[8];
    } else {
        key ^= kZobristEnPassantNums[(data >> 4) & 0b111];
    }

    if(this->blackToMove_) key ^= kZobristBlackToMoveNum;

    this->boardHistory_.push_back(key);
}

int ChessBoard::turnNum() const {
    return (this->moves_.size() >> 1) + 1;
}
int ChessBoard::playerScore(Player player) const {
    int scores[2] = {0,0};
    for(ChessPos pos = 0; pos < 64; pos++) scores[piece_player(this->pieces_[pos])] += kPieceValue[piece_type(this->pieces_[pos])];

    return (player == kPlayerWhite ? scores[0] : scores[1]);
}

void ChessBoard::printBoard() const {
    using std::cout;
    using std::endl;

    ChessPos pos;
    cout << (this->blackToMove_ ? "Black's " : "White's ") << "turn" << endl;
    cout << this->playerScore(kPlayerWhite) / 100 << " | " << this->playerScore(kPlayerBlack) / 100 << endl;
    cout << "Castling rights: ";
    if(!(this->boardData_.back() & 0b1111)) cout << '-';
    else{
        if(this->boardData_.back() & 0b0001) cout << 'K';
        if(this->boardData_.back() & 0b0010) cout << 'Q';
        if(this->boardData_.back() & 0b0100) cout << 'k';
        if(this->boardData_.back() & 0b1000) cout << 'q';
    }
    
    cout << endl;

    cout << "En passant square: ";
    if((this->boardData_.back() >> 10) & 1){
        cout << '-';
    } else {
        cout << pos_str((this->boardData_.back() >> 4) & 0b111111);
    }
    cout << endl;

    cout << "Moves since last capture/pawn move: ";
    cout << (this->boardData_.back() >> 11) << endl;

    for(int rank = 7; rank >= 0; rank--){
        cout << rank + 1 << ' ';
        for(int file = 0; file < 8; file++){
            pos = rank * 8 + file;
            if(this->piece(pos)) cout << piece_char(this->piece(pos));
            else cout << (((rank + file) & 1) ? ' ' : '#');
            cout << ' ';
        }
        cout << endl;
    }
    cout << "  ";
    for(char col = 'a'; col <= 'h'; col++){
        cout << col << ' ';
    }
    cout << endl;

    if(!this->moves_.empty()){
        if(piece_player(this->lastMove().piece) == kPlayerWhite) cout << this->turnNum() << ": " << this->lastMove().str() << endl;
        else cout << this->turnNum()-1 << ": " << this->moves_[this->moves_.size()-2].str() << ' ' << this->moves_[this->moves_.size()-1].str() << endl;
    }

    //cout << endl;
}
void ChessBoard::printMoves() const {
    using std::cout;
    using std::endl;
    if(this->turnNum() == 1){
        cout << "1: " << endl;
        return;
    }
    for(int turn = 1; turn < this->turnNum(); turn++) cout << turn << ": " << this->moves_.at((turn-1) * 2).str() << ' ' << this->moves_.at(((turn-1) * 2) + 1).str() << endl;

    if(this->blackToMove_) cout << this->turnNum() << ": " << this->lastMove().str() << endl;
}
void ChessBoard::printPieces() const {
    using std::cout;
    using std::endl;

    cout << "White king: ";
    cout << pos_str(this->whiteKingPos_) << endl;
    cout << "White pawns: ";
    this->pieceLists_[kPiecePawn].printList();
    cout << "White rooks: ";
    this->pieceLists_[kPieceRook].printList();
    cout << "White knights: ";
    this->pieceLists_[kPieceKnight].printList();
    cout << "White bishops: ";
    this->pieceLists_[kPieceBishop].printList();
    cout << "White queens: ";
    this->pieceLists_[kPieceQueen].printList();

    cout << "Black king: ";
    cout << pos_str(this->blackKingPos_) << endl;
    cout << "Black pawns: ";
    this->pieceLists_[kPiecePawn + 0b1000].printList();
    cout << "Black rooks: ";
    this->pieceLists_[kPieceRook + 0b1000].printList();
    cout << "Black knights: ";
    this->pieceLists_[kPieceKnight + 0b1000].printList();
    cout << "Black bishops: ";
    this->pieceLists_[kPieceBishop + 0b1000].printList();
    cout << "Black queens: ";
    this->pieceLists_[kPieceQueen + 0b1000].printList();
    
}

//WARNING: if it's an invalid position, it will break!
void ChessBoard::addPiece(ChessPos pos, ChessPiece piece) {

   //std::cout << "Adding piece " << piece.pieceChar() << " at " << pos.str() << std::endl;

    if(piece_type(piece) == kPieceKing){
        // std::cout << "Adding king:" << std::endl;
        // this->printBoard();
        if(piece_player(piece)== kPlayerWhite) this->whiteKingPos_ = pos;
        else this->blackKingPos_ = pos;
        //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;
    } else {
        this->pieceLists_[piece].addPiece(pos);
    }

    this->pieces_[pos] = piece;
    set_bit(this->occupied_, pos);
}
//WARNING: if it's an invalid position, it will break!
ChessPiece ChessBoard::removePiece(ChessPos pos) {

    //std::cout << "Removing piece at " << pos.str() << std::endl;

    ChessPiece removed = this->piece(pos);

    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;
    // if(pos == this->whiteKingPos_ || pos == this->blackKingPos_) {
    //     std::cout << "Removing king:" << std::endl;
    //     this->printBoard();
    // }
    if(this->whiteKingPos_ == pos) this->whiteKingPos_ = -1;
    else if(this->blackKingPos_ == pos) this->blackKingPos_ = -1;
    else this->pieceLists_[removed].removePiece(pos);
    
    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    this->pieces_[pos] = 0;
    reset_bit(this->occupied_, pos);

    return removed;
}
//WARNING: if either of the positions are invalid, it will not work!
void ChessBoard::movePiece(ChessPos oldPos, ChessPos newPos) {
    //std::cout << "Moving piece at " << oldPos.str() << " to " << newPos.str() << std::endl;
    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    if(oldPos == this->whiteKingPos_) this->whiteKingPos_ = newPos;
    else if (oldPos == this->blackKingPos_) this->blackKingPos_ = newPos;
    else this->pieceLists_[this->piece(oldPos)].movePiece(oldPos, newPos);

    //std::cout << this->whiteKingPos_.str() << '\t' << this->blackKingPos_.str() << std::endl;

    this->pieces_[newPos] = this->pieces_[oldPos];
    this->pieces_[oldPos] = 0;
    set_bit(this->occupied_, newPos);
    reset_bit(this->occupied_, oldPos);
}

void ChessBoard::doMove(ChessMove move, bool update) {

    if(move.moveData == kMoveIsCastling){
        if(move.isCastlingKingside()) this->doMove(ChessMove(this->piece(new_pos('h',pos_rank_char(move.oldPos))),new_pos('h',pos_rank_char(move.oldPos)),new_pos('f',pos_rank_char(move.oldPos))), false);
        else this->doMove(ChessMove(this->piece(new_pos('a',pos_rank_char(move.oldPos))),new_pos('a',pos_rank_char(move.oldPos)),new_pos('c',pos_rank_char(move.oldPos))), false);
    }

    if(move.moveData == kMoveEnPassant){
        //this->printBoard();
        ChessPos removePos = move.newPos;
        if(piece_player(move.piece) == kPlayerWhite) removePos -= 8;
        else removePos += 8;

        this->removePiece(removePos);
    } else if(move.isCapturing()) this->removePiece(move.newPos);


    if(move.isPromoting()) {
        this->removePiece(move.oldPos);
        this->addPiece(move.newPos, new_piece(move.promotionType(), piece_player(move.piece)));
    } else {
        this->movePiece(move.oldPos, move.newPos);
    }

    if(update){
        this->blackToMove_ = !this->blackToMove_;
        this->moves_.push_back(move);
        this->addNewKey(move);
    }

    //if(move.isEnPassant()) this->printBoard();


}
void ChessBoard::undoMove(ChessMove move, bool update) {

    if(move.moveData == kMoveIsCastling){
        if(move.isCastlingKingside()) this->undoMove(ChessMove(new_piece(kPieceRook, piece_player(move.piece)),new_pos('h',pos_rank_char(move.oldPos)),new_pos('f',pos_rank_char(move.oldPos))), false);
        else this->undoMove(ChessMove(new_piece(kPieceRook, piece_player(move.piece)),new_pos('a',pos_rank_char(move.oldPos)),new_pos('c',pos_rank_char(move.oldPos))), false);
    }

    this->removePiece(move.newPos);
    this->addPiece(move.oldPos, move.piece);

    if(move.moveData == kMoveEnPassant){
        ChessPos removePos = move.newPos;
        if(piece_player(move.piece) == kPlayerWhite) removePos -= 8;
        else removePos += 8;

        this->addPiece(removePos, move.captured);
    } else if (move.isCapturing()) this->addPiece(move.newPos, move.captured);

    if(update){
        this->moves_.pop_back();
        this->boardHistory_.pop_back();
        this->boardData_.pop_back();
        this->blackToMove_ = !this->blackToMove_;
    }
}
void ChessBoard::undoLastMove() {
    this->undoMove(this->lastMove());
}