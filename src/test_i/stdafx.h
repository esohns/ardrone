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
#include <string>
//#include <map>
//#include <vector>
//#include <set>
//#include <bitset>
//#include <iostream>
//#include <sstream>
//#include <functional>

// System Library Header Files
#include "ace/config-lite.h"
#include "ace/Assert.h"
#include "ace/Global_Macros.h"
#include "ace/Log_Msg.h"
//#include "ace/Synch_Traits.h"

//#include "gtk/gtk.h"

#include "common.h"
#include "common_macros.h"

#include "stream_common.h"
#include "stream_macros.h"

#include "net_common.h"
#include "net_macros.h"

//#include "test_i_common.h"

//#include "ardrone_common.h"
//#include "ardrone_types.h"
#include "ardrone_macros.h"

#if defined (ARDRONE_ENABLE_VALGRIND_SUPPORT)
#include "valgrind/memcheck.h"
#endif

#define LIGBGLADE_SUPPORT
