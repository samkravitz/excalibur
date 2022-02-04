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

#include "bitboard.h"
#include "constants.h"
#include "util.h"

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
    Square from = move.from();
    Square to = move.to();

    // get the piece type currently on the origin square
    auto moved_piece = piece_on(from);

    // make sure there is an actual piece on that square
    assert(moved_piece != NONE);

    if (move.flags() == CAPTURE)
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

    // switch the player to move
    to_move = ~to_move;
}

void Board::undo_move(Move const &move)
{
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

    if (move.flags() == CAPTURE)
    {
        assert(captured_piece != NONE);
        board[new_square] = captured_piece;

        // set the square that the captured piece was on
        piece_bb[captured_piece] |= new_square;

        // set the square that the captured piece was on color bb
        color_bb[~moved_color] |= new_square;
        captured_piece = NONE;
    }

    // switch the player to move
    to_move = ~to_move;
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