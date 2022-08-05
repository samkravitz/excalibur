/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: util.h
 * DATE: January 31st, 2022
 * DESCRIPTION: Utility functions not fitting in another class
 */

#pragma once

#include <random>
#include <string>
#include <tuple>

#include "types.h"

namespace Util
{
	std::tuple<int, int> rank_file_from_square(Square);
	Square square_from_rank_file(int, int);
	std::string to_algebraic(Square);
	Square from_algebraic(std::string const &);
	void print_bitboard(u64);

	template<typename T>
	T get_random_element(std::vector<T> const &vec)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> uni(0, vec.size() - 1);

		return vec[uni(rng)];
	}
}
