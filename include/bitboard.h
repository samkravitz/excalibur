/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: bitboard.h
 * DATE: January 31st, 2022
 * DESCRIPTION: bitboard utility and helper functions
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

#include "constants.h"
#include "types.h"

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

// just like the other bitscan except it doesn't modify its argument
template<BitscanDirection d = FORWARD>
Square bitstcan_cp(u64 bitboard)
{
    static_assert(d == FORWARD || d == REVERSE);
    assert(bitboard != 0);

    int idx = 0;
    if constexpr (d == FORWARD)
        idx = __builtin_ctzll(bitboard);
    else if constexpr (d == REVERSE)
        idx = 63 - __builtin_clzll(bitboard);
        
    return static_cast<Square>(idx);
}

template<Direction d>
u64 shift(u64 bitboard)
{
    int shift_amount = Constants::DIRECTION_VALUE[d];

    // positive direction - shift left
    if constexpr (d == EAST || d == NORTH || d == NORTHEAST || d == NORTHWEST)
        bitboard <<= shift_amount;
    
    // negative direction - shift right
    else if constexpr (d == WEST || d == SOUTH || d == SOUTHWEST || d == SOUTHEAST)
        bitboard >>= -1 * shift_amount;

    return bitboard;
}

inline u64 operator&(u64 bitboard, Square square) { return bitboard & (1ull << square); }
inline u64 operator|(u64 bitboard, Square square) { return bitboard | (1ull << square); }
inline u64 operator^(u64 bitboard, Square square) { return bitboard ^ (1ull << square); }

inline u64 &operator&=(u64 &bitboard, Square square) { bitboard = bitboard & (1ull << square); return bitboard; }
inline u64 &operator|=(u64 &bitboard, Square square) { bitboard = bitboard | (1ull << square); return bitboard; }
inline u64 &operator^=(u64 &bitboard, Square square) { bitboard = bitboard ^ (1ull << square); return bitboard; }

namespace Bitboard
{
    constexpr u64 RANK_BB[8] = {
        0x00000000000000ff,
        0x000000000000ff00,
        0x0000000000ff0000,
        0x00000000ff000000,
        0x000000ff00000000,
        0x0000ff0000000000,
        0x00ff000000000000,
        0xff00000000000000,
    };

    constexpr u64 FILE_BB[8] = {
        0x0101010101010101, // A file
        0x0202020202020202, // B file
        0x0404040404040404, // C file
        0x0808080808080808, // D file
        0x1010101010101010, // E file
        0x2020202020202020, // F file
        0x4040404040404040, // G file
        0x8080808080808080, // H file
    };
}