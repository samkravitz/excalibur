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

    constexpr Color opp_color = c == WHITE ? BLACK : WHITE;

    // get all pieces of opponents color
    u64 opponents = occ & ~our_pieces;

    Square king_square = board.king_square(c);

    auto make_flags = [&](Square to) -> MoveFlags
    {
        return board.piece_on(to) == NONE ? QUIET_MOVE : CAPTURE;
    };

    // declare variables needed for legal move generation
    u64 occ_without_king;                 // get occupancy set without our king (used for calculating legal moves)
    u64 opponent_attacks;                 // opponents attack set
    u64 checks;                           // set of squares attacking our king
    u64 check_mask  = 0xffffffffffffffff; // set of squares we are allowed to move to due to check
    u64 pinners;                          // set of squares on which is a piece pinning us
    u64 pinned = 0;                       // set of squares on which we haved a pinned piece
    u64 pinner_rays = 0xffffffffffffffff; // set of all rays from a pinner to our king (with pinned piece remove)
    bool in_check, in_double_check;
    
    if constexpr (type == LEGAL)
    {
        occ_without_king = occ ^ board.pieces(KING, c);
        opponent_attacks = attack_set<opp_color>(board, occ_without_king);
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
                switch (Constants::dir_lookup_table[checker_square][king_square])
                {
                    case NORTH:     check_mask |= ray_attacks<NORTH>(checker_square, occ);     break;
                    case SOUTH:     check_mask |= ray_attacks<SOUTH>(checker_square, occ);     break;
                    case EAST:      check_mask |= ray_attacks<EAST>(checker_square, occ);      break;
                    case WEST:      check_mask |= ray_attacks<WEST>(checker_square, occ);      break;
                    case NORTHEAST: check_mask |= ray_attacks<NORTHEAST>(checker_square, occ); break;
                    case NORTHWEST: check_mask |= ray_attacks<NORTHWEST>(checker_square, occ); break;
                    case SOUTHEAST: check_mask |= ray_attacks<SOUTHEAST>(checker_square, occ); break;
                    case SOUTHWEST: check_mask |= ray_attacks<SOUTHWEST>(checker_square, occ); break;
                }

                check_mask ^= king_square;
            }
        }

        // calculate pinned set

        // get opponent's rook/queen and bishop/queen sets
        u64 opRQ = board.pieces(ROOK, ~c)   | board.pieces(QUEEN, ~c);
        u64 opBQ = board.pieces(BISHOP, ~c) | board.pieces(QUEEN, ~c);

        pinners = xray_attacks<ROOK>(occ, our_pieces, king_square) & opRQ;
        pinners |= xray_attacks<BISHOP>(occ, our_pieces, king_square) & opBQ;

        // if there are pinners, calculate the pinned set
        if (pinners)
        {
            u64 tmp = pinners;
            while (tmp)
            {
                Square from = bitscan(tmp);
                u64 ray;
                switch (Constants::dir_lookup_table[from][king_square])
                {
                    case NORTH:    
                        ray = ray_attacks<NORTH>(from, occ);
                        pinned |= bitstcan_cp(ray & our_pieces);
                        break;
                    case SOUTH:    
                        ray = ray_attacks<SOUTH>(from, occ);
                        pinned |= bitstcan_cp<REVERSE>(ray & our_pieces);
                        break;
                    case EAST:     
                        ray = ray_attacks<EAST>(from, occ);
                        pinned |= bitstcan_cp(ray & our_pieces);
                        break;
                    case WEST:     
                        ray = ray_attacks<WEST>(from, occ);
                        pinned |= bitstcan_cp<REVERSE>(ray & our_pieces);
                        break;
                    case NORTHEAST:
                        ray = ray_attacks<NORTHEAST>(from, occ);
                        pinned |= bitstcan_cp(ray & our_pieces);
                        break;
                    case NORTHWEST:
                        ray = ray_attacks<NORTHWEST>(from, occ);
                        pinned |= bitstcan_cp(ray & our_pieces);
                        break;
                    case SOUTHEAST:
                        ray = ray_attacks<SOUTHEAST>(from, occ);
                        pinned |= bitstcan_cp<REVERSE>(ray & our_pieces);
                        break;
                    case SOUTHWEST:
                        ray = ray_attacks<SOUTHWEST>(from, occ);
                        pinned |= bitstcan_cp<REVERSE>(ray & our_pieces);
                        break;
                }
            }

            // calculate pinner rays
            pinner_rays = 0;

            // occupancy set exluded our pinned pieces
            u64 occ_without_pinned = occ ^ pinned;
            while (pinners)
            {
                Square from = bitscan(pinners);
                pinner_rays |= from;
                switch (Constants::dir_lookup_table[from][king_square])
                {
                    case NORTH:     pinner_rays |= ray_attacks<NORTH>(from, occ_without_pinned);     break;
                    case SOUTH:     pinner_rays |= ray_attacks<SOUTH>(from, occ_without_pinned);     break;
                    case EAST:      pinner_rays |= ray_attacks<EAST>(from, occ_without_pinned);      break;
                    case WEST:      pinner_rays |= ray_attacks<WEST>(from, occ_without_pinned);      break;
                    case NORTHEAST: pinner_rays |= ray_attacks<NORTHEAST>(from, occ_without_pinned); break;
                    case NORTHWEST: pinner_rays |= ray_attacks<NORTHWEST>(from, occ_without_pinned); break;
                    case SOUTHEAST: pinner_rays |= ray_attacks<SOUTHEAST>(from, occ_without_pinned); break;
                    case SOUTHWEST: pinner_rays |= ray_attacks<SOUTHWEST>(from, occ_without_pinned); break;
                }
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

        moves.push_back(Move(from, to, make_flags(to)));
    }

    // in double check, only king moves are valid, so we can short circuit here
    if constexpr (type == LEGAL)
    {
        if (in_double_check)
            return moves;
    }

    // generate castle moves
    if (!in_check)
    {
        // check for kingside castle
        if (board.get_castle_rights(c, KINGSIDE))
        {
            constexpr Square s1 = c == WHITE ? F1 : F8;
            constexpr Square s2 = c == WHITE ? G1 : G8;

            bool castling_impeded_by_check = false;
            bool castling_impeded_by_piece = false;

            // castling impeded by check
            if (opponent_attacks & s1 || opponent_attacks & s2)
                castling_impeded_by_check = true;
            
            // castling impeded by another piece
            if (occ & s1 || occ & s2)
                castling_impeded_by_piece = true;

            if (!castling_impeded_by_check && !castling_impeded_by_piece)
                moves.push_back(Move(king_square, s2, KINGSIDE_CASTLE));
        }

        // check for queenside castle
        if (board.get_castle_rights(c, QUEENSIDE))
        {
            constexpr Square s1 = c == WHITE ? D1 : D8;
            constexpr Square s2 = c == WHITE ? C1 : C8;

            bool castling_impeded_by_check = false;
            bool castling_impeded_by_piece = false;

            // castling impeded by check
            if (opponent_attacks & s1 || opponent_attacks & s2)
                castling_impeded_by_check = true;
            
            // castling impeded by another piece
            if (occ & s1 || occ & s2)
                castling_impeded_by_piece = true;

            if (!castling_impeded_by_check && !castling_impeded_by_piece)
                moves.push_back(Move(king_square, s2, QUEENSIDE_CASTLE));
        }
    }

    // generate pawn pushes
    u64 pawns = board.pieces(PAWN, c);
    constexpr int perspective = c == WHITE ? -1 : 1;

    u64 single_pushes = single_push_targets<c>(pawns, ~occ);
    single_pushes &= check_mask;
    constexpr u64 promotion_rank = c == WHITE ? Bitboard::RANK_BB[RANK_8] : Bitboard::RANK_BB[RANK_1];
    while (single_pushes)
    {
        Square to = bitscan(single_pushes);
        Square from = static_cast<Square>(to + 8 * perspective);

        // check if pawn is pinned and if it's unable to push
        if (pinned & from && !(pinner_rays & to))
            continue;

        // single push is a promotion
        if (promotion_rank & to)
        {
            moves.push_back(Move(from, to, QUEEN_PROMOTION));
            moves.push_back(Move(from, to, BISHOP_PROMOTION));
            moves.push_back(Move(from, to, ROOK_PROMOTION));
            moves.push_back(Move(from, to, KNIGHT_PROMOTION));
        }

        else
        {
            moves.push_back(Move(from, to));
        }
    }

    u64 double_pushes = double_push_targets<c>(pawns, ~occ);
    double_pushes &= check_mask;
    while (double_pushes)
    {
        Square to = bitscan(double_pushes);
        Square from = static_cast<Square>(to + 16 * perspective);

        // check if pawn is pinned and if it's unable to push
        if (pinned & from && !(pinner_rays & to))
            continue;

        moves.push_back(Move(from, to));
    }

    while (pawns)
    {
        Square from = bitscan(pawns);
        u64 attacks = Constants::pawn_attack_table[c][from] & opponents;
        attacks &= check_mask;
        while (attacks)
        {
            Square to = bitscan(attacks);

            // check if pawn is pinned and if it's unable to attack
            if (pinned & from && !(pinner_rays & to))
                continue;

            // capture is a promotion
            if (promotion_rank & to)
            {
                moves.push_back(Move(from, to, QUEEN_PROMO_CAPTURE));
                moves.push_back(Move(from, to, BISHOP_PROMO_CAPTURE));
                moves.push_back(Move(from, to, ROOK_PROMO_CAPTURE));
                moves.push_back(Move(from, to, KNIGHT_PROMO_CAPTURE));
            }

            else
            {
                moves.push_back(Move(from, to, CAPTURE));
            }
        }
    }

    // generate enpassant moves
    if (board.get_ep_sq() != EP_NONE)
    {
        u64 pawns = board.pieces(PAWN, c);
        while (pawns)
        {
            Square from = bitscan(pawns);

            // check if there is an enemy pawn on either side of us

            // moving pawn is not on A file
            if (~Bitboard::FILE_BB[FILE_A] & from)
            {
                Square one_left = static_cast<Square>(from - 1);

                // there is an enemy pawn immediately to the left of us
                if (board.pieces(PAWN, ~c) & one_left)
                {
                    Square to = c == WHITE ? static_cast<Square>(one_left + 8) : static_cast<Square>(one_left - 8);
                    moves.push_back(Move(from, to, ENPASSANT));
                }   
            }

            // moving pawn is not on H file
            if (~Bitboard::FILE_BB[FILE_H] & from)
            {
                Square one_right = static_cast<Square>(from + 1);

                // there is an enemy pawn immediately to the right of us
                if (board.pieces(PAWN, ~c) & one_right)
                {
                    Square to = c == WHITE ? static_cast<Square>(one_right + 8) : static_cast<Square>(one_right - 8);
                    moves.push_back(Move(from, to, ENPASSANT));
                }
            }
        }
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
            moves.push_back(Move(from, to, make_flags(to)));
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
        attacks &= check_mask;

        if (pinned & from)
            attacks &= pinner_rays;

        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to, make_flags(to)));
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
        attacks &= check_mask;
        
        if (pinned & from)
            attacks &= pinner_rays;

        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to, make_flags(to)));
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
        attacks &= check_mask;
        
        if (pinned & from)
            attacks &= pinner_rays;

        while (attacks)
        {
            Square to = bitscan(attacks);
            moves.push_back(Move(from, to, make_flags(to)));
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

/**
 * @brief get xray attacks
 * @param occ occupied set of the board
 * @param blockers set of blockers of the xray (subset of occ)
 * @param square square on which the xrayer is on
 * @return set of xray attacks
 */
template <PieceType pt>
u64 xray_attacks(u64 occ, u64 blockers, Square square)
{
    static_assert(pt == ROOK || pt == BISHOP);

    u64 attacks = sliding_attacks<pt>(square, occ);
    blockers &= attacks;
    return attacks ^ sliding_attacks<pt>(square, occ ^ blockers);
}

template <Color c>
u64 single_push_targets(u64 pawns, u64 empty)
{
    static_assert(c == WHITE || c == BLACK);

    constexpr Direction shift_dir = c == WHITE ? NORTH : SOUTH;
    return shift<shift_dir>(pawns) & empty;
}

template <Color c>
u64 double_push_targets(u64 pawns, u64 empty)
{
    static_assert(c == WHITE || c == BLACK);

    constexpr Direction shift_dir = c == WHITE ? NORTH : SOUTH;
    constexpr u64 double_push_rank = c == WHITE ? Bitboard::RANK_BB[RANK_4] : Bitboard::RANK_BB[RANK_5];
    u64 single_pushes = single_push_targets<c>(pawns, empty);
    return shift<shift_dir>(single_pushes) & empty & double_push_rank;
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
 * @param c color to generate attack set of
 * @param board the board
 * @param occupied occupied set of entire board
 * @return bitboard representing all the squares pt attacks
 */
template <Color c>
static u64 attack_set(Board const &board, u64 occupied)
{
    return (
        attack_set<PAWN,   c>(board.pieces(PAWN, c),   occupied) |
        attack_set<KNIGHT, c>(board.pieces(KNIGHT, c), occupied) |
        attack_set<BISHOP, c>(board.pieces(BISHOP, c), occupied) |
        attack_set<ROOK,   c>(board.pieces(ROOK, c),   occupied) |
        attack_set<QUEEN,  c>(board.pieces(QUEEN, c),  occupied) |
        attack_set<KING,   c>(board.pieces(KING, c),   occupied)
    );
}

/**
 * @brief generates the set of all squares a type of piece attacks
 * @param pt the type of piece to generate the attack set of
 * @param c color to generate attack set of
 * @param piece_set the set of all pt of color c
 * @param occupied occupied set of entire board
 * @return bitboard representing all the squares pt attacks
 */
template <PieceType pt, Color c>
static u64 attack_set(u64 piece_set, u64 occupied)
{
    static_assert(pt == PAWN || pt == KNIGHT || pt == KING || pt == BISHOP || pt == ROOK || pt == QUEEN);

    u64 attacks = 0;

    while (piece_set)
    {
        Square from = bitscan(piece_set);
        if constexpr (pt == PAWN)
            attacks |= Constants::pawn_attack_table[c][from];
        
        else if constexpr (pt == KNIGHT)
            attacks |= Constants::knight_move_table[from];
        
        else if constexpr (pt == KING)
            attacks |= Constants::king_move_table[from];
        
        else if constexpr (pt == BISHOP || pt == ROOK || pt == QUEEN)
            attacks |= sliding_attacks<pt>(from, occupied);
    }

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