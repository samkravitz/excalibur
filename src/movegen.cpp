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

#include <bit>
#include <iostream>

#include "bitboard.h"
#include "constants.h"

std::vector<Move> generate_moves(Board const &board)
{
    return board.mover() == WHITE ? movegen<WHITE, LEGAL>(board) : movegen<BLACK, LEGAL>(board);
}

template <Color c, MovegenType type>
std::vector<Move> movegen(Board const &board)
{
    static_assert(c == WHITE || c == BLACK);
    std::vector<Move> moves;

    // get occupancy set (all pieces)
    u64 occ = board.pieces();

    // get all pieces of our color
    u64 our_pieces = board.pieces(c);

    Square king_square = board.king_square(c);

    // declare variables needed for legal move generation
    u64 occ_without_king;                // get occupancy set without our king (used for calculating legal moves)
    u64 opponent_attacks;                // opponents attack set
    u64 checks;                          // set of squares attacking our king
    u64 check_mask = 0xffffffffffffffff; // set of squares we are allowed to move to due to check
    bool in_check, in_double_check;
    
    if constexpr (type == LEGAL)
    {
        occ_without_king = occ ^ board.pieces(KING, c);
        opponent_attacks = attack_set(board, ~c, occ_without_king);
        checks = checkers(board, c);
        in_check = std::popcount(checks) == 1;
        in_double_check = std::popcount(checks) > 1;

        /*
         * if we are in single check, we need to calculate what squares we are allowed to move to
         * (if we are in double check the only legal moves are king moves)
         * our options to get out of check are as follows:
         * 1. move the king out of check
         * 2. capture the checking piece
         * 3. block the checking piece (if checking piece is a sliding piece)
         * 
         * our check_mask from above helps us for 2 and 3
         * 
         * https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/
         */
        if (in_check)
        {
            // default our moves to only those who capture our checker
            check_mask = checks;

            Square checker_square = bitscan(checks);
            PieceType checker_piece = board.piece_on(checker_square);

            // checking piece is a sliding piece, so we can attempt to block
            if (checker_piece == BISHOP || checker_piece == ROOK || checker_piece == QUEEN)
            {
                // TODO - find a better way to calculate ray attack direction
                auto [king_rank, king_file] = Util::rank_file_from_square(king_square);
                auto [checker_rank, checker_file] = Util::rank_file_from_square(checker_square);
                if (king_rank == checker_rank)
                {
                    if (king_file > checker_file)
                        check_mask |= ray_attacks<EAST>(checker_square, occ);
                    else
                        check_mask |= ray_attacks<WEST>(checker_square, occ);
                }

                else if (king_file == checker_file)
                {
                    if (king_rank > checker_rank)
                        check_mask |= ray_attacks<NORTH>(checker_square, occ);
                    else
                        check_mask |= ray_attacks<SOUTH>(checker_square, occ);
                }

                else
                {
                    if (king_rank > checker_rank && king_file > checker_file)
                        check_mask |= ray_attacks<NORTHEAST>(checker_square, occ);
                    else if (king_rank > checker_rank && king_file < checker_file)
                        check_mask |= ray_attacks<NORTHWEST>(checker_square, occ);
                    else if (king_rank < checker_rank && king_file > checker_file)
                        check_mask |= ray_attacks<SOUTHEAST>(checker_square, occ);
                    else
                        check_mask |= ray_attacks<SOUTHWEST>(checker_square, occ);
                }

                check_mask ^= king_square;
            }
        }
    }

    // generate king moves
    u64 king = board.pieces(KING, c);
    Square from = bitscan(king);
    u64 moves_bb = Constants::king_move_table[from];
    // filter out attacked squares that are occupied by one of our pieces
    moves_bb &= ~our_pieces;
    while (moves_bb)
    {
        Square to = bitscan(moves_bb);

        if constexpr (type == LEGAL)
        {
            if (opponent_attacks & to)
                continue;
        }

        moves.push_back(Move(from, to));
    }

    // in double check, only king moves are valid, so we can short circuit here
    if constexpr (type == LEGAL)
    {
        if (in_double_check)
            return moves;
    }

    // generate pawn moves
    u64 pawns = board.pieces(PAWN, c);
    u64 single_pushes = single_push_targets<c>(pawns, ~occ);

    int perspective;
    if constexpr (c == WHITE)
        perspective = -1;
    else if constexpr (c == BLACK)
        perspective = 1;
    
    while (single_pushes)
    {
        Square to = bitscan(single_pushes);
        Square from = static_cast<Square>(to + 8 * perspective);
        moves.push_back(Move(from, to));
    }

    u64 double_pushes = double_push_targets<c>(pawns, ~occ);
    while (double_pushes)
    {
        Square to = bitscan(double_pushes);
        Square from = static_cast<Square>(to + 16 * perspective);
        moves.push_back(Move(from, to));
    }

    // generate knight moves
    u64 knights = board.pieces(KNIGHT, c);
    while (knights)
    {
        Square from = bitscan(knights);
        u64 moves_bb = Constants::knight_move_table[from];
        // filter out attacked squares that are occupied by one of our pieces
        moves_bb &= ~our_pieces;
        // filter out moves not allowed because of check
        moves_bb &= check_mask;
        while (moves_bb)
        {
            Square to = bitscan(moves_bb);
            moves.push_back(Move(from, to));
        }
    }

    // generate rook moves
    u64 rooks = board.pieces(ROOK, c);
    while (rooks)
    {
        Square from = bitscan(rooks);
        u64 attacks = sliding_attacks<ROOK>(from, occ);

        // filter out attacked squares that are occupied by one of our pieces
        attacks &= ~our_pieces;
        // filter out moves not allowed because of check
        moves_bb &= check_mask;
        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to));
        }
    }

    // generate bishop moves
    u64 bishops = board.pieces(BISHOP, c);
    while (bishops)
    {
        Square from = bitscan(bishops);
        u64 attacks = sliding_attacks<BISHOP>(from, occ);

        // filter out attacked squares that are occupied by one of our pieces
        attacks &= ~our_pieces;
        // filter out moves not allowed because of check
        moves_bb &= check_mask;
        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to));
        }
    }

    // generate queen moves
    u64 queens = board.pieces(QUEEN, c);
    while (queens)
    {
        Square from = bitscan(queens);
        u64 attacks = sliding_attacks<QUEEN>(from, occ);

        // filter out attacked squares that are occupied by one of our pieces
        attacks &= ~our_pieces;
        // filter out moves not allowed because of check
        moves_bb &= check_mask;
        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to));
        }
    }

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

template <Color c>
u64 single_push_targets(u64 pawns, u64 empty)
{
    static_assert(c == WHITE || c == BLACK);

    if constexpr (c == WHITE)
        return shift<NORTH>(pawns) & empty;
    else if constexpr (c == BLACK)
        return shift<SOUTH>(pawns) & empty;
}

template <Color c>
u64 double_push_targets(u64 pawns, u64 empty)
{
    static_assert(c == WHITE || c == BLACK);

    if constexpr (c == WHITE)
    {
        u64 double_push_rank = Bitboard::RANK_BB[RANK_4];
        u64 single_pushes = single_push_targets<c>(pawns, empty);
        return shift<NORTH>(single_pushes) & empty & double_push_rank;     
    }

    else if constexpr (c == BLACK)
    {
        u64 double_push_rank = Bitboard::RANK_BB[RANK_5];
        u64 single_pushes = single_push_targets<c>(pawns, empty);    
        return shift<SOUTH>(single_pushes) & empty & double_push_rank;     
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

/**
 * @brief generates the set of all squares a color attacks
 * @param board the board
 * @param c color to generate attack set of
 * @param occupied occupied set of current board
 * @return bitboard representing all the squares c attacks
 */
static u64 attack_set(Board const &board, Color c, u64 occupied)
{
    u64 attacks = 0;

    u64 pawns = board.pieces(PAWN, c);
    while (pawns)
    {
        Square from = bitscan(pawns);
        attacks |= Constants::pawn_attack_table[c][from];
    }

    u64 knights = board.pieces(KNIGHT, c);
    while (knights)
    {
        Square from = bitscan(knights);
        attacks |= Constants::knight_move_table[from];
    }

    u64 bishops = board.pieces(BISHOP, c);
    while (bishops)
    {
        Square from = bitscan(bishops);
        attacks |= sliding_attacks<BISHOP>(from, occupied);
    }

    u64 rooks = board.pieces(ROOK, c);
    while (rooks)
    {
        Square from = bitscan(rooks);
        attacks |= sliding_attacks<ROOK>(from, occupied);
    }

    u64 queens = board.pieces(QUEEN, c);
    while (queens)
    {
        Square from = bitscan(queens);
        attacks |= sliding_attacks<QUEEN>(from, occupied);
    }

    u64 king = board.pieces(KING, c);
    Square from = bitscan(king);
    attacks |= Constants::king_move_table[from];

    return attacks;
}

/**
 * @brief calculates the set of all attackers to a color's king
 * @param board the board
 * @param c the color of the king we are checking
 * @return the set of all squares attacking c's king
 */
u64 checkers(Board const &board, Color c)
{
    u64 attackers = 0;

    // get occupancy set (all pieces)
    u64 occ = board.pieces();

    Square king_square = board.king_square(c);

    attackers |= Constants::pawn_attack_table[c][king_square] & board.pieces(PAWN, ~c);
    attackers |= Constants::knight_move_table[king_square]    & board.pieces(KNIGHT, ~c);
    attackers |= sliding_attacks<BISHOP>(king_square, occ)    & board.pieces(BISHOP, ~c);
    attackers |= sliding_attacks<ROOK>(king_square, occ)      & board.pieces(ROOK, ~c);
    attackers |= sliding_attacks<QUEEN>(king_square, occ)     & board.pieces(QUEEN, ~c);
    return attackers;
}