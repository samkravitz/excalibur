/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: board.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: A board representation contains bitboards of each piece type of each color.
 */

#include "board.h"

#include <cassert>
#include <cstring>
#include <iostream>

#include "bitboard.h"
#include "constants.h"
#include "util.h"

/*
 * this array holds the squares a rook will move to when castling.
 * the outermost array is the color index
 * the middle array is the castle type index
 * the innermost array is { old_square, new_square } of the rook during a castle
 * 
 * an example usage is this:
 * Square old_rook_square = castle_squares[WHITE][KINGSIDE][0]
 * Square new_rook_square = castle_squares[WHITE][KINGSIDE][1]
 */
constexpr Square castle_squares[2][2][2] = {
    {
        { H1, F1 },
        { A1, D1 }
    },
    {
        { H8, F8 },
        { A8, D8 }
    }
};

Board::Board()
{
    reset();
}

/**
 * @brief returns board to initial position of a chess game
 */
void Board::reset()
{
    piece_bb[PAWN]   = Constants::WPAWNS_INIT   | Constants::BPAWNS_INIT;
    piece_bb[KNIGHT] = Constants::WKNIGHTS_INIT | Constants::BKNIGHTS_INIT;
    piece_bb[BISHOP] = Constants::WBISHOPS_INIT | Constants::BBISHOPS_INIT;
    piece_bb[ROOK]   = Constants::WROOKS_INIT   | Constants::BROOKS_INIT;
    piece_bb[QUEEN]  = Constants::WQUEENS_INIT  | Constants::BQUEENS_INIT;
    piece_bb[KING]   = Constants::WKING_INIT    | Constants::BKING_INIT;

    color_bb[WHITE]  = Constants::WPAWNS_INIT | Constants::WKNIGHTS_INIT | Constants::WBISHOPS_INIT | Constants::WROOKS_INIT | Constants::WQUEENS_INIT | Constants::WKING_INIT;
    color_bb[BLACK]  = Constants::BPAWNS_INIT | Constants::BKNIGHTS_INIT | Constants::BBISHOPS_INIT | Constants::BROOKS_INIT | Constants::BQUEENS_INIT | Constants::BKING_INIT;

    std::memcpy(board, Constants::BOARD_INIT, sizeof(board));

    to_move = WHITE;
    captured_piece = NONE;

    // reset castling rights
    castle_rights[WHITE][KINGSIDE]  = true;
    castle_rights[WHITE][QUEENSIDE] = true;
    castle_rights[BLACK][KINGSIDE]  = true;
    castle_rights[BLACK][QUEENSIDE] = true;
}

/**
 * @brief clears all the pieces off the board
 */
void Board::clear()
{
    // clear board
    for (int i = 0; i < 64; i++)
        board[i] = NONE;
    
    // clear piece bb
    for (int i = 0; i < 6; i++)
        piece_bb[i] = 0;
    
    // clear color bb
    color_bb[0] = 0;
    color_bb[1] = 0;

    // clear castling rights
    castle_rights[WHITE][KINGSIDE]  = false;
    castle_rights[WHITE][QUEENSIDE] = false;
    castle_rights[BLACK][KINGSIDE]  = false;
    castle_rights[BLACK][QUEENSIDE] = false;
}

void Board::set_piece(PieceType pt, Square square, Color c)
{
    board[square] = pt;
    piece_bb[pt] |= square;
    color_bb[c]  |= square;
}

void Board::set_to_move(Color c)
{
    to_move = c;
}

void Board::make_move(Move const &move)
{
    // save irreversable state
    save();

    Square from = move.from();
    Square to = move.to();

    // get the piece type currently on the origin square
    auto moved_piece = piece_on(from);

    // make sure there is an actual piece on that square
    assert(moved_piece != NONE);

    // update castle rights
    if (moved_piece == KING || moved_piece == ROOK)
    {
        castle_rights[mover()][KINGSIDE]  = false;
        castle_rights[mover()][QUEENSIDE] = false;
    }

    // do castle
    if (move.is_castle())
    {
        int idx = move.flags() == KINGSIDE_CASTLE ? 0 : 1;

        Square kold = from, knew = to;
        Square rold = castle_squares[mover()][idx][0];
        Square rnew = castle_squares[mover()][idx][1];

        // update the board array
        board[knew] = KING;
        board[kold] = NONE;
        board[rnew] = ROOK;
        board[rold] = NONE;

        // clear the old king square on the piece bitboard
        piece_bb[KING] ^= kold;
        // clear the old rook square on the piece bitboard
        piece_bb[ROOK] ^= rold;
        // set the new king square on the piece bitboard
        piece_bb[KING] |= knew;
        // set the new rook square on the piece bitboard
        piece_bb[ROOK] |= rnew;

        // clear the old king square on the color bitboard
        color_bb[mover()] ^= kold;
        // clear the old rook square on the color bitboard
        color_bb[mover()] ^= rold;
        // set the new king square on the color bitboard
        color_bb[mover()] |= knew;
        // set the new rook square on the color bitboard
        color_bb[mover()] |= rnew;

        // switch the player to move
        to_move = ~to_move;

        return;
    }

    if (move.is_capture())
    {
        // save type of piece on the captured square
        captured_piece = piece_on(to);

        // unset the captured square
        piece_bb[captured_piece] ^= to;

        // unset the captured square on the color bb
        color_bb[~mover()] ^= to;
    }

    board[from] = NONE;
    board[to] = moved_piece;

    // clear the origin square on the bitboard of the moved piece
    piece_bb[moved_piece] ^= from;

    // clear the origin square on the color bitboard of the moved piece
    color_bb[mover()] ^= from;

    // set the destination square on the bitboard of the moved piece
    piece_bb[moved_piece] |= to;

    // set the destination square on the color bitboard of the moved piece
    color_bb[mover()] |= to;

    if (move.is_promotion())
    {
        PieceType promoted_to;
        switch (move.flags())
        {
            case QUEEN_PROMOTION:
            case QUEEN_PROMO_CAPTURE:
                promoted_to = QUEEN;
                break;
            
            case KNIGHT_PROMOTION:
            case KNIGHT_PROMO_CAPTURE:
                promoted_to = KNIGHT;
                break;

            case ROOK_PROMOTION:
            case ROOK_PROMO_CAPTURE:
                promoted_to = ROOK;
                break;

            case BISHOP_PROMOTION:
            case BISHOP_PROMO_CAPTURE:
                promoted_to = BISHOP;
                break;
        }

        // clear the origin square on the bitboard of the promoted pawn
        piece_bb[PAWN] ^= from;

        // set the destination square on the bitboard of the promoted piece
        piece_bb[promoted_to] |= to;

        board[to] = promoted_to;
    }

    // switch the player to move
    to_move = ~to_move;
}

void Board::undo_move(Move const &move)
{
    // restore irreversable state
    restore();

    // square the piece was on before moving
    Square old_square = move.from();
    
    // square the piece moved to
    Square new_square = move.to();

    // color of the piece that moved
    Color moved_color = ~to_move;

    // get the piece type on the new square
    auto moved_piece = piece_on(new_square);

    // make sure there is an actual piece on that square
    assert(moved_piece != NONE);

    // undo castle
    if (move.is_castle())
    {
        int idx = move.flags() == KINGSIDE_CASTLE ? 0 : 1;

        Square kold = old_square, knew = new_square;
        Square rold = castle_squares[moved_color][idx][0];
        Square rnew = castle_squares[moved_color][idx][1];

        // update the board array
        board[kold] = KING;
        board[knew] = NONE;
        board[rold] = ROOK;
        board[rnew] = NONE;

        // clear the old king square on the piece bitboard
        piece_bb[KING] ^= knew;
        // clear the old rook square on the piece bitboard
        piece_bb[ROOK] ^= rnew;
        // set the new king square on the piece bitboard
        piece_bb[KING] |= kold;
        // set the new rook square on the piece bitboard
        piece_bb[ROOK] |= rold;

        // clear the old king square on the color bitboard
        color_bb[moved_color] ^= knew;
        // clear the old rook square on the color bitboard
        color_bb[moved_color] ^= rnew;
        // set the new king square on the color bitboard
        color_bb[moved_color] |= kold;
        // set the new rook square on the color bitboard
        color_bb[moved_color] |= rold;

        // switch the player to move
        to_move = ~to_move;

        return;
    }

    board[old_square] = moved_piece;
    board[new_square] = NONE;

    // clear the square that the piece moved to
    piece_bb[moved_piece] ^= new_square;

    // clear the square that the piece moved to on the color bb
    color_bb[moved_color] ^= new_square;

    // set the square that the piece originally was on
    piece_bb[moved_piece] |= old_square;

    // set the square that the piece originally was on on the color bb
    color_bb[moved_color] |= old_square;

    if (move.is_capture())
    {
        assert(captured_piece != NONE);
        board[new_square] = captured_piece;

        // set the square that the captured piece was on
        piece_bb[captured_piece] |= new_square;

        // set the square that the captured piece was on color bb
        color_bb[~moved_color] |= new_square;
        captured_piece = NONE;
    }

    if (move.is_promotion())
    {
        // clear the promoted piece
        piece_bb[moved_piece] ^= old_square;

        // set the pawn bitboard for old_sqaure (we are un-promoting it)
        piece_bb[PAWN] |= old_square;

        board[old_square] = PAWN;
    }

    // switch the player to move
    to_move = ~to_move;
}

void Board::save()
{
    BoardState bs(castle_rights);
    saved_state.push(bs);
}

void Board::restore()
{
    assert(!saved_state.empty());
    auto state = saved_state.top();
    castle_rights[0][0] = state.castle_rights[0][0];
    castle_rights[0][1] = state.castle_rights[0][1];
    castle_rights[1][0] = state.castle_rights[1][0];
    castle_rights[1][1] = state.castle_rights[1][1];
    saved_state.pop();
}

std::string Board::to_string() const
{
    std::string res = "";

    for (int rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (int file = FILE_A; file <= FILE_H; ++file)
        {
            Square s = static_cast<Square>(rank * 8 + file);
            PieceType type = board[s];
            char c;
            switch (type)
            {
                case PAWN:   c = color_bb[WHITE] & s ? 'P' : 'p'; break;
                case KNIGHT: c = color_bb[WHITE] & s ? 'N' : 'n'; break;
                case BISHOP: c = color_bb[WHITE] & s ? 'B' : 'b'; break;
                case ROOK:   c = color_bb[WHITE] & s ? 'R' : 'r'; break;
                case QUEEN:  c = color_bb[WHITE] & s ? 'Q' : 'q'; break;
                case KING:   c = color_bb[WHITE] & s ? 'K' : 'k'; break;
                default:     c = '*';
            }
            res += c;
            res += ' ';
        }
        res += '\n';
    }

    return res;
}

void Board::print() const
{
    std::cout << to_string() << '\n';
}
