/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: search.h
 * DATE: February 16th, 2022
 * DESCRIPTION: Search & evaluation functionality
 */

#pragma once

#include <functional>
#include <tuple>

#include "move.h"

// global board object
class Board;
extern Board board;

float alphabeta(int, float, float);
float quiesce(float, float);
std::tuple<Move, float> search(int);
std::tuple<Move, float> search_time(int, int);

float evaluate();
