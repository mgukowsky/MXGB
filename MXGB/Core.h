#pragma once

#include "defs.h"

#include <iostream>
#include <string>

namespace MXGB {

class Core {
public:
	static Core& get_app_core();

	const bool create_window();
	void destroy_window();

	//Logging
	template<typename T>
	void logmsg(const T &msg) { std::cout << msg << std::endl; }

	template<typename T>
	void logerr(const T &msg) { std::cerr << msg << std::endl; }

	//Pop up a dialog and log a message
	void alert_msg(const char * const msg);
	void alert_err(const char * const msg);

	//Sleep for the desired number of milliseconds
	void sleep(const u32 ms);

	//Process system events
	void pump_events();

	//Pop up the file selection dialog
	const bool pick_file();

	//Holds options used throughout the application
	struct APPOPTIONS {
		bool shouldRun;
		std::string activeFile;
	} appOptions;

private:
	Core();
	~Core() = default;

	DISABLE_ALTERNATE_CTORS(Core);
};

} /* namespace MXGB */