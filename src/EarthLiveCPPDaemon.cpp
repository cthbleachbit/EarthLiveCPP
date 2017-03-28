/**
 * ==========================================================
 * EarthLiveCPP
 * Backend process
 * 
 * Copyright (C) Tianhao Chai (cth451) <cth451@gmail.com>
 * 
 * Inspired by bitdust's [ https://github.com/bitdust ]
 * Licensed under LGPL 3
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 * ==========================================================
**/

#include "lib/io.hpp"
#include <iostream>

void printHelp() {
	std::cerr << "Usage: elcpp-daemon <interval> <density> <debug>" << std::endl;
	std::cerr << "\tinterval:    refresh interval in minutes." << std::endl;
	std::cerr << "\tdensity:     numerical value for image tiles." << std::endl;
	std::cerr << "\tdebug:       1 for debugging logs in log file" << std::endl;
}

void refreshRoutine() {

}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Need 3 arguments." << std::endl;
		return 1;
	}
}
