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

#include <string>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "lib/io.hpp"

const std::string SITE = "http://himawari8-dl.nict.go.jp/himawari8/img/D531106/";
const std::string DAEMON_LOG = "/.earthlivecpp/daemon.log";

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

void printHelp() {
	std::cerr << "Usage: elcppd <interval> <density> <debug>" << std::endl;
	std::cerr << "\tinterval:    refresh interval in minutes." << std::endl;
	std::cerr << "\tdensity:     numerical value for image tiles." << std::endl;
	std::cerr << "\tdebug:       1 for debugging logs in log file" << std::endl;
}

void downloadRoutine(std::string& url, std::ofstream& file) {
	CURL *curl;
	CURLcode res;
}

int main(int argc, char *argv[]) {
	// setting up logging
	
	// check update
	curl_global_init(CURL_GLOBAL_DEFAULT);
	if (argc != 4) {
		std::cerr << "Need 3 arguments. Got " << argc - 1 << "." << std::endl;
		return 1;
	}
}
