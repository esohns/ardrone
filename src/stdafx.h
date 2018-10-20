#ifndef __STDAFX__
#define __STDAFX__

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// *NOTE*: work around quirky MSVC...
#define NOMINMAX

//#include "targetver.h"

// Windows Header Files
#include "ace/config-lite.h"
#include <windows.h>

#include <strmif.h>
#include <reftime.h>
#if defined (DEBUG)
// *NOTE*: wxWidgets may have #defined __WXDEBUG__
#if defined (__WXDEBUG__)
#undef __WXDEBUG__
#endif // __WXDEBUG__
#include <wxdebug.h>
#endif // DEBUG
// *NOTE*: uuids.h does not have double include protection (?) (and is therefore
//         really a PITA to integrate consistently)
// *NOTE*: M******** obviously relies on precompilation features to get this
//         right; Note how this apparently precludes chances of meaningful
//         compiler standardization at this stage; YMMV
#define UUIDS_H
#include <uuids.h>

//#include <streams.h>
#endif // _MSC_VER

// C RunTime Header Files
//#include <iostream>
#include <string>

// System Library Header Files
#include "ace/config-lite.h"
#include "ace/Global_Macros.h"
#include "ace/Log_Msg.h"

//#ifdef LIBACENETWORK_ENABLE_VALGRIND_SUPPORT
#if defined (VALGRIND_SUPPORT)
#include "valgrind/valgrind.h"
#endif // VALGRIND_SUPPORT

// Library Header Files
#if defined (HAVE_CONFIG_H)
#include "libCommon_config.h"
#endif // HAVE_CONFIG_H
#include "common.h"
#include "common_macros.h"
#include "common_pragmas.h"

#if defined (HAVE_CONFIG_H)
#include "libACEStream_config.h"
#endif // HAVE_CONFIG_H
#include "stream_common.h"
#include "stream_macros.h"

#if defined (HAVE_CONFIG_H)
#include "libACENetwork_config.h"
#endif // HAVE_CONFIG_H
#include "net_common.h"
#include "net_macros.h"

//#if defined (_MSC_VER)
//#include "targetver.h"
//#endif

// *TODO*: reference additional headers your program requires here

// Local Header Files
#if defined (HAVE_CONFIG_H)
#include "ardrone_config.h"
#endif // HAVE_CONFIG_H
#include "ardrone_macros.h"

#endif // __STDAFX__
