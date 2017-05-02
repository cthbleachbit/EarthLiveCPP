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
#include <chrono>
#include <thread>

#include <Magick++.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <curl/curl.h>

#include "lib/io.hpp"

void assembleUrl(std::string& target, int x, int y, int dimens, const std::string& datetime) {
	const std::string SITE = "http://himawari8-dl.nict.go.jp/himawari8/img/D531106/";
	if (dimens == 0) {
		target = SITE+ "latest.json";
	} else {
		std::string dimensSuffix = std::to_string(dimens) + "d/550/";
		std::string dateSuffix = datetime.substr(0,4) + "/" + datetime.substr(5,2) + "/" + datetime.substr(8,2) + "/";
		std::string timeSuffix = datetime.substr(11,2) + datetime.substr(14,2) + datetime.substr(17) + "_";
		std::string tileSuffix = std::to_string(x) + "_" + std::to_string(y) + ".png";
		// structure yyyy/mm/dd/hhmmss
		std::string finalUrl = SITE + dimensSuffix + dateSuffix + timeSuffix + tileSuffix;
		target = finalUrl;
	}
}

static size_t write_data(void *contents, size_t size, size_t nmemb, void *stream)
{
	((std::string*)stream)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// regular help usage
void printHelp() {
	std::cerr << "Usage: elcppd <interval> <density> <debug>" << std::endl;
	std::cerr << "\tinterval:    refresh interval in minutes." << std::endl;
	std::cerr << "\tdensity:     numerical value for image tiles." << std::endl;
	std::cerr << "\tdebug:       1 for debugging logs in log file" << std::endl;
}

// The function that actually downloads, returns a CURLcode. Not 0 = fail
int downloadRoutine(std::string& url, std::string& blob) {
	CURL* curl;
	CURLcode retCode;
	const char* c_url = url.c_str();
	curl = curl_easy_init();
	if (curl) {// if successfully created curl struct
		curl_easy_setopt(curl, CURLOPT_URL, c_url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, blob);
		retCode = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return retCode;
}

void imageUpdateRoutine(std::string& timestamp, int density, std::ofstream& os) {
	std::string tileURL;
	std::string result;
	std::vector<std::vector<Magick::Blob> > imageArray(density, std::vector<Magick::Blob>(density));
	for (int i = 0; i < density; i++) {
		for (int j = 0; j < density; j++) {
			tileURL = "";
			result = "";
			assembleUrl(tileURL, i, j, density, timestamp);
			downloadRoutine(tileURL, result);
			std::cout << i << " " << j << std::endl;
			size_t len = result.size();
			Magick::Blob imageBlob(static_cast<const void *>(result.c_str()), len);
			imageArray[i][j] = imageBlob;
		}
	}
}

int main(int argc, char *argv[]) {
	// setting up logging and storage space
	const std::string HOME_DIR = getenv("HOME");
	const std::string CONFIG_DIR = HOME_DIR + "/.earthlivecpp/";
	const std::string CONFIG_IMAGE = HOME_DIR + "/.earthlivecpp/final.png";
	const boost::filesystem::path confDir(CONFIG_DIR);
	if (!boost::filesystem::exists(confDir)) {
		boost::filesystem::create_directory(confDir);
	}
	// ioInstance logHandler(CONFIG_DIR + "log.txt");

	// initialize
	curl_global_init(CURL_GLOBAL_DEFAULT);
	InitializeMagick(*argv);
	int optionRefresh;
	int optionDensity;
	std::string lastUpdate = "0";
	std::string jsonURL;
	std::string jsonContent;

	// verify arguments
	if (argc != 4) {
		std::cerr << "Need 3 arguments. Got " << argc - 1 << "." << std::endl;
		printHelp();
		return 1;
	} else {
		optionRefresh = std::atoi(argv[1]);
		optionDensity = std::atoi(argv[2]);
	}	

	// start timing routine
	while (true) {
		// check update
		jsonURL = "";
		jsonContent = "";
		assembleUrl(jsonURL, 0, 0, 0, jsonURL); // get jsonURL
		downloadRoutine(jsonURL,jsonContent);
		jsonContent = jsonContent.substr(9,19);
		// got timestamp updated
		std::cout << jsonContent << std::endl;
		if (lastUpdate == "" || jsonContent > lastUpdate) {
			lastUpdate = jsonContent;
			std::ofstream image(CONFIG_IMAGE);
			imageUpdateRoutine(jsonContent, optionDensity, image);
		}
		std::this_thread::sleep_for(std::chrono::seconds(optionRefresh));
	}
}
