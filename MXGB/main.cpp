#include "main.h"

#include <iostream>
#include <string>

using namespace MXGB;

namespace {
	Core &appCore = Core::get_app_core();
}

const int MXGB::common_main() {
	if (!appCore.create_window())
		return 1;

	while (appCore.appOptions.shouldRun) {
		appCore.pump_events();
		appCore.sleep(100);
	}

	appCore.destroy_window();

	return 0;
}

//Entry points
#ifdef TESTBUILD
#ifdef _WIN64
#pragma message("Warning: please supply a 64 bit version of gtest.lib if building x64 test build")
#endif

#include "Testrunner.h"
#include <gtest\gtest.h>

int main(int argc, char **argv) {
	//gtest magic to parse cmdline switches
	::testing::InitGoogleTest(&argc, argv);
	Testrunner tr;
	tr.run_all_tests();

	//Block until user presses ENTER
	appCore.logmsg("Press ENTER to exit");
	std::string s;
	std::getline(std::cin, s);

	return 0;
}

#else

#ifdef _MSC_VER

#include <Windows.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	return MXGB::common_main();
}

#else

int main(int argc, char **argv) {
	return MXGB::common_main();
}

#endif /* ifdef _MSC_VER */
#endif /* ifdef TESTBUILD */