/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: main.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: main()
 */

#include <iostream>

#include "board.h"
#include "game.h"
#include "search.h"
#include "uci.h"

// global board object
Board board;

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		uci();
		return 0;
	}

	for (int i = 1; i < argc; ++i)
		parse_uci_move(std::string(argv[i]));

	const auto [move, eval] = search(5);

	std::cout << move << "\n";
	return 0;
}
