/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: uci.h
 * DATE: February 2nd, 2022
 * DESCRIPTION: Parses UCI messages and sends responses
 * 
 * http://wbec-ridderkerk.nl/html/UCIProtocol.html
 */

#pragma once

#include <string>

// global board object
class Board;
extern Board board;

// Types of messages GUI can send to engine
enum UciType
{
	UCI,
	DEBUG,
	ISREADY,
	SETOPTION,
	REGISTER,
	UCINEWGAME,
	POSITION,
	GO,
	STOP,
	PONDERHIT,
	QUIT,

	UNKNOWN,
};

void uci();
UciType decode_msg(std::string const &);
void send_msg(std::string const &);
