/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: game.h
 * DATE: February 16th, 2022
 * DESCRIPTION: Functions that affect the global state of the game
 */

#pragma once

#include <string>

// global board object
class Board;
extern Board board;

void parse_uci_moves(std::string const &);
void parse_uci_move(std::string const &);
void load_fen(std::string const &);
