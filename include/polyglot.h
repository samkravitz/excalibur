/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: polyglot.h
 * DATE: November 14th, 2022
 * DESCRIPTION: Implements the polyglot opening book binary format
 */

#pragma once

#include <unordered_map>

#include "board.h"
#include "move.h"
#include "types.h"

struct PolyglotEntry
{
	u64 key;
	u16 move;
	u16 weight;
	u32 learn;
};

Move polyglot(Board const &);
