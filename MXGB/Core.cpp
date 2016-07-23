#include "Core.h"

using namespace MXGB;

namespace {
	const char * const appTitle = "MXGB: GB emulator by Matt Gukowsky";
	Core &appCore = Core::get_app_core();
}

//Inserts inline platform-specific code
#ifdef _MSC_VER
#include "Win/Core.impl.h"
#else
#error Core functionality not defined for this platform
#endif

Core::Core() {
	appOptions.shouldRun = true;
	appOptions.activeFile = "";
}

Core& Core::get_app_core() {
	static Core appCore;
	return appCore;
}

const bool Core::create_window() {
	return impl_create_window();
}

void Core::destroy_window() {
	impl_destroy_window();
}

void Core::alert_msg(const char * const msg) {
	impl_msgbox(msg);
	logmsg(msg);
}

void Core::alert_err(const char * const msg) {
	impl_errbox(msg);
	logerr(msg);
}

void Core::sleep(const u32 ms) {
	impl_sleep(ms);
}

void Core::pump_events() {
	impl_pump_events();
}

const bool Core::pick_file() {
	return impl_pick_file();
}