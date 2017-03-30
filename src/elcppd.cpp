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

#include <stdio.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <curl/curl.h>

#include "lib/io.hpp"

const std::string SITE = "http://himawari8-dl.nict.go.jp/himawari8/img/D531106/";

static size_t write_data(void *contents, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(contents, size, nmemb, (FILE *)stream);
	return written;
}

// regular help usage
void printHelp() {
	std::cerr << "Usage: elcppd <interval> <density> <debug>" << std::endl;
	std::cerr << "\tinterval:    refresh interval in minutes." << std::endl;
	std::cerr << "\tdensity:     numerical value for image tiles." << std::endl;
	std::cerr << "\tdebug:       1 for debugging logs in log file" << std::endl;
}

// The function that actually downloads, returns a CURLcode. Not 0 = fail
int downloadRoutine(std::string& url, std::string& file) {
	CURL *curl;
	FILE *out;
	CURLcode retCode;
	const char *c_url = url.c_str();
	const char *c_fname = file.c_str();
	curl = curl_easy_init();
	if (curl) {// if successfully created curl struct
		out = fopen(c_fname, "wb"); // open with write + binary
		curl_easy_setopt(curl, CURLOPT_URL, c_url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
		retCode = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(out);
	}
	return retCode;
}

int main(int argc, char *argv[]) {
	// setting up logging and storage space
	const std::string HOME_DIR = getenv("HOME");
	const std::string CONFIG_DIR = HOME_DIR + "/.earthlivecpp/";
	boost::filesystem::path confDir(CONFIG_DIR);
	if (!boost::filesystem::exists(confDir)) {
		boost::filesystem::create_directory(confDir);
	}
	//ioInstance logHandler(CONFIG_DIR + "log.txt");

	// network initialize
	curl_global_init(CURL_GLOBAL_DEFAULT);

	// verify arguments
	if (argc != 4) {
		std::cerr << "Need 3 arguments. Got " << argc - 1 << "." << std::endl;
		return 1;
	}
	// check update
}
