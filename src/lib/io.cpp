/**
 * ==========================================================
 * EarthLiveCPP
 * I/O functions library
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
 */

#include "io.hpp"

#include <string>
#include <ctime>
#include <fstream>

ioInstance::ioInstance(std::string &logFilePath) {
	(this -> logHandle).open(logFilePath);
};

/**
 * @brief write a line of log to disk
 * 
 * @param infType
 *        type of this piece of log, 1 means info, 2 means error.
 * @param info
 *        the string for output
 */
void ioInstance::log(int &infType, std::string &info) {
	std::time_t currentTime = std::time(nullptr);
	switch (infType) {
	case 1:	this -> logHandle << "[Inf] ";
	case 2: this -> logHandle << "[Err] ";
	}
	this -> logHandle << std::asctime(std::gmtime(&currentTime)) << info << std::endl;
};
