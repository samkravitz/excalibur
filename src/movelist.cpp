/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: movelist.cpp
 * DATE: February 16th, 2022
 * DESCRIPTION: wrapper class for holding all the possible moves in a position
 */
#include "movelist.h"

Movelist::Movelist()
{
	m_size = 0;
}

void Movelist::add(Move mv)
{
	movelist[m_size++] = mv;
}
