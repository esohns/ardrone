// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define _USE_MATH_DEFINES

// Windows Header Files
#include <windows.h>
#endif

//#define min
//#define max

// C RunTime Header Files
//#include <string>
//#include <map>
//#include <vector>
//#include <set>
//#include <bitset>
//#include <iostream>
//#include <sstream>
//#include <functional>

// System Library Header Files
#include <ace/OS.h>
//#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Assert.h>
//#include <ace/Synch.h>
//#include <ace/Task.h>
//#include <ace/Reactor.h>
//#include <ace/Malloc_Allocator.h>
//#include <ace/Lock_Adapter_T.h>
//#include <ace/Stream.h>

#if defined (ARDRONE_ENABLE_VALGRIND_SUPPORT)
#include <valgrind/memcheck.h>
#endif

#define LIGBGLADE_SUPPORT
