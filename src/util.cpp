/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: util.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: Utility functions not fitting in another class
 */

#include "util.h"
#include "bitboard.h"

#include <iostream>

/**
 * @brief gets the rank & file of a square on the board
 * @param square board-index of square to get
 * @return tuple of { rank, file } of square
 * 
 * example invocation:
 * auto [rank, file] = rank_file_from_square(11) => [1, 3]
 */
std::tuple<int, int> Util::rank_file_from_square(Square square)
{
	int rank = square / 8;
	int file = square % 8;
	return std::make_tuple(rank, file);
}

/**
 * @brief gets the square on the board given a rank and file
 * @param rank rank of square [0, 8)
 * @param file file of square [0, 8)
 * @return Square corresponding to rank & file
 * 
 * this function is the inverse of rank_file_from_square
 * 
 * example invocation:
 * auto square = square_from_rank_file(1, 3) => D2
 */
Square Util::square_from_rank_file(int rank, int file)
{
	return Square(rank * 8 + file);
}

/**
 * @brief gets the algebraic notation representation of a square
 * @param square board-index of square to turn to algebraic notation
 * @return the algebraic representation of the square
 * 
 * example invocation:
 * auto algebraic = to_algebraic(E4) => "e4"
 */
std::string Util::to_algebraic(Square square)
{
	std::string res   = "";
	auto [rank, file] = rank_file_from_square(square);
	res += file + 'a';
	res += std::to_string(rank + 1);
	return res;
}

/**
 * @brief gets the square given the algebraic notation
 * @param square 2 character string of the algebraic notation
 * @return Square corresponding to the algebraic notation
 * 
 * this function is the inverse of to_algebraic
 * 
 * example invocation:
 * auto square = from_algebraic("e4") => E4
 */
Square Util::from_algebraic(std::string const &square)
{
	int file = square.at(0) - 'a';
	int rank = square.at(1) - '1';
	return square_from_rank_file(rank, file);
}

void Util::print_bitboard(u64 bitboard)
{
	for (int rank = RANK_8; rank >= RANK_1; --rank)
	{
		for (int file = FILE_A; file <= FILE_H; ++file)
		{
			Square s = static_cast<Square>(rank * 8 + file);
			char c = bitboard & s ? '1' : '*';
			std::cout << c << ' ';
		}
		std::cout << '\n';
	}
}
