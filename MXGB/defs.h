#pragma once

//Note that MSVC Release builds define _HAS_EXCEPTION=0 on the command line; this fully turns off exception handling in the STL implementation

#ifdef _MSC_VER
#define FORCEINLINE		__forceinline
#else
#define FORCEINLINE		inline
#endif

//If we need to test, this macro grants the testrunner complete access to all other classes
#ifdef TESTBUILD
#define DECLARE_TESTRUNNER_ACCESS		friend class Testrunner
#else
#define DECLARE_TESTRUNNER_ACCESS
#endif

//Turn off copy, assignment, move, and move assignment constructors
#define DISABLE_ALTERNATE_CTORS(CLASS_T)\
	CLASS_T(CLASS_T&) = delete;\
	CLASS_T& operator=(CLASS_T&) = delete;\
	CLASS_T(CLASS_T&&) = delete;\
	CLASS_T& operator=(CLASS_T&&) = delete

#include <cstdint>

namespace MXGB {
	typedef int8_t		s8;
	typedef int16_t		s16;
	typedef int32_t		s32;
	typedef int64_t		s64;

	typedef uint8_t		u8;
	typedef uint16_t	u16;
	typedef uint32_t	u32;
	typedef uint64_t	u64;

	namespace DEFAULTS {
		const u16 WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
	}
}