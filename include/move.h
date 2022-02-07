/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: move.h
 * DATE: January 31st, 2022
 * DESCRIPTION: An encoding of a move
 * 
 * A move can be uniquely identified with the following 3 pieces of information:
 * from square, to square, and a flag for special cases such as promotions.
 * This information is encoded into a 16 bit integer.
 * To square and from square require only 6 bits each (0 - 63), so they occupy
 * bits 0-5 and 6-10 respectively. Bits 12-15 are used to encode special moves.
 * 
 * https://www.chessprogramming.org/Encoding_Moves
 */

#pragma once

#include "util.h"
#include "types.h"

struct Move
{
public:
    Move(Square from, Square to, MoveFlags flags = QUIET_MOVE)
    {
        move_enc = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
    }

    Move() { move_enc = 0; }

    Move(int from, int to) : Move(static_cast<Square>(from), static_cast<Square>(to)) { }

    inline Square to()       const { return Square(move_enc >> 0 & 0x3f); }
    inline Square from()     const { return Square(move_enc >> 6 & 0x3f); }
    inline MoveFlags flags() const { return MoveFlags(move_enc >> 12 & 0xf); }

    std::string to_string()
    {
        std::string res = "";
        res += "{ ";
        res += Util::to_algebraic(from());
        res += Util::to_algebraic(to());
        res += " }";
        return res;
    }

private:
    u16 move_enc;
};
