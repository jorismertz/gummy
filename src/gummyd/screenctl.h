/**
* gummy
* Copyright (C) 2022  Francesco Fusco
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCREENCTL_H
#define SCREENCTL_H

#include "xorg.h"
#include "sysfs.h"
#include "../common/defs.h"

#include <thread>
#include <condition_variable>
#include <sdbus-c++/ProxyInterfaces.h>

class TempCtl
{
public:
    TempCtl(Xorg*);
	~TempCtl();
	int  getCurrentStep() const;
	void notify();
	void quit();
private:
	bool _quit;
	bool _force;
	int  _current_step;
	std::time_t _cur_time;
	std::time_t _sunrise_time;
	std::time_t _sunset_time;

	std::condition_variable _temp_cv;
	std::unique_ptr<std::thread> _thr;
	std::unique_ptr<sdbus::IProxy> _dbus_proxy;

	/**
	 * Temperature is adjusted in two steps.
	 * The first one is for quickly catching up to the proper temperature when:
	 * - time is checked sometime after the start time
	 * - the system wakes up
	 * - temperature settings change */
	void adjustTemp(Xorg*);
	void updateTimestamps();
	void checkWakeup();
};

class Monitor
{
public:
    Monitor(Xorg* server, Device *device, const int id);
	Monitor(Monitor&&);
	~Monitor();

	bool hasBacklight() const;
	void setBacklight(const int perc);
	void notify();
	void quit();
private:
	const int _id;
	bool _quit;
	bool _force;
	int  _current_step;
	int  _ss_brt;
	bool _brt_needs_change;

	Xorg *_server;
	Device *_device;
	std::mutex _brt_mtx;
	std::condition_variable _ss_cv;
	std::unique_ptr<std::thread> _thr;

	void capture();
	void adjustBrightness(std::condition_variable&);
};

class ScreenCtl
{
public:
    ScreenCtl(Xorg *server);
	~ScreenCtl();
	void applyOptions(const std::string&);
private:
	bool _quit;
	Xorg *_server;
	TempCtl _temp_ctl;
	std::vector<Device>  _devices;
	std::vector<Monitor> _monitors;

	std::condition_variable _gamma_refresh_cv;
	std::unique_ptr<std::thread> _gamma_refresh_thr;
	void reapplyGamma();
};

struct Options
{
    Options(const std::string &in);
	int scr_no               = -1;
	int brt_perc             = -1;
	int brt_auto             = -1;
	int brt_auto_min         = -1;
	int brt_auto_max         = -1;
	int brt_auto_offset      = -1;
	int brt_auto_speed       = -1;
	int screenshot_rate_ms   = -1;
	int temp_auto            = -1;
	int temp_k               = -1;
	int temp_day_k           = -1;
	int temp_night_k         = -1;
	int temp_adaptation_time = -1;
	std::string sunrise_time;
	std::string sunset_time;
};

#endif // SCREENCTL_H
