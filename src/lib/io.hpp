/**
 * ==========================================================
 * EarthLiveCPP
 * I/O functions library header
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

#ifndef ELC_IO_HPP
#define ELC_IO_HPP

#include <string>
#include <ctime>
#include <boost/filesystem.hpp>

class ioInstance {

private:
	boost::filesystem::ofstream logHandle;

public:
	/**
	 * @brief constructor
	 * 
	 * @param logFilePath
	 *        log file path.
	 */
	ioInstance(const std::string &logFilePath);

	/**
	 * @brief write a line of log to disk
	 * 
	 * @param infType
	 *        type of this piece of log, 1 means info, 2 means error.
	 * @param info
	 *        the string for output
	 */
	void log(int &verboseLvl, std::string &info);
};

#endif
