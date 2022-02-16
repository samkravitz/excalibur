/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: movegen.h
 * DATE: January 31st, 2022
 * DESCRIPTION: Generation of legal and pseudolegal moves
 */

#pragma once

#include <vector>

#include "board.h"
#include "move.h"
#include "movelist.h"

enum MovegenType
{
    ALL,
    CAPTURES,
};

Movelist generate_moves(Board const &);
Movelist generate_captures(Board const &);

template<Color, MovegenType = ALL>
Movelist movegen(Board const &);

template <Direction>
u64 ray_attacks(Square, u64);

template <PieceType>
u64 sliding_attacks(Square, u64);

template <PieceType>
u64 xray_attacks(u64, u64, Square);

template<Color>
u64 single_push_targets(u64, u64);

template<Color>
u64 double_push_targets(u64, u64);

u64 diagonal_attacks(Square, u64);
u64 antidiagonal_attacks(Square, u64);
u64 rank_attacks(Square, u64);
u64 file_attacks(Square, u64);

template <Color>
static u64 attack_set(u64, u64);

template <PieceType, Color>
static u64 attack_set(u64, u64);

u64 checkers(Board const &, Color);
