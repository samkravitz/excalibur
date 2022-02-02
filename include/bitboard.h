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

#include <functional>

#include "bitset_iter.h"
#include "types.h"

// bitset iterator defined in bitset_iter.h
using bitset_iter = bitset::index_iterable<64, const std::bitset<64>, true>;

class Bitboard
{
public:
    Bitboard() = default;
    Bitboard(u64);

    void set(u64);

    // provides access to bitset_iter api
    inline bitset_iter indeces_set() { return bitset::indices_on(std::bitset<64>(bitboard)); }

    // overloads of bitwise operations on bitboards
    inline Bitboard operator&(Bitboard const &other) const { return Bitboard(bitboard & other.bitboard); }
    inline Bitboard operator|(Bitboard const &other) const { return Bitboard(bitboard | other.bitboard); }
    inline Bitboard operator^(Bitboard const &other) const { return Bitboard(bitboard ^ other.bitboard); }

    inline Bitboard &operator&=(Bitboard const &other) { bitboard &= other.bitboard; return *this; }
    inline Bitboard &operator|=(Bitboard const &other) { bitboard |= other.bitboard; return *this; }
    inline Bitboard &operator^=(Bitboard const &other) { bitboard ^= other.bitboard; return *this; }
private:
    u64 bitboard;
};
