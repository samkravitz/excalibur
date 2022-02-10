/** excalibur
 * License: GPLv2
 * See LICENSE for full license text
 * Author: Sam Kravitz
 * 
 * FILE: perft.h
 * DATE: February 4th, 2022
 * DESCRIPTION: performance testing for move generation
 */

#pragma once

#include <iostream>
#include <string>

struct PerftDetail
{
    int nodes             = 0;
    int captures          = 0;
    int enpassants        = 0;
    int castles           = 0;
    int checks            = 0;
    int discovered_checks = 0;
    int double_checks     = 0;
    int checkmates        = 0;

    friend std::ostream &operator<<(std::ostream &os, const PerftDetail &pd)
    {
        os << "{\n";
        os << "\tnodes:             " << pd.nodes             << "\n";
        os << "\tcaptures:          " << pd.captures          << "\n";
        os << "\tenpassants:        " << pd.enpassants        << "\n";
        os << "\tcastles:           " << pd.castles           << "\n";
        os << "\tchecks:            " << pd.checks            << "\n";
        os << "\tdiscovered_checks: " << pd.discovered_checks << "\n";
        os << "\tdouble_checks:     " << pd.double_checks     << "\n";
        os << "\tcheckmates:        " << pd.checkmates        << "\n";
        os << "}\n";
        return os;
    }
};

int perft(int, std::string fen = "");
PerftDetail perft_detail(int, std::string fen = "");
