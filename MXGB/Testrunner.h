#pragma once

#ifdef TESTBUILD

#include "Core.h"

namespace MXGB {

//Uses the Google test framework to implement TDD
class Testrunner {
public:
	Testrunner() = default;	
	~Testrunner() = default;

	DISABLE_ALTERNATE_CTORS(Testrunner);

	void run_all_tests();

	void run_basic_bus_tests();
	void run_basic_cpu_tests();
	void run_op_cpu_tests();

};

} /*namespace MXGB*/

#endif /*ifdef TESTBUILD*/