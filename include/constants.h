/* excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: constants.h
 * DATE: January 31st, 2022
 * DESCRIPTION: lookup tables of bitboard constants for some piece moves & attacks
 * to avoid unnecessary calculation at runtime
 */

#include "types.h"

namespace Constants
{
    // bitboard representations of initial positions of pieces
    constexpr u64 WPAWNS_INIT   = 0x000000000000ff00;
    constexpr u64 BPAWNS_INIT   = 0x00ff000000000000;
    constexpr u64 WKNIGHTS_INIT = 0x0000000000000042;
    constexpr u64 BKNIGHTS_INIT = 0x4200000000000000;
    constexpr u64 WBISHOPS_INIT = 0x0000000000000024;
    constexpr u64 BBISHOPS_INIT = 0x2400000000000000;
    constexpr u64 WROOKS_INIT   = 0x0000000000000081;
    constexpr u64 BROOKS_INIT   = 0x8100000000000000;
    constexpr u64 WQUEENS_INIT  = 0x0000000000000008;
    constexpr u64 BQUEENS_INIT  = 0x0800000000000000;
    constexpr u64 WKING_INIT    = 0x0000000000000010;
    constexpr u64 BKING_INIT    = 0x1000000000000000;
};
