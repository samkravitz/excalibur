/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: movegen.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: Generation of legal and pseudolegal moves
 */

#include "movegen.h"

#include <iostream>

#include "bitboard.h"
#include "constants.h"

std::vector<Move> pseudolegal(Board const &board, Color c)
{
    std::vector<Move> moves;

    // get all pieces
    auto pieces = board.pieces();
    // get all pieces of our color
    auto our_pieces = board.pieces(c);

    // generate pseudolegal pawn moves
    auto pawns = board.pieces(PAWN, c);
    for (const auto &from : indeces_set(pawns))
    {
        auto moves_bb = Constants::pawn_push_table[c][from];
        for (const auto &to : indeces_set(moves_bb))
        {
            if (!(pieces & static_cast<Square>(to)))
                moves.push_back(Move(from, to));
        }
    }

    // generate pseudolegal knight moves
    auto knights = board.pieces(KNIGHT, c);
    for (const auto &from : indeces_set(knights))
    {
        auto moves_bb = Constants::knight_move_table[from];
        for (const auto &to : indeces_set(moves_bb))
        {
            if (!(our_pieces & static_cast<Square>(to)))
                moves.push_back(Move(from, to));
        }
    }

    // generate pseudolegal king moves
    auto king = board.pieces(KING, c);
    for (const auto &from : indeces_set(king))
    {
        auto moves_bb = Constants::king_move_table[from];
        for (const auto &to : indeces_set(moves_bb))
        {
            if (!(our_pieces & static_cast<Square>(to)))
                moves.push_back(Move(from, to));
        }
    }

    return moves;
}

std::vector<Move> legal(Board const &board, Color c)
{
    std::vector<Move> moves;
    return moves;
}
