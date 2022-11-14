/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: polyglot.h
 * DATE: November 14th, 2022
 * DESCRIPTION: Implements the polyglot opening book binary format
 */

#include "polyglot.h"

#include <fstream>
#include <iostream>

#include "zobrist.h"

static u64 swap64(u64 x)
{
	x = (x & 0x00000000ffffffff) << 32 | (x & 0xffffffff00000000) >> 32;
	x = (x & 0x0000ffff0000ffff) << 16 | (x & 0xffff0000ffff0000) >> 16;
	x = (x & 0x00ff00ff00ff00ff) << 8 | (x & 0xff00ff00ff00ff00) >> 8;
	return x;
}

static u32 swap32(u32 x)
{
	return ((x >> 24) & 0xff) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | ((x << 24) & 0xff000000);
}

static u16 swap16(u16 x)
{
	return (x >> 8) | (x << 8);
}

Move polyglot(const Board &board)
{
	const std::string book_name = "./book/human.bin";
	u64 key;
	u16 move;
	u16 weight;
	u32 learn;

	auto hash = zobrist(board);

	std::ifstream book(book_name, std::ios::in | std::ios::binary);
	if (!book.good())
	{
		std::cerr << "Error opening book " << book_name << "\n";
		return false;
	}

	while (book.peek() != EOF)
	{
		book.read((char *) &key, sizeof(key));
		book.read((char *) &move, sizeof(move));
		book.read((char *) &weight, sizeof(weight));
		book.read((char *) &learn, sizeof(learn));

		if (swap64(key) == hash)
			return Move(swap16(move));
	}

	return Move();
}
