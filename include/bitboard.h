/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: bitboard.h
 * DATE: January 31st, 2022
 * DESCRIPTION: bitboard representation
 * 
 * A bitboard is a bit-array representation of every square on a chess board.
 * Since there are 64 squares on a chess board, we can use a 64 bit integer
 * to represent every square in a single CPU word. To keep track of the entire
 * board, we will keep track of bitboards for every piece of every type, ie
 * a bitboard for all the white pawns, for the black king, etc.
 * https://www.chessprogramming.org/Bitboards
 */

#pragma once

#include <cassert>
#include <functional>

#include "bitset_iter.h"
#include "types.h"

// bitset iterator defined in bitset_iter.h
using bitset_iter = bitset::index_iterable<64, const std::bitset<64>, true>;

bitset_iter indeces_set(u64);

void print_bitboard(u64);

inline u64 operator&(u64 bitboard, Square square) { return bitboard & (1ull << square); }
inline u64 operator|(u64 bitboard, Square square) { return bitboard | (1ull << square); }
inline u64 operator^(u64 bitboard, Square square) { return bitboard ^ (1ull << square); }

inline u64 &operator&=(u64 &bitboard, Square square) { bitboard = bitboard & (1ull << square); return bitboard; }
inline u64 &operator|=(u64 &bitboard, Square square) { bitboard = bitboard | (1ull << square); return bitboard; }
inline u64 &operator^=(u64 &bitboard, Square square) { bitboard = bitboard ^ (1ull << square); return bitboard; }

enum BitscanDirection
{
    FORWARD,
    REVERSE,
};

template<BitscanDirection d = FORWARD>
Square bitscan(u64 &bitboard)
{
    static_assert(d == FORWARD || d == REVERSE);
    assert(bitboard != 0);

    int idx = 0;
    if constexpr (d == FORWARD)
        idx = __builtin_ctzll(bitboard);
    else if constexpr (d == REVERSE)
        idx = 63 - __builtin_clzll(bitboard);
    
    bitboard &= bitboard - 1;
    
    return static_cast<Square>(idx);
}
