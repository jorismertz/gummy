﻿#include "sysfs.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include "../commons/defs.h"

Sysfs::Sysfs()
{
	namespace fs = std::filesystem;

	for (const auto &entry : fs::directory_iterator("/sys/class/backlight")) {

		const auto path         = entry.path();
		const auto path_str     = path.generic_string();
		const auto max_brt_file = path_str + "/max_brightness";

		std::ifstream stream(max_brt_file);
		if (!stream.is_open()) {
			LOGF << "Unable to open " << max_brt_file;
			exit(1);
		}

		std::string max_brt;
		stream.read(max_brt.data(), sizeof(max_brt));

		m_devices.emplace_back(
		    path.filename(),
		    path,
		    path_str + "/brightness",
		    std::stoi(max_brt)
		);
	}
}

Device::Device(std::string name, std::string path, std::string brt_file, int max_brt)
    : name(name),
      path(path),
      brt_file(brt_file),
      max_brt(max_brt)
{

}

Device::Device(Device&&) { }

void Device::setBrightness(int brt)
{
    brt = std::clamp(brt, 0, max_brt);
    std::string out(std::to_string(brt) + '\n');

    std::ofstream stream(brt_file);
    stream.write(out.c_str(), out.size());
}