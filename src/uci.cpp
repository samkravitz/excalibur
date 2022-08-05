/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: uci.cpp
 * DATE: February 2nd, 2022
 * DESCRIPTION: Parses UCI messages and sends responses
 */
#include "uci.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

void uci()
{
	while (1)
	{
		// get entire line of input
		std::string line;
		while (std::getline(std::cin, line))
		{
			// split line on space delimiter
			std::vector<std::string> words;
			std::string word = "";
			std::stringstream ss(line);
			while (std::getline(ss, word, ' '))
				words.push_back(word);

			switch (decode_msg(words[0]))
			{
				case UCI:
					send_msg("id name excalibur 0.0.1");
					send_msg("id author Sam Kravitz");
					send_msg("uciok");
					break;
				case DEBUG:
				case ISREADY:
					send_msg("readyok");
					break;
				case UCINEWGAME:
					// start a new game in the initial position
					send_msg("readyok");
					break;
				case POSITION:
					// position is starting from a fen
					if (words[1] == "fen")
					{
						// loadfen(words[1])
					}

					// position is starting from start position
					else if (words[1] == "startpos")
					{
					}

					else
					{
						std::cerr << line << "\n";
						assert(!"Bad input to position uci command");
					}
					break;
				case GO:
					send_msg("info score cp -30");
					break;
				case STOP:
					return;
				case UNKNOWN:
				default:
					std::cerr << "Unrecognized message: " << line << "\n";
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

UciType decode_msg(std::string const &msg)
{
	if (msg == "uci")
		return UCI;
	else if (msg == "debug")
		return DEBUG;
	else if (msg == "isready")
		return ISREADY;
	else if (msg == "ucinewgame")
		return UCINEWGAME;
	else if (msg == "position")
		return POSITION;
	else if (msg == "go")
		return GO;
	else if (msg == "stop")
		return STOP;
	else
		return UNKNOWN;
}

void send_msg(std::string const &msg)
{
	std::cout << msg << "\n";
}
