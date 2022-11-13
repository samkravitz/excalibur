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

	// time left in the game, total game time in milliseconds
	int time_left, game_time;

	for (int i = 1; i < argc; ++i)
	{
		auto arg = std::string(argv[i]);

		if (arg == "-g")
			game_time = atoi(argv[++i]);

		else if (arg == "-t")
			time_left = atoi(argv[++i]);

		else
			parse_uci_move(std::string(arg));
	}

	const auto [move, eval] = search_time(game_time, time_left);

	std::cout << move << "\n";
	return 0;
}
