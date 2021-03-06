/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: main.cpp
 * DATE: January 31st, 2022
 * DESCRIPTION: main()
 */

#include "board.h"
#include "uci.h"

// global board object
Board board;

int main(int argc, char **argv)
{
    // launch uci handler
    uci();
    return 0;
}