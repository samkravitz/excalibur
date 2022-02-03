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

    // get occupancy set (all pieces)
    u64 occ = board.pieces();

    // get all pieces of our color
    auto our_pieces = board.pieces(c);

    // generate pseudolegal pawn moves
    u64 pawns = board.pieces(PAWN, c);
    u64 single_pushes = single_push_targets(pawns, ~occ, c);
    int perspective = c == WHITE ? -1 : 1;
    while (single_pushes)
    {
        Square to = bitscan(single_pushes);
        Square from = static_cast<Square>(to + 8 * perspective);
        moves.push_back(Move(from, to));
        single_pushes &= single_pushes - 1;
    }

    u64 double_pushes = double_push_targets(pawns, ~occ, c);
    while (double_pushes)
    {
        Square to = bitscan(double_pushes);
        Square from = static_cast<Square>(to + 16 * perspective);
        moves.push_back(Move(from, to));
        double_pushes &= double_pushes - 1;
    }

    // generate pseudolegal knight moves
    //auto knights = board.pieces(KNIGHT, c);
    //for (const auto &from : indeces_set(knights))
    //{
    //    auto moves_bb = Constants::knight_move_table[from];
    //    for (const auto &to : indeces_set(moves_bb))
    //    {
    //        if (!(our_pieces & static_cast<Square>(to)))
    //            moves.push_back(Move(from, to));
    //    }
    //}

    //// generate pseudolegal king moves
    //auto king = board.pieces(KING, c);
    //for (const auto &from : indeces_set(king))
    //{
    //    auto moves_bb = Constants::king_move_table[from];
    //    for (const auto &to : indeces_set(moves_bb))
    //    {
    //        if (!(our_pieces & static_cast<Square>(to)))
    //            moves.push_back(Move(from, to));
    //    }
    //}

    // get pseudolegal rook moves
    u64 rooks = board.pieces(ROOK, c);
    while (rooks)
    {
        Square from = bitscan(rooks);
        u64 attacks = sliding_attacks<ROOK>(from, occ);

        // filter out attacked squares that are occupied by one of our pieces
        attacks &= ~our_pieces;
        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to));
            attacks &= attacks - 1;
        }
        rooks &= rooks - 1;
    }

    // get pseudolegal bishop moves
    u64 bishops = board.pieces(BISHOP, c);
    while (bishops)
    {
        Square from = bitscan(bishops);
        u64 attacks = sliding_attacks<BISHOP>(from, occ);

        // filter out attacked squares that are occupied by one of our pieces
        attacks &= ~our_pieces;
        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to));
            attacks &= attacks - 1;
        }
        bishops &= bishops - 1;
    }

    // get pseudolegal queen moves
    u64 queens = board.pieces(QUEEN, c);
    while (queens)
    {
        Square from = bitscan(queens);
        u64 attacks = sliding_attacks<QUEEN>(from, occ);

        // filter out attacked squares that are occupied by one of our pieces
        attacks &= ~our_pieces;
        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to));
            attacks &= attacks - 1;
        }
        queens &= queens - 1;
    }

    return moves;
}

std::vector<Move> legal(Board const &board, Color c)
{
    std::vector<Move> moves;
    return moves;
}

template <PieceType pt>
u64 sliding_attacks(Square square, u64 occupied)
{
    static_assert(pt != PAWN && pt != KNIGHT && pt != KING, "can only use attacks() for sliding pieces!");

    if constexpr (pt == BISHOP)
        return diagonal_attacks(square, occupied) | antidiagonal_attacks(square, occupied);
    else if constexpr(pt == ROOK)
        return file_attacks(square, occupied) | rank_attacks(square, occupied);
    else
        return diagonal_attacks(square, occupied) | antidiagonal_attacks(square, occupied) | file_attacks(square, occupied) | rank_attacks(square, occupied);

}

template <Direction d>
u64 ray_attacks(Square square, u64 occupied)
{
    u64 attacks = Constants::ray_attack_table[d][square];
    
    // blocker set - pieces that block the ray
    u64 blockers = attacks & occupied;

    if (blockers == 0)
        return attacks;
    
    // get square of first blocker
    Square first_blocker;
    
    // positive ray attacks - bitscan forward
    if constexpr (d == EAST || d == NORTH || d == NORTHEAST || d == NORTHWEST)
        first_blocker = bitscan<FORWARD>(blockers);
    
    // negative ray attacks - bitscan backwards
    else if constexpr (d == WEST || d == SOUTH || d == SOUTHWEST || d == SOUTHEAST)
        first_blocker = bitscan<REVERSE>(blockers);
    
    return attacks ^ Constants::ray_attack_table[d][first_blocker];
}

u64 single_push_targets(u64 pawns, u64 empty, Color c)
{
    if (c == WHITE)
        return shift<NORTH>(pawns) & empty;
    else
        return shift<SOUTH>(pawns) & empty;
}

u64 double_push_targets(u64 pawns, u64 empty, Color c)
{
    u64 double_push_target;
    if (c == WHITE)
    {
        double_push_target = 0x00000000FF000000;
        u64 single_pushes = single_push_targets(pawns, empty, c);
        return shift<NORTH>(single_pushes) & empty & double_push_target;     
    }

    else
    {
        double_push_target = 0x000000FF00000000;
        u64 single_pushes = single_push_targets(pawns, empty, c);    
        return shift<SOUTH>(single_pushes) & empty & double_push_target;     
    }
}

u64 diagonal_attacks(Square square, u64 occupied)
{
    return ray_attacks<NORTHEAST>(square, occupied) | ray_attacks<SOUTHWEST>(square, occupied);
}

u64 antidiagonal_attacks(Square square, u64 occupied)
{
    return ray_attacks<NORTHWEST>(square, occupied) | ray_attacks<SOUTHEAST>(square, occupied);
}

u64 rank_attacks(Square square, u64 occupied)
{
    return ray_attacks<EAST>(square, occupied) | ray_attacks<WEST>(square, occupied);
}

u64 file_attacks(Square square, u64 occupied)
{
    return ray_attacks<NORTH>(square, occupied) | ray_attacks<SOUTH>(square, occupied);
}
