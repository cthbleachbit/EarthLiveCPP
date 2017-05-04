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

//#include <Magick++.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <curl/curl.h>

// quic way to assemble a url for downloading
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

// write function for curl to fill in data
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
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, blob);
		retCode = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return retCode;
}

void imageUpdateRoutine(std::string& timestamp, int density, const std::string& path) {
	std::string tileURL;
	/* the looser decides not to compile in imagemagick
	std::vector<std::string> bufferArray;
	std::vector<Magick::Blob> blobArray;
	std::vector<Magick::Image> tileArray;
	std::vector<Magick::Image> thumb;
	Magick::Blob finalBlob;
	*/

	// EXTERNAL CALL: prepare download
	const std::string TILES_DIR = path + "tiles/";
	const boost::filesystem::path tilesDir(TILES_DIR);
	if (!boost::filesystem::exists(tilesDir)) {
		boost::filesystem::create_directory(tilesDir);
	} else if (!boost::filesystem::is_directory(tilesDir)) {
		boost::filesystem::remove_all(tilesDir);
		boost::filesystem::create_directory(tilesDir);
	}

	// download all tiles
	for (int j = 0; j < density; j++) {
		for (int i = 0; i < density; i++) {
			tileURL = "";
			std::string buffer = "";
			assembleUrl(tileURL, i, j, density, timestamp);
			if (downloadRoutine(tileURL, buffer) != CURLE_OK) {
				std::cerr << "Image downloading failed." << std::endl;
				return;
			};
			/*
			size_t len = buffer.size();
			bufferArray.push_back(buffer);
			Magick::Blob tileBlob(static_cast<const void *>(bufferArray[j * density + i].c_str()), len);
			blobArray.push_back(tileBlob);
			std::cout << i << " " << j <<std::endl;
			Magick::Image tile(blobArray[j * density + i]);
			tileArray.push_back(tile);
			tileArray[j * density + i].magick("PNG");
			*/

			// EXTERNAL CALL: save in actual files
			std::ofstream tileOutput(path + "tiles/tile_" + std::to_string(j) + "_" + std::to_string(i) + ".png");
			if (tileOutput.is_open()){
				tileOutput << buffer;
				tileOutput.close();
			} else {
				std::cerr << "Fail to write tile image." << std::endl;
			}
		}
	}

	// EXTERNAL CALL to magick processing (dirty)
	const std::string cmdBase = "montage";
	const std::string cmdInput = " \"" + path + "tiles/tile_*_*.png\"";
	const std::string cmdTiling = " -tile " + std::to_string(density) + "x" + std::to_string(density);
	const std::string cmdGeometry = " -geometry +0+0";
	const std::string cmdTarget = " " + path + "final.png";
	FILE *f = popen((cmdBase + cmdInput + cmdTiling + cmdGeometry + cmdTarget).c_str(), "r");
	if ( f == 0 ) {
        fprintf( stderr, "Could not execute\n" );
    } else {
		pclose(f);
	};
	// clean up remaining tiles
	boost::filesystem::remove_all(tilesDir);

	// TO-DO: get imagemagick running inside
	/* imagemagick montage processing
	Magick::Montage tiling;
	const Magick::Geometry tilingArrangment(std::to_string(density) + "x" + std::to_string(density));
	const Magick::Geometry tilingGeometry("550x550");
	tiling.tile(tilingArrangment);
	tiling.geometry(tilingGeometry);
	std::cerr << "pros" << std::endl;
	Magick::montageImages(&thumb, tileArray.begin(), tileArray.end(), tiling);
	thumb[1].magick("PNG");
	thumb[1].write(&finalBlob);
	os << ((char*)finalBlob.data(), finalBlob.length());
	*/
}

int main(int argc, char *argv[]) {
	// setting up logging and storage space
	const std::string HOME_DIR = getenv("HOME");
	const std::string CONFIG_DIR = HOME_DIR + "/.earthlivecpp/";
	const std::string TILES_DIR = HOME_DIR + "/.earthlivecpp/tiles";
	const boost::filesystem::path confDir(CONFIG_DIR);
	const boost::filesystem::path tilesDir(TILES_DIR);
	if (!boost::filesystem::exists(confDir)) {
		boost::filesystem::create_directory(confDir);
	}
	// store pid of itself
	std::ofstream pidFile(CONFIG_DIR + "pid");
	pidFile << ::getpid() << std::endl;
	pidFile.close();
	// ioInstance logHandler(CONFIG_DIR + "log.txt");

	// initialize
	curl_global_init(CURL_GLOBAL_DEFAULT);
	//Magick::InitializeMagick(*argv);
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
		if (downloadRoutine(jsonURL, jsonContent) != CURLE_OK) {
			std::cerr << "json refresh failed" << std::endl;
			continue;
		};
		jsonContent = jsonContent.substr(9,19);
		std::cout << jsonContent << std::endl;
		// got timestamp updated
		if (lastUpdate == "" || jsonContent > lastUpdate) {
			lastUpdate = jsonContent;
			imageUpdateRoutine(jsonContent, optionDensity, CONFIG_DIR);
		}
		std::this_thread::sleep_for(std::chrono::seconds(optionRefresh));
	}
}
