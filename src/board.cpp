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

#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>

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
    char board[8][8];
    std::memset(board, '*', sizeof(board));

    const std::vector<u64> bb_all = { 
        pieces(PAWN,   WHITE),
        pieces(PAWN,   BLACK),
        pieces(KNIGHT, WHITE),
        pieces(KNIGHT, BLACK),
        pieces(BISHOP, WHITE),
        pieces(BISHOP, BLACK),
        pieces(ROOK,   WHITE),
        pieces(ROOK,   BLACK),
        pieces(QUEEN,  WHITE),
        pieces(QUEEN,  BLACK),
        pieces(KING,   WHITE),
        pieces(KING,   BLACK),
    };
    const std::vector<char> chars = { 'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r','Q', 'q', 'K', 'k' };

    std::vector<std::tuple<u64, char>> zipped;

    std::transform(bb_all.begin(), bb_all.end(), chars.begin(), std::back_inserter(zipped), [](const auto &a, const auto &b) {
        return std::make_tuple(a, b);
    });

    for (auto [bitboard, c] : zipped)
    {
        for (const auto &idx : indeces_set(bitboard))
        {
            auto const [rank, file] = Util::rank_file_from_square(Square(idx));
            board[rank][file] = c;
        }
    }

    std::string res = "";

    for (int rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (int file = FILE_A; file <= FILE_H; ++file)
        {
            res += board[rank][file];
            res += ' ';
        }
        res += '\n';
    }

    return res;
}