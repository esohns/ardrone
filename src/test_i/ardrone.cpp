/***************************************************************************
*   Copyright (C) 2010 by Erik Sohns   *
*   erik.sohns@web.de   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "stdafx.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>

#if defined (ENABLE_NLS)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gettext.h"
#else
#include "locale.h"
#include "libintl.h"

#include "tinygettext.h"
#endif // ACE_WIN32 || ACE_WIN64
#endif // ENABLE_NLS

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <dshow.h>
#include <initguid.h> // *NOTE*: this exports DEFINE_GUIDs (see stream_misc_common.h)
#include <mfapi.h>
#endif

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixfmt.h"
}
#endif /* __cplusplus */

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_main.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "ace/POSIX_Proactor.h"
#endif
#include "ace/Profile_Timer.h"
#include "ace/Synch.h"
#include "ace/Time_Value.h"

#ifdef HAVE_CONFIG_H
#include "ardrone_config.h"
#endif

#include "common_defines.h"
#include "common_logger.h"
#include "common_file_tools.h"
#include "common_signal_tools.h"
#include "common_tools.h"

#include "common_timer_tools.h"

#include "common_ui_defines.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"

#include "stream_allocatorheap.h"

#include "stream_dec_tools.h"

#include "stream_dev_tools.h"

#include "stream_lib_common.h"
#include "stream_lib_defines.h"

#include "net_defines.h"

#include "net_client_connector.h"
#include "net_client_asynchconnector.h"

#include "ardrone_callbacks.h"
#include "ardrone_configuration.h"
#include "ardrone_defines.h"
#include "ardrone_eventhandler.h"
#include "ardrone_macros.h"
#include "ardrone_module_eventhandler.h"
#include "ardrone_network.h"
#include "ardrone_signalhandler.h"
#include "ardrone_stream.h"
#include "ardrone_types.h"

//const char net_video_stream_name_string_[] =
//  ACE_TEXT_ALWAYS_CHAR ("NetVideoStream");
//const char net_control_stream_name_string_[] =
//  ACE_TEXT_ALWAYS_CHAR ("NetControlStream");
//const char net_navdata_stream_name_string_[] =
//  ACE_TEXT_ALWAYS_CHAR ("NetNavDataStream");
//const char net_mavlink_stream_name_string_[] =
//  ACE_TEXT_ALWAYS_CHAR ("NetMAVLinkStream");

//----------------------------------------

//void
//do_atExit (void)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::do_atExit"));

//#ifdef HAVE_CONFIG_H
//  ACE_DEBUG ((LM_ERROR,
//              ACE_TEXT ("program %s aborted\n"),
//              ACE_TEXT (ARDRONE_PACKAGE)));
//#else
//  ACE_DEBUG ((LM_ERROR,
//              ACE_TEXT ("program aborted\n")));
//#endif
//}

//----------------------------------------

void
do_printVersion (const std::string& programName_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::do_printVersion"));

  // step1: program version
  //   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in
            << ACE_TEXT_ALWAYS_CHAR (": ")
#ifdef HAVE_CONFIG_H
            << ARDRONE_VERSION_MAJOR
            << ACE_TEXT_ALWAYS_CHAR (".")
            << ARDRONE_VERSION_MINOR
            << ACE_TEXT_ALWAYS_CHAR (".")
            << ARDRONE_VERSION_MICRO
#else
            << ACE_TEXT_ALWAYS_CHAR ("N/A")
#endif
            << std::endl;

  std::ostringstream version_number;
  // step2: ACE version
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number (this is needed, as the library soname is compared to this
  // string)
//  version_number.str ("");
  version_number << ACE::major_version ();
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << ACE::minor_version ();
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << ACE::beta_version ();
  std::cout << ACE_TEXT ("ACE: ")
    //             << ACE_VERSION
            << version_number.str ()
            << std::endl;
}

void
do_printUsage (const std::string& programName_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

//   enum Common_PlatformOSType os_type_e;
// #if defined (ACE_LINUX)
//   Common_Tools::isLinux (os_type_e);
// #endif

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-a [IPv4]   : drone IP address (dotted decimal) [")
            << ACE_TEXT_ALWAYS_CHAR (ARDRONE_DEFAULT_IP_ADDRESS)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-b          : buffer size (bytes) [")
            << ARDRONE_MESSAGE_BUFFER_SIZE
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c          : show console [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
#endif // ACE_WIN32 || ACE_WIN64
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d          : debug parser(s) [")
            << COMMON_PARSER_DEFAULT_LEX_TRACE
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f          : display video full-screen (: windowed) [")
            << ARDRONE_DEFAULT_VIDEO_FULLSCREEN
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-i [GUID]   : network interface [\"")
            << ACE_TEXT_ALWAYS_CHAR (Net_Common_Tools::interfaceToString (Net_Common_Tools::getDefaultInterface (NET_LINKLAYER_802_11)).c_str ())
#else
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-i [STRING] : network interface [\"")
            << ACE_TEXT_ALWAYS_CHAR (Net_Common_Tools::getDefaultInterface (NET_LINKLAYER_802_11).c_str ())
#endif // ACE_WIN32 || ACE_WIN64
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : use media foundation framework (: directshow) [")
            << (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK == STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#endif // ACE_WIN32 || ACE_WIN64
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p [UDP]    : drone video port [")
            << ARDRONE_PORT_TCP_VIDEO
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r          : use reactor [")
            << NET_EVENT_USE_REACTOR
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s          : drone WLAN SSID")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  std::string UI_file = path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_DEFINITION_FILE_NAME);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-u[[STRING]]: interface definition file [\"")
            << UI_file
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v          : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-z          : debug nl80211 [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be 'const'
                     ACE_INET_Addr& address_out,
                     unsigned int& bufferSize_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                     bool& showConsole_out,
#endif // ACE_WIN32 || ACE_WIN64
                     bool& debugScanner_out,
                     bool& fullScreen_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                     struct _GUID& interfaceIdentifier_out,
#else
                     std::string& interfaceIdentifier_out,
#endif // ACE_WIN32 || ACE_WIN64
                     bool& logToFile_out,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                     bool& useMediaFoundation_out,
#endif // ACE_WIN32 || ACE_WIN64
                     unsigned short& portNumber_out,
                     bool& useReactor_out,
                     std::string& SSID_out,
                     bool& traceInformation_out,
                     std::string& interfaceDefinitionFile_out,
                     bool& printVersionAndExit_out
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
                     ,bool& debugNl80211_out
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
                     )
{
  ARDRONE_TRACE (ACE_TEXT ("::do_processArguments"));


  int result = -1;
  std::string configuration_path = Common_File_Tools::getWorkingDirectory ();

  // initialize results
  std::string address_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_DEFAULT_IP_ADDRESS);
  if (address_string.find (':') != std::string::npos)
    result = address_out.set (address_string.c_str (), 0);
  else
    result = address_out.set (ARDRONE_PORT_TCP_VIDEO,
                              address_string.c_str (),
                              1,
                              0);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (address_string.c_str ())));
    return false;
  } // end IF
  bufferSize_out              = ARDRONE_MESSAGE_BUFFER_SIZE;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  showConsole_out             = false;
#endif
  debugScanner_out            = COMMON_PARSER_DEFAULT_LEX_TRACE;
  fullScreen_out              = ARDRONE_DEFAULT_VIDEO_FULLSCREEN;
  interfaceIdentifier_out     =
    Net_Common_Tools::getDefaultInterface (NET_LINKLAYER_802_11);
  logToFile_out               = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  useMediaFoundation_out      =
    (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK == STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION);
#endif
  portNumber_out              = ARDRONE_PORT_TCP_VIDEO;
  useReactor_out              = NET_EVENT_USE_REACTOR;
  SSID_out                    =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_DEFAULT_WLAN_SSID);
  traceInformation_out        = false;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  interfaceDefinitionFile_out = path;
  interfaceDefinitionFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  interfaceDefinitionFile_out +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_DEFINITION_FILE_NAME);
  printVersionAndExit_out     = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
  debugNl80211_out = false;
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

  ACE_Get_Opt argument_parser (argc_in,
                               argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                               ACE_TEXT ("a:b:cdfi:lmp:rs:tu::v"),
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
                               ACE_TEXT ("a:b:dfi:lp:rs:tu::vz"),
#else
                               ACE_TEXT ("a:b:dfi:lp:rs:tu::v"),
#endif // _DEBUG
#else
                               ACE_TEXT ("a:b:dfi:lp:rs:tu::v"),
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
                               1,                          // skip command name
                               1,                          // report parsing errors
                               ACE_Get_Opt::PERMUTE_ARGS,  // ordering
                               0);                         // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argument_parser ()) != EOF)
  {
    switch (option)
    {
      case 'a':
      {
        address_string = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        if (address_string.find (':') != std::string::npos)
          result = address_out.set (address_string.c_str (), 0);
        else
          result = address_out.set (ARDRONE_PORT_TCP_VIDEO,
                                    address_string.c_str (),
                                    1,
                                    0);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                      ACE_TEXT (argument_parser.opt_arg ())));
          return false;
        } // end IF
        break;
      }
      case 'b':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        converter >> bufferSize_out;
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case 'c':
      {
        showConsole_out = true;
        break;
      }
#endif
      case 'd':
      {
        debugScanner_out = true;
        break;
      }
      case 'f':
      {
        fullScreen_out = true;
        break;
      }
      case 'i':
      {
        interfaceIdentifier_out =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          Common_Tools::StringToGUID (ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ()));
#else
          ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
#endif
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case 'm':
      {
        useMediaFoundation_out = true;
        break;
      }
#endif
      case 'p':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        converter >> portNumber_out;
        break;
      }
      case 'r':
      {
        useReactor_out = true;
        break;
      }
      case 's':
      {
        SSID_out = ACE_TEXT_ALWAYS_CHAR (argument_parser.opt_arg ());
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'u':
      {
        ACE_TCHAR* opt_arg = argument_parser.opt_arg ();
        if (opt_arg)
          interfaceDefinitionFile_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          interfaceDefinitionFile_out.clear ();
        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;
        break;
      }
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
      case 'z':
      {
        debugNl80211_out = true;
        break;
      }
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argument_parser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argument_parser.long_option ())));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_initializeSignals (bool useReactor_in,
                      bool allowUserRuntimeStats_in,
                      ACE_Sig_Set& signals_out,
                      ACE_Sig_Set& ignoredSignals_out)
{
  ARDRONE_TRACE (ACE_TEXT ("::do_initializeSignals"));

  int result = -1;

  // initialize return value(s)
  result = signals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF
  result = ignoredSignals_out.empty_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", returning\n")));
    return;
  } // end IF

  // *PORTABILITY*: on Microsoft Windows (TM) most signals are not defined,
  //                and ACE_Sig_Set::fill_set() doesn't really work as specified
  //                --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
  //  signals_out.sig_add(SIGSEGV);          // 11      /* segment violation */
  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
  if (allowUserRuntimeStats_in)
  {
    signals_out.sig_add (SIGBREAK);        // 21      /* Ctrl-Break sequence */
    ignoredSignals_out.sig_add (SIGBREAK); // 21      /* Ctrl-Break sequence */
  } // end IF
  signals_out.sig_add (SIGABRT);           // 22      /* abnormal termination triggered by abort call */
  signals_out.sig_add (SIGABRT_COMPAT);    // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
  result = signals_out.fill_set ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", returning\n")));
    return;
  } // end IF
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeStats_in)
  {
    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
} // end IF
  // *NOTE* core dump on SIGSEGV
  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */

  // *IMPORTANT NOTE*: "...NPTL makes internal use of the first two real-time
  //                   signals (see also signal(7)); these signals cannot be
  //                   used in applications. ..." (see 'man 7 pthreads')
  // --> on POSIX platforms, make sure that ACE_SIGRTMIN == 34
  //  for (int i = ACE_SIGRTMIN;
  //       i <= ACE_SIGRTMAX;
  //       i++)
  //    signals_out.sig_del (i);

  if (!useReactor_in)
  {
    ACE_Proactor* proactor_p = ACE_Proactor::instance ();
    ACE_ASSERT (proactor_p);
    ACE_POSIX_Proactor* proactor_impl_p =
      dynamic_cast<ACE_POSIX_Proactor*> (proactor_p->implementation ());
    ACE_ASSERT (proactor_impl_p);
    if (proactor_impl_p->get_impl_type () == ACE_POSIX_Proactor::PROACTOR_SIG)
      signals_out.sig_del (COMMON_EVENT_PROACTOR_SIG_RT_SIGNAL);
  } // end IF
#endif

  // *NOTE*: gdb sends some signals (when running in an IDE ?)
  //         --> remove signals (and let IDE handle them)
  // *TODO*: clean this up
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (DEBUG_DEBUGGER)
  //  signals_out.sig_del (SIGINT);
  signals_out.sig_del (SIGCONT);
  signals_out.sig_del (SIGHUP);
#endif
#endif

  // *TODO*: improve valgrind support
#ifdef ARDRONE_ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_out.sig_del (SIGRTMAX);     // 64
#endif
}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
bool
do_initialize_directshow (IGraphBuilder*& IGraphBuilder_out,
                          struct _AMMediaType*& mediaType_out,
                          bool coInitialize_in,
                          bool fullScreen_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::do_initialize_directshow"));

  HRESULT result = E_FAIL;
  std::list<std::wstring> filter_pipeline;
  BOOL result_2 = FALSE;

  // sanity check(s)
  ACE_ASSERT (!IGraphBuilder_out);
  if (mediaType_out)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (mediaType_out);

  if (!coInitialize_in)
    goto continue_;

  result = CoInitializeEx (NULL,
                           (COINIT_MULTITHREADED    |
                            COINIT_DISABLE_OLE1DDE  |
                            COINIT_SPEED_OVER_MEMORY));
  if (FAILED (result))
  { // *NOTE*: most probable reason: RPC_E_CHANGED_MODE
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to CoInitializeEx(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
  } // end IF

continue_:
#if defined (_DEBUG)
  DWORD dwFlags = GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
  HMODULE module_h = NULL;
  result_2 = GetModuleHandleEx (dwFlags,
                                NULL,
                                &module_h);
  if (!result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetModuleHandleEx(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (::GetLastError ()).c_str ())));
    return false;
  } // end IF

  DbgInitialise (module_h);

  DWORD debug_log_type = (LOG_ERROR   |
                          LOG_LOCKING |
                          LOG_MEMORY  |
                          LOG_TIMING  |
                          LOG_TRACE   |
                          LOG_CUSTOM1 |
                          LOG_CUSTOM2 |
                          LOG_CUSTOM3 |
                          LOG_CUSTOM4 |
                          LOG_CUSTOM5);
  // *NOTE*: message levels < of the current setting will be displayed
  //         --> 0: display all messages
  // *TODO*: find other debug levels in the DirectShow base-class code
  DWORD debug_log_level = 0;
  //DWORD debug_log_level = CONNECT_TRACE_LEVEL;
  DbgSetModuleLevel (debug_log_type,
                     debug_log_level);
#endif

  // sanity check(s)
  ACE_ASSERT (!mediaType_out);

  mediaType_out =
    static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
  if (!mediaType_out)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  ACE_OS::memset (mediaType_out, 0, sizeof (struct _AMMediaType));

  ACE_ASSERT (!mediaType_out->pbFormat);
  mediaType_out->pbFormat =
    static_cast<BYTE*> (CoTaskMemAlloc (sizeof (struct tagVIDEOINFOHEADER)));
      //static_cast<BYTE*> (CoTaskMemAlloc (sizeof (struct tagVIDEOINFOHEADER2)));
  if (!mediaType_out->pbFormat)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to CoTaskMemAlloc(%u): \"%m\", aborting\n"),
                sizeof (struct tagVIDEOINFOHEADER)));
                //sizeof (struct tagVIDEOINFOHEADER2)));
    goto error;
  } // end IF
  ACE_OS::memset (mediaType_out->pbFormat,
                  0,
                  sizeof (struct tagVIDEOINFOHEADER));
                  //sizeof (struct tagVIDEOINFOHEADER2));

  // *NOTE*: this specifies the 'input' media format, i.e. the media format that
  //         is delivered to the DirectShow (TM) pipeline. The ARDrone Parrot
  //         (TM) quadcopter serves an encapsulated H264 ('PaVe') format, which
  //         is currently pre-processed (ffmpeg) and streamed as uncompressed
  //         RGB. Note that this implementation may be more efficient, as it
  //         uses less DirectShow capabilities and resources; this requires more
  //         investigation, however. The DirectShow processing pipeline includes
  //         the RGB Color Converter and a resizer DMO modules that transform
  //         and scale the frames to whatever format/size needed for convenient
  //         rendering
  // *TODO*: the current implementation does not leverage GPU hardware
  //         acceleration and does not support the Media Foundation (TM)
  //         framework, and thus probably requires more CPU power.
  //         Also, GNU/Linux support is incomplete. Given the diversity of
  //         computing platforms and graphics capabilities of host devices, this
  //         may not be an issue at this point and will require specification
  mediaType_out->majortype = MEDIATYPE_Video;
  //mediaType_out->subtype = MEDIASUBTYPE_H264;
  //mediaType_out->subtype = MEDIASUBTYPE_YV12;
  mediaType_out->subtype = MEDIASUBTYPE_RGB24;
  //mediaType_out->bFixedSizeSamples = FALSE;
  //mediaType_out->bTemporalCompression = TRUE;
  mediaType_out->bFixedSizeSamples = TRUE;
  mediaType_out->bTemporalCompression = FALSE;
  // *NOTE*: lSampleSize is set after pbFormat (see below)
  //mediaType_out->lSampleSize = video_info_header_p->bmiHeader.biSizeImage;
  mediaType_out->formattype = FORMAT_VideoInfo;
  //mediaType_out->formattype = FORMAT_VideoInfo2;
  mediaType_out->cbFormat = sizeof (struct tagVIDEOINFOHEADER);
  //mediaType_out->cbFormat = sizeof (struct tagVIDEOINFOHEADER2);

  struct tagVIDEOINFOHEADER* video_info_header_p =
    reinterpret_cast<struct tagVIDEOINFOHEADER*> (mediaType_out->pbFormat);
  //struct tagVIDEOINFOHEADER2* video_info_header_p =
  //  reinterpret_cast<struct tagVIDEOINFOHEADER2*> (mediaType_out->pbFormat);
  ACE_ASSERT (video_info_header_p);

  //video_info_header_p->rcSource.right = ARDRONE_DEFAULT_VIDEO_WIDTH;
  //video_info_header_p->rcSource.bottom = ARDRONE_DEFAULT_VIDEO_HEIGHT;
  //video_info_header_p->rcTarget.right = ARDRONE_DEFAULT_VIDEO_WIDTH;
  //video_info_header_p->rcTarget.bottom = ARDRONE_DEFAULT_VIDEO_HEIGHT;

  // *NOTE*: width * height * bytes/pixel * frames/s * 8
  ARDroneVideoModeToResolution (ARDRONE_DEFAULT_VIDEO_MODE,
                                *reinterpret_cast<unsigned int*> (&video_info_header_p->bmiHeader.biWidth),
                                *reinterpret_cast<unsigned int*> (&video_info_header_p->bmiHeader.biHeight));
  video_info_header_p->dwBitRate =
    (video_info_header_p->bmiHeader.biWidth * video_info_header_p->bmiHeader.biHeight) * 4 * 30 * 8;
  //video_info_header_p->dwBitErrorRate = 0;
  video_info_header_p->AvgTimePerFrame =
    MILLISECONDS_TO_100NS_UNITS (1000 / 30); // --> 30 fps

  //video_info_header_p->dwInterlaceFlags = 0; // --> progressive
  //video_info_header_p->dwCopyProtectFlags = 0; // --> not protected

  //video_info_header_p->dwPictAspectRatioX = 0; // --> *TODO*
  //video_info_header_p->dwPictAspectRatioY = 0; // --> *TODO*

  //video_info_header_p->dwControlFlags = 0;
  //video_info_header_p->dwReserved2 = 0;

  // *TODO*: make this configurable (and part of a protocol)
  video_info_header_p->bmiHeader.biSize = sizeof (struct tagBITMAPINFOHEADER);
  video_info_header_p->bmiHeader.biHeight =
    -video_info_header_p->bmiHeader.biHeight;
  video_info_header_p->bmiHeader.biPlanes = 1;
  //video_info_header_p->bmiHeader.biBitCount = 12;
  video_info_header_p->bmiHeader.biBitCount = 24;
  video_info_header_p->bmiHeader.biCompression = BI_RGB;
  // *NOTE*: "...For compressed video and YUV formats, this member is a FOURCC
  //         code, specified as a DWORD in little-endian order. ..."
  //if (ACE_LITTLE_ENDIAN)
  //  video_info_header_p->bmiHeader.biCompression = MAKEFOURCC ('Y', 'V', '1', '2');
  //else
  //  video_info_header_p->bmiHeader.biCompression = FCC (ACE_SWAP_LONG ((DWORD)'YV12'));

  if (fullScreen_in)
  {
    GdkDisplay* display_p =
      gdk_display_manager_get_default_display (gdk_display_manager_get ());
    ACE_ASSERT (display_p);
#if GTK_CHECK_VERSION (3,0,0)
    GdkMonitor* monitor_p = gdk_display_get_primary_monitor (display_p);
    ACE_ASSERT (monitor_p);
    struct _cairo_rectangle_int rectangle_s;
    gdk_monitor_get_geometry (monitor_p,
                              &rectangle_s);
#else
    GdkScreen* screen_p =
      //gdk_display_get_screen (display_p,
      //                        0);
      gdk_display_get_default_screen (display_p);
    ACE_ASSERT (screen_p);
    gint monitor_i = gdk_screen_get_primary_monitor (screen_p);
    struct _GdkRectangle rectangle_s;
    gdk_screen_get_monitor_geometry (screen_p,
                                     monitor_i,
                                     &rectangle_s);
#endif
    video_info_header_p->bmiHeader.biHeight = -rectangle_s.height;
    video_info_header_p->bmiHeader.biWidth = rectangle_s.width;
  } // end IF
  video_info_header_p->bmiHeader.biSizeImage =
    DIBSIZE (video_info_header_p->bmiHeader);
  //video_info_header_p->bmiHeader.biXPelsPerMeter;
  //video_info_header_p->bmiHeader.biYPelsPerMeter;
  //video_info_header_p->bmiHeader.biClrUsed;
  //video_info_header_p->bmiHeader.biClrImportant;

  // *NOTE*: union
  //video_info_header_p->bmiColors = ;
  //video_info_header_p->dwBitMasks = {0, 0, 0};
  //video_info_header_p->TrueColorInfo = ;

  mediaType_out->lSampleSize = video_info_header_p->bmiHeader.biSizeImage;

  return true;

error:
  if (mediaType_out)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (mediaType_out);
  IGraphBuilder_out->Release ();
  IGraphBuilder_out = NULL;

  if (coInitialize_in)
    CoUninitialize ();

  return false;
}
void
do_finalize_directshow (IGraphBuilder*& IGraphBuilder_inout,
                        struct _AMMediaType*& mediaType_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("::do_finalize_directshow"));

  if (IGraphBuilder_inout)
  {
    IGraphBuilder_inout->Release ();
    IGraphBuilder_inout = NULL;
  } // end IF
  if (mediaType_inout)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (mediaType_inout);

#if defined (_DEBUG)
  DbgTerminate ();
#endif

  CoUninitialize ();
}

bool
do_initialize_mediafoundation (bool coInitialize_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::do_initialize_mediafoundation"));

  HRESULT result = E_FAIL;

  if (!coInitialize_in)
    goto continue_;

  result = CoInitializeEx (NULL,
                           (COINIT_MULTITHREADED    |
                            COINIT_DISABLE_OLE1DDE  |
                            COINIT_SPEED_OVER_MEMORY));
  if (FAILED (result))
  {
    // *NOTE*: most probable reason: already initialized (happens in the
    //         debugger)
    //         --> continue
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("failed to CoInitializeEx(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
  } // end IF

  result = MFStartup (MF_VERSION,
                      MFSTARTUP_LITE);
  if (FAILED (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFStartup(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    goto error;
  } // end IF

continue_:
  return true;

error:
  result = MFShutdown ();
  if (FAILED (result))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFShutdown(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

  if (coInitialize_in)
    CoUninitialize ();

  return false;
}
void
do_finalize_mediafoundation ()
{
  ARDRONE_TRACE (ACE_TEXT ("::do_finalize_mediafoundation"));

  HRESULT result = E_FAIL;

  result = MFShutdown ();
  if (FAILED (result))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFShutdown(): \"%s\", continuing\n"),
                ACE_TEXT (Common_Tools::errorToString (result).c_str ())));

  CoUninitialize ();
}
#endif

void
do_work (int argc_in,
         ACE_TCHAR** argv_in,
         const ACE_INET_Addr& address_in,
         unsigned int bufferSize_in,
         bool debugScanner_in,
         bool fullScreen_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         REFGUID interfaceIdentifier_in,
#else
         const std::string& interfaceIdentifier_in,
#endif
         bool useReactor_in,
         const std::string& SSID_in,
         const std::string& UIInterfaceDefinitionFile_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
         bool debugNl80211_in,
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
         struct ARDrone_GtkCBData& CBData_in,
         const ACE_Sig_Set& signalSet_in,
         const ACE_Sig_Set& ignoredSignalSet_in,
         Common_SignalActions_t& previousSignalActions_inout,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         bool showConsole_in,
#endif
         ARDrone_SignalHandler& signalHandler_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::do_work"));

  int result = -1;
  bool result_2 = false;
  //std::string interface_identifier_string;
  std::string stream_name_string;

  // sanity check(s)
  ACE_ASSERT (CBData_in.configuration);

  // step1: initialize configuration data
  ARDrone_EventHandler event_handler (&CBData_in,
                                      UIInterfaceDefinitionFile_in.empty ());
  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct ARDrone_AllocatorConfiguration> heap_allocator;
  if (!heap_allocator.initialize (CBData_in.configuration->allocatorConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize heap allocator, returning\n")));
    return;
  } // end IF
  ARDrone_MessageAllocator_t message_allocator (ARDRONE_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                &heap_allocator,
                                                true); // block

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_ConnectionManager_t* directshow_connection_manager_p =
    NULL;
  ARDrone_MediaFoundation_ConnectionManager_t* mediafoundation_connection_manager_p =
    NULL;
  switch (CBData_in.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_connection_manager_p =
        ARDRONE_DIRECTSHOW_CONNECTIONMANAGER_SINGLETON::instance ();
      ACE_ASSERT (directshow_connection_manager_p);
      directshow_connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_connection_manager_p =
        ARDRONE_MEDIAFOUNDATION_CONNECTIONMANAGER_SINGLETON::instance ();
      ACE_ASSERT (mediafoundation_connection_manager_p);
      mediafoundation_connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  CBData_in.mediaFramework));
      return;
    }
  } // end SWITCH
#else
  ARDrone_ConnectionManager_t* connection_manager_p =
    ARDRONE_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max ());
#endif

  Stream_Base_t* stream_base_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_ControlStream_t directshow_control_stream;
  ARDrone_DirectShow_MAVLinkStream_t directshow_mavlink_stream;
  // *TODO*: some AR drones use flashed firmware that supports 'MAVLink'
  //         communications instead of the 'NavData' stream documented in the
  //         developer guide (this is a total mess, apparently)
  ARDrone_DirectShow_NavDataStream_t directshow_navdata_stream;
  ARDrone_DirectShow_AsynchVideoStream_t directshow_asynch_video_stream;
  ARDrone_DirectShow_VideoStream_t directshow_video_stream;
  ARDrone_MediaFoundation_ControlStream_t mediafoundation_control_stream;
  ARDrone_MediaFoundation_MAVLinkStream_t mediafoundation_mavlink_stream;
  // *TODO*: some AR drones use flashed firmware that supports 'MAVLink'
  //         communications instead of the 'NavData' stream documented in the
  //         developer guide (this is a total mess, apparently)
  ARDrone_MediaFoundation_NavDataStream_t mediafoundation_navdata_stream;
  ARDrone_MediaFoundation_AsynchVideoStream_t mediafoundation_asynch_video_stream;
  ARDrone_MediaFoundation_VideoStream_t mediafoundation_video_stream;
#else
  ARDrone_ControlStream_t control_stream;
  ARDrone_MAVLinkStream_t mavlink_stream;
  // *TODO*: some AR drones use flashed firmware that supports 'MAVLink'
  //         communications instead of the 'NavData' stream documented in the
  //         developer guide (this is a total mess, apparently)
  ARDrone_NavDataStream_t navdata_stream;
  ARDrone_AsynchVideoStream_t asynch_video_stream;
  ARDrone_VideoStream_t video_stream;
#endif
  Common_Timer_Manager_t* timer_manager_p = NULL;
  struct Common_TimerConfiguration timer_configuration;
  int group_id = -1;
  struct Common_EventDispatchThreadData thread_data_s;
  ARDrone_GTK_Manager_t* gtk_manager_p = NULL;
  struct ARDrone_StreamConfiguration stream_configuration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_ConnectionConfiguration_t directshow_connection_configuration;
  ARDrone_DirectShow_ConnectionConfigurationIterator_t directshow_connection_iterator;
  ARDrone_MediaFoundation_ConnectionConfiguration_t mediafoundation_connection_configuration;
  ARDrone_MediaFoundation_ConnectionConfigurationIterator_t mediafoundation_connection_iterator;
  ARDrone_DirectShow_StreamConnectionConfigurations_t directshow_connection_configurations;
  ARDrone_DirectShow_StreamConnectionConfigurationIterator_t directshow_stream_connection_iterator;
  ARDrone_MediaFoundation_StreamConnectionConfigurations_t mediafoundation_connection_configurations;
  ARDrone_MediaFoundation_StreamConnectionConfigurationIterator_t mediafoundation_stream_connection_iterator;
#else
  ARDrone_ConnectionConfiguration_t connection_configuration;
  ARDrone_ConnectionConfigurationIterator_t connection_iterator;
  ARDrone_StreamConnectionConfigurations_t connection_configurations;
  ARDrone_StreamConnectionConfigurationIterator_t stream_connection_iterator;
#endif
  struct Stream_ModuleConfiguration module_configuration;

  timer_manager_p =
    COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  if (!timer_manager_p->initialize (timer_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize timer manager, returning\n")));
    return;
  } // end IF

  // sanity check(s)
  ACE_ASSERT (CBData_in.configuration);

  //CBData_in.controller =
  //    const_cast<ARDrone_IController*> (navdata_stream.getP ());
  CBData_in.configuration->parserConfiguration.debugScanner = debugScanner_in;

  //  module_configuration.generateUniqueNames = true;

  stream_configuration.CBData = &CBData_in;
  stream_configuration.initializeControl = &event_handler;
  stream_configuration.initializeMAVLink = &event_handler;
  stream_configuration.initializeNavData = &event_handler;
  stream_configuration.messageAllocator = &message_allocator;
  stream_configuration.printFinalReport = false;
  stream_configuration.useReactor = useReactor_in;
  stream_configuration.userData = CBData_in.configuration->userData;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_Module_DirectShow_EventHandler_Module directshow_event_handler_module (NULL,
                                                                                 ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_HANDLER_NAME_STRING));
  ARDrone_Module_MediaFoundation_EventHandler_Module mediafoundation_event_handler_module (NULL,
                                                                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_HANDLER_NAME_STRING));

  struct ARDrone_DirectShow_ModuleHandlerConfiguration directshow_modulehandler_configuration;
  ARDrone_DirectShow_StreamConfiguration_t directshow_stream_configuration_2;
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_control_streamconfiguration_iterator,
    directshow_mavlink_streamconfiguration_iterator,
    directshow_navdata_streamconfiguration_iterator,
    directshow_video_streamconfiguration_iterator,
    directshow_network_streamconfiguration_iterator;
  struct ARDrone_MediaFoundation_ModuleHandlerConfiguration mediafoundation_modulehandler_configuration;
  ARDrone_MediaFoundation_StreamConfiguration_t mediafoundation_stream_configuration_2;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_control_streamconfiguration_iterator,
    mediafoundation_mavlink_streamconfiguration_iterator,
    mediafoundation_navdata_streamconfiguration_iterator,
    mediafoundation_video_streamconfiguration_iterator,
    mediafoundation_network_streamconfiguration_iterator;
  switch (CBData_in.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      // sanity check(s)
      ACE_ASSERT (CBData_in.configuration->directShowPinConfiguration.format);

      if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*CBData_in.configuration->directShowPinConfiguration.format,
                                                                 directshow_modulehandler_configuration.inputFormat))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), returning\n")));
        goto clean;
      } // end IF
      ACE_ASSERT (directshow_modulehandler_configuration.inputFormat);

      directshow_modulehandler_configuration.CBData = &CBData_in;
      directshow_modulehandler_configuration.connectionManager =
        directshow_connection_manager_p;
      //directshow_modulehandler_configuration.consoleMode =
      //  UIInterfaceDefinitionFile_in.empty ();
      directshow_modulehandler_configuration.demultiplex = true;
      directshow_modulehandler_configuration.finishOnDisconnect = true;
      directshow_modulehandler_configuration.fullScreen = fullScreen_in;
      directshow_modulehandler_configuration.parserConfiguration =
        &CBData_in.configuration->parserConfiguration;
      directshow_modulehandler_configuration.statisticReportingInterval =
        ACE_Time_Value (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0);
      directshow_modulehandler_configuration.subscriber = &event_handler;
      directshow_modulehandler_configuration.subscribers =
        &CBData_in.configuration->streamSubscribers;
      directshow_modulehandler_configuration.subscribersLock =
        &CBData_in.configuration->streamSubscribersLock;
      //directshow_modulehandler_configuration.useYYScanBuffer = false;

      stream_configuration.module = &directshow_event_handler_module;

      // control
      directshow_stream_configuration_2.initialize (module_configuration,
                                                    directshow_modulehandler_configuration,
                                                    CBData_in.configuration->allocatorConfiguration,
                                                    stream_configuration);
      CBData_in.configuration->directShowStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING),
                                                                                      directshow_stream_configuration_2));

      // mavlink
      directshow_stream_configuration_2.initialize (module_configuration,
                                                    directshow_modulehandler_configuration,
                                                    CBData_in.configuration->allocatorConfiguration,
                                                    stream_configuration);
      CBData_in.configuration->directShowStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING),
                                                                                      directshow_stream_configuration_2));

      // navdata
      directshow_stream_configuration_2.initialize (module_configuration,
                                                    directshow_modulehandler_configuration,
                                                    CBData_in.configuration->allocatorConfiguration,
                                                    stream_configuration);
      CBData_in.configuration->directShowStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING),
                                                                                      directshow_stream_configuration_2));

      // video (already added in main())
      //CBData_in.configuration->streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
      //                                                                      stream_configuration));

      // network i/o
      // *NOTE*: do not specify a final module as these streams will be
      //         pre-/appended to the other streams
      stream_configuration.module = NULL;
      directshow_stream_configuration_2.initialize (module_configuration,
                                                    directshow_modulehandler_configuration,
                                                    CBData_in.configuration->allocatorConfiguration,
                                                    stream_configuration);
      CBData_in.configuration->directShowStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (STREAM_NET_DEFAULT_NAME_STRING),
                                                                                      directshow_stream_configuration_2));

      directshow_control_streamconfiguration_iterator =
        CBData_in.configuration->directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_control_streamconfiguration_iterator != CBData_in.configuration->directShowStreamConfigurations.end ());
      directshow_mavlink_streamconfiguration_iterator =
        CBData_in.configuration->directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_mavlink_streamconfiguration_iterator != CBData_in.configuration->directShowStreamConfigurations.end ());
      directshow_navdata_streamconfiguration_iterator =
        CBData_in.configuration->directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_navdata_streamconfiguration_iterator != CBData_in.configuration->directShowStreamConfigurations.end ());
      directshow_video_streamconfiguration_iterator =
        CBData_in.configuration->directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_video_streamconfiguration_iterator != CBData_in.configuration->directShowStreamConfigurations.end ());

      directshow_network_streamconfiguration_iterator =
        CBData_in.configuration->directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (STREAM_NET_DEFAULT_NAME_STRING));
      ACE_ASSERT (directshow_network_streamconfiguration_iterator != CBData_in.configuration->directShowStreamConfigurations.end ());

      // reset video stream module configuration (it was added in main)
      (*directshow_video_streamconfiguration_iterator).second.allocatorConfiguration_.defaultBufferSize =
        std::max (bufferSize_in,
                  static_cast<unsigned int> (ARDRONE_MESSAGE_BUFFER_SIZE));
      (*directshow_video_streamconfiguration_iterator).second.configuration_.CBData =
        &CBData_in;
      (*directshow_video_streamconfiguration_iterator).second.configuration_.finishOnDisconnect =
        true;
      (*directshow_video_streamconfiguration_iterator).second.configuration_.messageAllocator =
        &message_allocator;
      (*directshow_video_streamconfiguration_iterator).second.configuration_.module =
        &directshow_event_handler_module;
      (*directshow_video_streamconfiguration_iterator).second.configuration_.printFinalReport =
        false;
      (*directshow_video_streamconfiguration_iterator).second.configuration_.useReactor =
        useReactor_in;
      (*directshow_video_streamconfiguration_iterator).second.configuration_.userData =
        CBData_in.configuration->userData;

      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_modulehandler_configuration.CBData = &CBData_in;
      mediafoundation_modulehandler_configuration.connectionManager =
        mediafoundation_connection_manager_p;
      //mediafoundation_modulehandler_configuration.consoleMode =
      //  UIInterfaceDefinitionFile_in.empty ();
      mediafoundation_modulehandler_configuration.demultiplex = true;
      mediafoundation_modulehandler_configuration.finishOnDisconnect = true;
      mediafoundation_modulehandler_configuration.fullScreen = fullScreen_in;
      mediafoundation_modulehandler_configuration.parserConfiguration =
        &CBData_in.configuration->parserConfiguration;
      mediafoundation_modulehandler_configuration.statisticReportingInterval =
        ACE_Time_Value (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0);
      mediafoundation_modulehandler_configuration.subscriber = &event_handler;
      mediafoundation_modulehandler_configuration.subscribers =
        &CBData_in.configuration->streamSubscribers;
      mediafoundation_modulehandler_configuration.subscribersLock =
        &CBData_in.configuration->streamSubscribersLock;
      //mediafoundation_modulehandler_configuration.useYYScanBuffer = false;

      stream_configuration.module = &mediafoundation_event_handler_module;

      // control
      mediafoundation_stream_configuration_2.initialize (module_configuration,
                                                         mediafoundation_modulehandler_configuration,
                                                         CBData_in.configuration->allocatorConfiguration,
                                                         stream_configuration);
      CBData_in.configuration->mediaFoundationStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING),
                                                                                           mediafoundation_stream_configuration_2));

      // mavlink
      mediafoundation_stream_configuration_2.initialize (module_configuration,
                                                         mediafoundation_modulehandler_configuration,
                                                         CBData_in.configuration->allocatorConfiguration,
                                                         stream_configuration);
      CBData_in.configuration->mediaFoundationStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING),
                                                                                           mediafoundation_stream_configuration_2));

      // navdata
      mediafoundation_stream_configuration_2.initialize (module_configuration,
                                                         mediafoundation_modulehandler_configuration,
                                                         CBData_in.configuration->allocatorConfiguration,
                                                         stream_configuration);
      CBData_in.configuration->mediaFoundationStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING),
                                                                                           mediafoundation_stream_configuration_2));

      // video (already added in main())
      //CBData_in.configuration->streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
      //                                                                      stream_configuration));

      // network i/o
      // *NOTE*: do not specify a final module as these streams will be
      //         pre-/appended to the other streams
      stream_configuration.module = NULL;
      mediafoundation_stream_configuration_2.initialize (module_configuration,
                                                         mediafoundation_modulehandler_configuration,
                                                         CBData_in.configuration->allocatorConfiguration,
                                                         stream_configuration);
      CBData_in.configuration->mediaFoundationStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (STREAM_NET_DEFAULT_NAME_STRING),
                                                                                           mediafoundation_stream_configuration_2));

      mediafoundation_control_streamconfiguration_iterator =
        CBData_in.configuration->mediaFoundationStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_control_streamconfiguration_iterator != CBData_in.configuration->mediaFoundationStreamConfigurations.end ());
      mediafoundation_mavlink_streamconfiguration_iterator =
        CBData_in.configuration->mediaFoundationStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_mavlink_streamconfiguration_iterator != CBData_in.configuration->mediaFoundationStreamConfigurations.end ());
      mediafoundation_navdata_streamconfiguration_iterator =
        CBData_in.configuration->mediaFoundationStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_navdata_streamconfiguration_iterator != CBData_in.configuration->mediaFoundationStreamConfigurations.end ());
      mediafoundation_video_streamconfiguration_iterator =
        CBData_in.configuration->mediaFoundationStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_video_streamconfiguration_iterator != CBData_in.configuration->mediaFoundationStreamConfigurations.end ());

      mediafoundation_network_streamconfiguration_iterator =
        CBData_in.configuration->mediaFoundationStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (STREAM_NET_DEFAULT_NAME_STRING));
      ACE_ASSERT (mediafoundation_network_streamconfiguration_iterator != CBData_in.configuration->mediaFoundationStreamConfigurations.end ());

      // reset video stream module configuration (it was added in main)
      (*mediafoundation_video_streamconfiguration_iterator).second.allocatorConfiguration_.defaultBufferSize =
        std::max (bufferSize_in,
                  static_cast<unsigned int> (ARDRONE_MESSAGE_BUFFER_SIZE));
      (*mediafoundation_video_streamconfiguration_iterator).second.configuration_.CBData =
        &CBData_in;
      (*mediafoundation_video_streamconfiguration_iterator).second.configuration_.finishOnDisconnect =
        true;
      (*mediafoundation_video_streamconfiguration_iterator).second.configuration_.messageAllocator =
        &message_allocator;
      (*mediafoundation_video_streamconfiguration_iterator).second.configuration_.module =
        &mediafoundation_event_handler_module;
      (*mediafoundation_video_streamconfiguration_iterator).second.configuration_.printFinalReport =
        false;
      (*mediafoundation_video_streamconfiguration_iterator).second.configuration_.useReactor =
        useReactor_in;
      (*mediafoundation_video_streamconfiguration_iterator).second.configuration_.userData =
        CBData_in.configuration->userData;

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  CBData_in.mediaFramework));
      return;
    }
  } // end SWITCH
#else
 ARDrone_Module_EventHandler_Module event_handler_module (NULL,
                                                          ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_HANDLER_NAME_STRING));

 struct ARDrone_ModuleHandlerConfiguration modulehandler_configuration;
  ARDrone_StreamConfiguration_t stream_configuration_2;
  ARDrone_StreamConfigurationsIterator_t control_streamconfiguration_iterator,
    mavlink_streamconfiguration_iterator,
    navdata_streamconfiguration_iterator,
    video_streamconfiguration_iterator,
    network_streamconfiguration_iterator;

  modulehandler_configuration.CBData = &CBData_in;
  modulehandler_configuration.connectionManager = connection_manager_p;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  modulehandler_configuration.consoleMode = UIInterfaceDefinitionFile_in.empty ();
#endif
  modulehandler_configuration.demultiplex = true;
  modulehandler_configuration.finishOnDisconnect = true;
  modulehandler_configuration.fullScreen = fullScreen_in;
  modulehandler_configuration.parserConfiguration =
    &CBData_in.configuration->parserConfiguration;
  modulehandler_configuration.frameRate.den = 1;
  modulehandler_configuration.frameRate.num = 30;
  modulehandler_configuration.pixelBufferLock = &CBData_in.lock;
  modulehandler_configuration.statisticReportingInterval =
    ACE_Time_Value (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0);
  modulehandler_configuration.subscriber = &event_handler;
  modulehandler_configuration.subscribers =
      &CBData_in.configuration->streamSubscribers;
  modulehandler_configuration.subscribersLock =
      &CBData_in.configuration->streamSubscribersLock;
  //modulehandler_configuration.useYYScanBuffer = false;

  stream_configuration.module = &event_handler_module;

  // control
  stream_configuration.initializeControl = &event_handler;
  stream_configuration_2.initialize (module_configuration,
                                     modulehandler_configuration,
                                     CBData_in.configuration->allocatorConfiguration,
                                     stream_configuration);
  CBData_in.configuration->streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING),
                                                                        stream_configuration_2));

  // mavlink
  stream_configuration.initializeMAVLink = &event_handler;
  stream_configuration_2.initialize (module_configuration,
                                     modulehandler_configuration,
                                     CBData_in.configuration->allocatorConfiguration,
                                     stream_configuration);
  CBData_in.configuration->streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING),
                                                                        stream_configuration_2));

  // navdata
  stream_configuration.initializeNavData = &event_handler;
  stream_configuration_2.initialize (module_configuration,
                                     modulehandler_configuration,
                                     CBData_in.configuration->allocatorConfiguration,
                                     stream_configuration);
  CBData_in.configuration->streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING),
                                                                        stream_configuration_2));

  // video (already added in main())
  //CBData_in.configuration->streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
  //                                                                      stream_configuration));

  // network i/o
  // *NOTE*: do not specify a final module as these streams will be
  //         pre-/appended to the other streams
  stream_configuration.module = NULL;
  stream_configuration_2.initialize (module_configuration,
                                     modulehandler_configuration,
                                     CBData_in.configuration->allocatorConfiguration,
                                     stream_configuration);
  CBData_in.configuration->streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (STREAM_NET_DEFAULT_NAME_STRING),
                                                                        stream_configuration_2));

  control_streamconfiguration_iterator =
    CBData_in.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
  ACE_ASSERT (control_streamconfiguration_iterator != CBData_in.configuration->streamConfigurations.end ());
  mavlink_streamconfiguration_iterator =
    CBData_in.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
  ACE_ASSERT (mavlink_streamconfiguration_iterator != CBData_in.configuration->streamConfigurations.end ());
  navdata_streamconfiguration_iterator =
    CBData_in.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
  ACE_ASSERT (navdata_streamconfiguration_iterator != CBData_in.configuration->streamConfigurations.end ());
  video_streamconfiguration_iterator =
    CBData_in.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (video_streamconfiguration_iterator != CBData_in.configuration->streamConfigurations.end ());

  network_streamconfiguration_iterator =
    CBData_in.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (STREAM_NET_DEFAULT_NAME_STRING));
  ACE_ASSERT (network_streamconfiguration_iterator != CBData_in.configuration->streamConfigurations.end ());

  (*video_streamconfiguration_iterator).second.allocatorConfiguration_.defaultBufferSize =
      std::max (bufferSize_in,
                static_cast<unsigned int> (ARDRONE_MESSAGE_BUFFER_SIZE));
  (*video_streamconfiguration_iterator).second.configuration_.CBData =
      &CBData_in;
  (*video_streamconfiguration_iterator).second.configuration_.finishOnDisconnect =
      true;
  (*video_streamconfiguration_iterator).second.configuration_.messageAllocator =
      &message_allocator;
  (*video_streamconfiguration_iterator).second.configuration_.module =
      &event_handler_module;
  (*video_streamconfiguration_iterator).second.configuration_.printFinalReport =
      false;
  (*video_streamconfiguration_iterator).second.configuration_.useReactor =
      useReactor_in;
  (*video_streamconfiguration_iterator).second.configuration_.userData =
      CBData_in.configuration->userData;
#endif

  // ******************* socket configuration data ****************************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
  CBData_in.configuration->WLANMonitorConfiguration.debug = debugNl80211_in;
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
  // *TODO*: implement an elegant way to specify a network interface on Win32
  CBData_in.configuration->WLANMonitorConfiguration.interfaceIdentifier =
    interfaceIdentifier_in;
  CBData_in.configuration->WLANMonitorConfiguration.SSID = SSID_in;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  CBData_in.configuration->WLANMonitorConfiguration.timerInterface =
    timer_manager_p;
#endif
  CBData_in.configuration->WLANMonitorConfiguration.useReactor = useReactor_in;
  CBData_in.configuration->WLANMonitorConfiguration.userData =
    CBData_in.configuration->userData;
  if (!WLAN_monitor_p->initialize (CBData_in.configuration->WLANMonitorConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_WLANMonitor_T::initialize(), returning\n")));
    goto clean;
  } // end IF
  // *NOTE*: start the WLAN monitor after initializing the NavData stream to
  //         enable the event notification callbacks (see below #2174)

  // control
  // *TODO*: bind to a specific interface
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (CBData_in.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      // control
      directshow_connection_configuration.connectionManager =
        directshow_connection_manager_p;
      directshow_connection_configuration.generateUniqueIOModuleNames = true;
      directshow_connection_configuration.messageAllocator = &message_allocator;
      directshow_connection_configuration.PDUSize =
        std::max (bufferSize_in,
                  static_cast<unsigned int> (ARDRONE_MESSAGE_BUFFER_SIZE));
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address =
        address_in;
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (ARDRONE_PORT_TCP_CONTROL,
                                                                                                                    1);
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_2.bufferSize =
        NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
      directshow_connection_configuration.socketHandlerConfiguration.statisticReportingInterval =
        ACE_Time_Value (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0);
      directshow_connection_configuration.socketHandlerConfiguration.userData =
        CBData_in.configuration->userData;
      directshow_connection_configuration.userData =
        CBData_in.configuration->userData;

      directshow_connection_configuration.initialize (CBData_in.configuration->allocatorConfiguration,
                                                      (*directshow_network_streamconfiguration_iterator).second);

      directshow_connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                   directshow_connection_configuration));
      CBData_in.configuration->directShowConnectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING),
                                                                                          directshow_connection_configurations));
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());
      directshow_stream_connection_iterator = (*directshow_connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      ACE_ASSERT (directshow_stream_connection_iterator != (*directshow_connection_iterator).second.end ());
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
        &((*directshow_stream_connection_iterator).second);
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
        &(*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_2;

      // mavlink
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.bufferSize =
        NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
      result =
        directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.listenAddress.set (static_cast<u_short> (ARDRONE_PORT_UDP_MAVLINK),
                                                                                                                static_cast<ACE_UINT32> (INADDR_ANY),
                                                                                                                1,
                                                                                                                0);
      ACE_ASSERT (result == 0);
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress.reset ();
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.writeOnly =
        false;
      //  connection_configuration.streamConfiguration =
      //    &((*mavlink_streamconfiguration_iterator).second);
      directshow_connection_configurations.clear ();
      directshow_connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                   directshow_connection_configuration));

      CBData_in.configuration->directShowConnectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING),
                                                                                          directshow_connection_configurations));
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());
      directshow_stream_connection_iterator = (*directshow_connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      ACE_ASSERT (directshow_stream_connection_iterator != (*directshow_connection_iterator).second.end ());
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
        &((*directshow_stream_connection_iterator).second);
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
        &(*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_3;

      // navdata
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.listenAddress.reset ();
      // *TODO*: bind to a specific interface
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress =
        address_in;
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress.set_port_number (ARDRONE_PORT_UDP_CONTROL_CONFIGURATION,
                                                                                                             1);
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.connect =
        !useReactor_in;
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.writeOnly =
        true;
      //  connection_configuration.streamConfiguration =
      //    &((*navdata_streamconfiguration_iterator).second);
      directshow_connection_configurations.clear ();
      directshow_connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING),
                                                                   directshow_connection_configuration));
      CBData_in.configuration->directShowConnectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING),
                                                                                          directshow_connection_configurations));
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());
      directshow_stream_connection_iterator = (*directshow_connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
      ACE_ASSERT (directshow_stream_connection_iterator != (*directshow_connection_iterator).second.end ());
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
        &((*directshow_stream_connection_iterator).second);
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
        &(*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_3;

      result =
        directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.listenAddress.set (static_cast<u_short> (ARDRONE_PORT_UDP_CONTROL_CONFIGURATION),
                                                                                                                static_cast<ACE_UINT32> (INADDR_ANY),
                                                                                                                1,
                                                                                                                0);
      ACE_ASSERT (result == 0);
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress.base_set (AF_ANY,
                                                                                                                 -1);
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.connect =
        false;
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.sourcePort =
        0;
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_3.writeOnly =
        false;
      //  directshow_connection_configuration.streamConfiguration =
      //    &((*navdata_streamconfiguration_iterator).second);
      (*directshow_connection_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                       directshow_connection_configuration));
      directshow_stream_connection_iterator = (*directshow_connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      ACE_ASSERT (directshow_stream_connection_iterator != (*directshow_connection_iterator).second.end ());
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
        &((*directshow_stream_connection_iterator).second);
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
        &(*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_3;

      // video
      //  // *TODO*: verify the given address
      //  if (!Net_Common_Tools::IPAddress2Interface (address_in,
      //                                              interface_identifier_string))
      //  {
      //    ACE_DEBUG ((LM_ERROR,
      //                ACE_TEXT ("failed to Net_Common_Tools::IPAddressToInterface(%s), returning\n"),
      //                ACE_TEXT (Net_Common_Tools::IPAddressToString (address_in).c_str ())));
      //    goto error;
      //  } // end IF
      //  if (!Net_Common_Tools::interface2IPAddress (interface_identifier_string,
      //                                              CBData_in.localSAP))
      //  {
      //    ACE_DEBUG ((LM_ERROR,
      //                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(%s), returning\n"),
      //                ACE_TEXT (interface_identifier_string.c_str ())));
      //    goto error;
      //  } // end IF
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("set local SAP: %s...\n"),
      //              ACE_TEXT (Net_Common_Tools::IPAddressToString (CBData_in.localSAP).c_str ())));
      directshow_connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (ARDRONE_PORT_TCP_VIDEO,
                                                                                                                    1);
      //  connection_configuration.streamConfiguration =
      //    &((*video_streamconfiguration_iterator).second);
      directshow_connection_configurations.clear ();
      directshow_connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                   directshow_connection_configuration));
      CBData_in.configuration->directShowConnectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
                                                                                          directshow_connection_configurations));
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());
      directshow_stream_connection_iterator = (*directshow_connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      ACE_ASSERT (directshow_stream_connection_iterator != (*directshow_connection_iterator).second.end ());
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
        &((*directshow_stream_connection_iterator).second);
      (*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
        &(*directshow_stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_2;
      //  iterator_2 =
      //    CBData_in.configuration->mediaFoundationConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
      //  ACE_ASSERT (iterator_2 != CBData_in.configuration->mediaFoundationConnectionConfigurations.end ());

      // ******************** stream configuration data ***************************
      // control
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());

      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      //connection_configuration.mediaFoundationStreamConfiguration =
      //  &((*mediafoundation_network_streamconfiguration_iterator).second);
      //connection_configuration.streamConfiguration =
      //  connection_configuration.mediaFoundationStreamConfiguration;
      ACE_ASSERT (false); // *TODO*
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  CBData_in.mediaFramework));
      return;
    }
  } // end SWITCH
#else
  // control
  connection_configuration.connectionManager = connection_manager_p;
  connection_configuration.generateUniqueIOModuleNames = true;
  connection_configuration.messageAllocator = &message_allocator;
  connection_configuration.PDUSize =
    std::max (bufferSize_in,
              static_cast<unsigned int> (ARDRONE_MESSAGE_BUFFER_SIZE));
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address =
    address_in;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (ARDRONE_PORT_TCP_CONTROL,
                                                                                                     1);
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.bufferSize =
    NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
  connection_configuration.socketHandlerConfiguration.statisticReportingInterval =
    ACE_Time_Value (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0);
  connection_configuration.socketHandlerConfiguration.userData =
    CBData_in.configuration->userData;
  connection_configuration.userData = CBData_in.configuration->userData;

  connection_configuration.initialize (CBData_in.configuration->allocatorConfiguration,
                                       (*network_streamconfiguration_iterator).second);

  connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                    connection_configuration));
  CBData_in.configuration->connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING),
                                                                            connection_configurations));
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
  stream_connection_iterator = (*connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (stream_connection_iterator != (*connection_iterator).second.end ());
  (*stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*stream_connection_iterator).second);
  (*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
    &(*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_2;

  // mavlink
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.bufferSize =
    NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
  result =
    connection_configuration.socketHandlerConfiguration.socketConfiguration_3.listenAddress.set (static_cast<u_short> (ARDRONE_PORT_UDP_MAVLINK),
                                                                                                 static_cast<ACE_UINT32> (INADDR_ANY),
                                                                                                 1,
                                                                                                 0);
  ACE_ASSERT (result == 0);
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress.reset ();
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.writeOnly =
    false;
  //  connection_configuration.streamConfiguration =
  //    &((*mavlink_streamconfiguration_iterator).second);
  connection_configurations.clear ();
  connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                    connection_configuration));

  CBData_in.configuration->connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING),
                                                                            connection_configurations));
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
  stream_connection_iterator = (*connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (stream_connection_iterator != (*connection_iterator).second.end ());
  (*stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*stream_connection_iterator).second);
  (*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
    &(*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_3;

  // navdata
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.listenAddress.reset ();
  // *TODO*: bind to a specific interface
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress =
    address_in;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress.set_port_number (ARDRONE_PORT_UDP_CONTROL_CONFIGURATION,
                                                                                                         1);
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.connect =
    !useReactor_in;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.writeOnly =
    true;
  //  connection_configuration.streamConfiguration =
  //    &((*navdata_streamconfiguration_iterator).second);
  connection_configurations.clear ();
  connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING),
                                                    connection_configuration));
  CBData_in.configuration->connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING),
                                                                            connection_configurations));
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
  stream_connection_iterator = (*connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
  ACE_ASSERT (stream_connection_iterator != (*connection_iterator).second.end ());
  (*stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*stream_connection_iterator).second);
  (*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
    &(*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_3;

  result =
    connection_configuration.socketHandlerConfiguration.socketConfiguration_3.listenAddress.set (static_cast<u_short> (ARDRONE_PORT_UDP_CONTROL_CONFIGURATION),
                                                                                                 static_cast<ACE_UINT32> (INADDR_ANY),
                                                                                                 1,
                                                                                                 0);
  ACE_ASSERT (result == 0);
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.peerAddress.base_set (AF_ANY,
                                                                                                  -1);
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.connect =
    false;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.sourcePort =
    0;
  connection_configuration.socketHandlerConfiguration.socketConfiguration_3.writeOnly =
    false;
  //  connection_configuration.streamConfiguration =
  //    &((*navdata_streamconfiguration_iterator).second);
  (*connection_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                        connection_configuration));
  stream_connection_iterator = (*connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (stream_connection_iterator != (*connection_iterator).second.end ());
  (*stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*stream_connection_iterator).second);
  (*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
    &(*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_3;

  // video
  //  // *TODO*: verify the given address
  //  if (!Net_Common_Tools::IPAddress2Interface (address_in,
  //                                              interface_identifier_string))
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to Net_Common_Tools::IPAddressToInterface(%s), returning\n"),
  //                ACE_TEXT (Net_Common_Tools::IPAddressToString (address_in).c_str ())));
  //    goto error;
  //  } // end IF
  //  if (!Net_Common_Tools::interface2IPAddress (interface_identifier_string,
  //                                              CBData_in.localSAP))
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(%s), returning\n"),
  //                ACE_TEXT (interface_identifier_string.c_str ())));
  //    goto error;
  //  } // end IF
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("set local SAP: %s...\n"),
  //              ACE_TEXT (Net_Common_Tools::IPAddressToString (CBData_in.localSAP).c_str ())));
  connection_configuration.socketHandlerConfiguration.socketConfiguration_2.address.set_port_number (ARDRONE_PORT_TCP_VIDEO,
                                                                                                     1);
  //  connection_configuration.streamConfiguration =
  //    &((*video_streamconfiguration_iterator).second);
  connection_configurations.clear ();
  connection_configurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                    connection_configuration));
  CBData_in.configuration->connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
                                                                            connection_configurations));
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
  stream_connection_iterator = (*connection_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (stream_connection_iterator != (*connection_iterator).second.end ());
  (*stream_connection_iterator).second.socketHandlerConfiguration.connectionConfiguration =
    &((*stream_connection_iterator).second);
  (*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration =
    &(*stream_connection_iterator).second.socketHandlerConfiguration.socketConfiguration_2;
  //  stream_connection_iterator =
  //    CBData_in.configuration->mediaFoundationConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  //  ACE_ASSERT (stream_connection_iterator != CBData_in.configuration->mediaFoundationConnectionConfigurations.end ());

  // ******************** stream configuration data ***************************
  // control
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (CBData_in.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      CBData_in.configuration->directShowFilterConfiguration.allocatorProperties.cbBuffer =
        directshow_modulehandler_configuration.inputFormat->lSampleSize;
      CBData_in.configuration->directShowPinConfiguration.isTopToBottom = true;

      directshow_modulehandler_configuration.connectionConfigurations =
        &(*directshow_connection_iterator).second;

      (*directshow_control_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                        std::make_pair (module_configuration,
                                                                                                        directshow_modulehandler_configuration)));

      // mavlink
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());
      directshow_modulehandler_configuration.connectionConfigurations =
        &(*directshow_connection_iterator).second;
      (*directshow_mavlink_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                        std::make_pair (module_configuration,
                                                                                                        directshow_modulehandler_configuration)));

      // navdata
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());
      directshow_modulehandler_configuration.connectionConfigurations =
        &(*directshow_connection_iterator).second;
      (*directshow_navdata_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                        std::make_pair (module_configuration,
                                                                                                        directshow_modulehandler_configuration)));
      (*directshow_navdata_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING),
                                                                                        std::make_pair (module_configuration,
                                                                                                        directshow_modulehandler_configuration)));

      // video
      directshow_connection_iterator =
        CBData_in.configuration->directShowConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_connection_iterator != CBData_in.configuration->directShowConnectionConfigurations.end ());
      directshow_modulehandler_configuration.connectionConfigurations =
        &(*directshow_connection_iterator).second;
      (*directshow_video_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                      std::make_pair (module_configuration,
                                                                                                      directshow_modulehandler_configuration)));
      directshow_modulehandler_configuration.connectionConfigurations = NULL;
      (*directshow_video_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING),
                                                                                      std::make_pair (module_configuration,
                                                                                                      directshow_modulehandler_configuration)));

      //  network
      module_configuration.generateUniqueNames = true;
      (*directshow_network_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_IO_DEFAULT_NAME_STRING),
                                                                                        std::make_pair (module_configuration,
                                                                                                        directshow_modulehandler_configuration)));

      CBData_in.directShowControlStream = &directshow_control_stream;
      CBData_in.directShowMAVLinkStream = &directshow_mavlink_stream;
      CBData_in.directShowNavDataStream = &directshow_navdata_stream;
      if (useReactor_in)
        CBData_in.directShowVideoStream = &directshow_video_stream;
      else
        CBData_in.directShowAsynchVideoStream =
          &directshow_asynch_video_stream;

      // step2: initialize connection manager
      //configuration_2.streamConfiguration->module = NULL;
      directshow_connection_manager_p->set ((*directshow_stream_connection_iterator).second,
                                            CBData_in.configuration->userData); // passed to all handlers

      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_modulehandler_configuration.connectionConfigurations =
        &(*mediafoundation_connection_iterator).second;

      (*mediafoundation_control_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                             std::make_pair (module_configuration,
                                                                                                             mediafoundation_modulehandler_configuration)));

      // mavlink
      mediafoundation_connection_iterator =
        CBData_in.configuration->mediaFoundationConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_connection_iterator != CBData_in.configuration->mediaFoundationConnectionConfigurations.end ());
      mediafoundation_modulehandler_configuration.connectionConfigurations = &(*mediafoundation_connection_iterator).second;
      (*mediafoundation_mavlink_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                             std::make_pair (module_configuration,
                                                                                                             mediafoundation_modulehandler_configuration)));

      // navdata
      mediafoundation_connection_iterator =
        CBData_in.configuration->mediaFoundationConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_connection_iterator != CBData_in.configuration->mediaFoundationConnectionConfigurations.end ());
      mediafoundation_modulehandler_configuration.connectionConfigurations = &(*mediafoundation_connection_iterator).second;
      (*mediafoundation_navdata_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                             std::make_pair (module_configuration,
                                                                                                             mediafoundation_modulehandler_configuration)));
      (*mediafoundation_navdata_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING),
                                                                                             std::make_pair (module_configuration,
                                                                                                             mediafoundation_modulehandler_configuration)));

      // video
      mediafoundation_connection_iterator =
        CBData_in.configuration->mediaFoundationConnectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_connection_iterator != CBData_in.configuration->mediaFoundationConnectionConfigurations.end ());
      mediafoundation_modulehandler_configuration.connectionConfigurations = &(*mediafoundation_connection_iterator).second;
      (*mediafoundation_video_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                                           std::make_pair (module_configuration,
                                                                                                           mediafoundation_modulehandler_configuration)));
      mediafoundation_modulehandler_configuration.connectionConfigurations = NULL;
      (*mediafoundation_video_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING),
                                                                                           std::make_pair (module_configuration,
                                                                                                           mediafoundation_modulehandler_configuration)));

      //  network
      module_configuration.generateUniqueNames = true;
      (*mediafoundation_network_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_IO_DEFAULT_NAME_STRING),
                                                                                             std::make_pair (module_configuration,
                                                                                                             mediafoundation_modulehandler_configuration)));

      CBData_in.mediaFoundationControlStream = &mediafoundation_control_stream;
      CBData_in.mediaFoundationMAVLinkStream = &mediafoundation_mavlink_stream;
      CBData_in.mediaFoundationNavDataStream = &mediafoundation_navdata_stream;
      if (useReactor_in)
        CBData_in.mediaFoundationVideoStream = &mediafoundation_video_stream;
      else
        CBData_in.mediaFoundationAsynchVideoStream =
          &mediafoundation_asynch_video_stream;

      // step2: initialize connection manager
      //configuration_2.streamConfiguration->module = NULL;
      mediafoundation_connection_manager_p->set ((*mediafoundation_stream_connection_iterator).second,
                                                 CBData_in.configuration->userData); // passed to all handlers

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  CBData_in.mediaFramework));
      return;
    }
  } // end SWITCH
#else
  modulehandler_configuration.connectionConfigurations = &(*connection_iterator).second;

  (*control_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                         std::make_pair (module_configuration,
                                                                                         modulehandler_configuration)));

  // mavlink
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
  modulehandler_configuration.connectionConfigurations = &(*connection_iterator).second;
  (*mavlink_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                         std::make_pair (module_configuration,
                                                                                         modulehandler_configuration)));

  // navdata
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
  modulehandler_configuration.connectionConfigurations = &(*connection_iterator).second;
  (*navdata_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                         std::make_pair (module_configuration,
                                                                                         modulehandler_configuration)));
  (*navdata_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING),
                                                                         std::make_pair (module_configuration,
                                                                                         modulehandler_configuration)));

  // video
  connection_iterator =
    CBData_in.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (connection_iterator != CBData_in.configuration->connectionConfigurations.end ());
  modulehandler_configuration.connectionConfigurations = &(*connection_iterator).second;
  (*video_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                       std::make_pair (module_configuration,
                                                                                       modulehandler_configuration)));
  modulehandler_configuration.connectionConfigurations = NULL;
  (*video_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING),
                                                                       std::make_pair (module_configuration,
                                                                                       modulehandler_configuration)));
  if (useReactor_in)
    stream_base_p = &video_stream;
  else
    stream_base_p = &asynch_video_stream;

  //  network
  module_configuration.generateUniqueNames = true;
  (*network_streamconfiguration_iterator).second.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_IO_DEFAULT_NAME_STRING),
                                                                         std::make_pair (module_configuration,
                                                                                         modulehandler_configuration)));

  CBData_in.streams.insert (std::make_pair (control_stream_name_string_,
                                            &control_stream));
  CBData_in.streams.insert (std::make_pair (mavlink_stream_name_string_,
                                            &mavlink_stream));
  CBData_in.streams.insert (std::make_pair (navdata_stream_name_string_,
                                            &navdata_stream));
  CBData_in.streams.insert (std::make_pair (video_stream_name_string_,
                                            stream_base_p));

  // step2: initialize connection manager
  //configuration_2.streamConfiguration->module = NULL;
  connection_manager_p->set ((*stream_connection_iterator).second,
                             CBData_in.configuration->userData); // passed to all handlers
#endif

  // step3: initialize event dispatch
  enum Common_ProactorType proactor_type;
  enum Common_ReactorType reactor_type;
  bool serialize_output;
  if (!Common_Tools::initializeEventDispatch (useReactor_in,                                      // use reactor ? : proactor
                                              (useReactor_in &&
                                               (ARDRONE_DEFAULT_NUMBER_OF_DISPATCH_THREADS > 1)), // use thread pool reactor ?
                                              ARDRONE_DEFAULT_NUMBER_OF_DISPATCH_THREADS,         // # dispatching threads
                                              proactor_type,
                                              reactor_type,
                                              serialize_output))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    goto clean;
  } // end IF

  // step4: initialize signal handling
  CBData_in.configuration->signalHandlerConfiguration.hasUI =
      !UIInterfaceDefinitionFile_in.empty ();
  CBData_in.configuration->signalHandlerConfiguration.peerAddress = address_in;
  CBData_in.configuration->signalHandlerConfiguration.useReactor =
      useReactor_in;
  if (!signalHandler_in.initialize (CBData_in.configuration->signalHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handler, returning\n")));
    goto clean;
  } // end IF
  if (!Common_Signal_Tools::initialize ((useReactor_in ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                       : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                        signalSet_in,
                                        ignoredSignalSet_in,
                                        &signalHandler_in,
                                        previousSignalActions_inout))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handling, returning\n")));
    goto clean;
  } // end IF

  // intialize timers
  timer_manager_p->start ();

  // step1a: start GTK event loop ?
  gtk_manager_p = ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  if (!UIInterfaceDefinitionFile_in.empty ())
  {
    gtk_manager_p->start ();
    ACE_Time_Value delay (0,
                          ARDRONE_UI_INITIALIZATION_DELAY);
    result = ACE_OS::sleep (delay);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sleep(%#T): \"%m\", continuing\n"),
                  &delay));
    if (!gtk_manager_p->isRunning ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start GTK event dispatch, returning\n")));
      goto clean;
    } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    HWND window_p = GetConsoleWindow ();
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::GetConsoleWindow(), returning\n")));
      goto clean;
    } // end IF
    BOOL was_visible_b = false;
    if (!showConsole_in)
      was_visible_b = ShowWindow (window_p,
                                  SW_HIDE);
    ACE_UNUSED_ARG (was_visible_b);
#endif
  } // end IF

  // *WARNING*: from this point on, clean up any remote connections !

  // step6: start event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step6b: initialize worker(s)
  thread_data_s.numberOfDispatchThreads =
    ARDRONE_DEFAULT_NUMBER_OF_DISPATCH_THREADS;
  thread_data_s.proactorType = proactor_type;
  thread_data_s.reactorType = reactor_type;
  thread_data_s.useReactor = useReactor_in;
  if (!Common_Tools::startEventDispatch (thread_data_s,
                                         group_id))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, returning\n")));
//		{ // synch access
//			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard  (CBData_in.lock);

//			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin ();
//					 iterator != CBData_in.event_source_ids.end ();
//					 iterator++)
//				g_source_remove (*iterator);
//		} // end lock scope

    goto clean;
  } // end IF

  // *NOTE*: initialize the NavData stream early so it can process WLAN status
  //         updates
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (CBData_in.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      stream_name_string = directshow_navdata_stream.name ();
      result_2 =
        directshow_navdata_stream.initialize ((*directshow_navdata_streamconfiguration_iterator).second);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      stream_name_string = mediafoundation_navdata_stream.name ();
      result_2 =
        mediafoundation_navdata_stream.initialize ((*mediafoundation_navdata_streamconfiguration_iterator).second);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  CBData_in.mediaFramework));
      return;
    }
  } // end SWITCH
#else
  stream_name_string = navdata_stream.name ();
  result_2 =
    navdata_stream.initialize ((*navdata_streamconfiguration_iterator).second);
#endif
  if (!result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize \"%s\" stream, returning\n"),
                ACE_TEXT (stream_name_string.c_str ())));
    goto clean;
  } // end IF

  ACE_ASSERT (WLAN_monitor_p);
  WLAN_monitor_p->start ();
  if (!WLAN_monitor_p->isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_WLANMonitor_T::start(), returning\n")));
    goto clean;
  } // end IF

  if (UIInterfaceDefinitionFile_in.empty ())
  {
//    // *TODO*: verify the given address
//    if (!Net_Common_Tools::IPAddress2Interface (cb_data_p->configuration->socketConfigurations.back ().address,
//                                                interface_identifier_string))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Net_Common_Tools::IPAddress2Interface(%s), returning\n"),
//                  ACE_TEXT (Net_Common_Tools::IPAddress2String (cb_data_p->configuration->socketConfigurations.back ().address).c_str ())));
//      goto error;
//    } // end IF
//    if (!Net_Common_Tools::interface2IPAddress (interface_identifier_string,
//                                                cb_data_p->localSAP))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Net_Common_Tools::interface2IPAddress(%s), returning\n"),
//                  ACE_TEXT (interface_identifier_string.c_str ())));
//      goto error;
//    } // end IF
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("set local SAP: %s...\n"),
//                ACE_TEXT (Net_Common_Tools::IPAddress2String (cb_data_p->localSAP).c_str ())));

    // initialize processing streams
    Stream_IStreamControlBase* istream_control_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (CBData_in.mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        stream_name_string = directshow_control_stream.name ();
        result_2 =
          directshow_control_stream.initialize ((*directshow_control_streamconfiguration_iterator).second);
        if (!result_2)
          break;
        stream_name_string = directshow_mavlink_stream.name ();
        result_2 =
          directshow_mavlink_stream.initialize ((*directshow_mavlink_streamconfiguration_iterator).second);
        if (!result_2)
          break;
        stream_name_string =
          (useReactor_in ? directshow_video_stream.name ()
                         : directshow_asynch_video_stream.name ());
        result_2 =
          (useReactor_in ? directshow_video_stream.initialize ((*directshow_video_streamconfiguration_iterator).second)
                         : directshow_asynch_video_stream.initialize ((*directshow_video_streamconfiguration_iterator).second));
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        stream_name_string = mediafoundation_control_stream.name ();
        result_2 =
          mediafoundation_control_stream.initialize ((*mediafoundation_control_streamconfiguration_iterator).second);
        if (!result_2)
          break;
        stream_name_string = mediafoundation_mavlink_stream.name ();
        result_2 =
          mediafoundation_mavlink_stream.initialize ((*mediafoundation_mavlink_streamconfiguration_iterator).second);
        if (!result_2)
          break;
        stream_name_string =
          (useReactor_in ? mediafoundation_video_stream.name ()
                         : mediafoundation_asynch_video_stream.name ());
        result_2 =
          (useReactor_in ? mediafoundation_video_stream.initialize ((*mediafoundation_video_streamconfiguration_iterator).second)
                         : mediafoundation_asynch_video_stream.initialize ((*mediafoundation_video_streamconfiguration_iterator).second));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    CBData_in.mediaFramework));
        return;
      }
    } // end SWITCH
#else
    stream_name_string = control_stream.name ();
    result_2 =
      control_stream.initialize ((*control_streamconfiguration_iterator).second);
    if (!result_2)
      goto continue_;
    stream_name_string = mavlink_stream.name ();
    result_2 =
      mavlink_stream.initialize ((*mavlink_streamconfiguration_iterator).second);
    if (!result_2)
      goto continue_;
    stream_name_string = video_stream.name ();
    result_2 =
      video_stream.initialize ((*video_streamconfiguration_iterator).second);
continue_:
#endif
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize \"%s\" stream, returning\n"),
                  ACE_TEXT (stream_name_string.c_str ())));
      goto clean;
    } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (CBData_in.mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        stream_name_string = directshow_control_stream.name ();
        istream_control_p = &directshow_control_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        if (!result_2)
          break;
        stream_name_string = directshow_mavlink_stream.name ();
        istream_control_p = &directshow_mavlink_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        if (!result_2)
          break;
        stream_name_string = directshow_navdata_stream.name ();
        istream_control_p = &directshow_navdata_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        if (!result_2)
          break;
        stream_name_string =
          (useReactor_in ? directshow_video_stream.name ()
                         : directshow_asynch_video_stream.name ());
        if (useReactor_in)
          istream_control_p = &directshow_video_stream;
        else
          istream_control_p = &directshow_asynch_video_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        stream_name_string = mediafoundation_control_stream.name ();
        istream_control_p = &mediafoundation_control_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        if (!result_2)
          break;
        stream_name_string = mediafoundation_mavlink_stream.name ();
        istream_control_p = &mediafoundation_mavlink_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        if (!result_2)
          break;
        stream_name_string = mediafoundation_navdata_stream.name ();
        istream_control_p = &mediafoundation_navdata_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        if (!result_2)
          break;
        stream_name_string =
          (useReactor_in ? mediafoundation_video_stream.name ()
                         : mediafoundation_asynch_video_stream.name ());
        if (useReactor_in)
          istream_control_p = &mediafoundation_video_stream;
        else
          istream_control_p = &mediafoundation_asynch_video_stream;
        istream_control_p->start ();
        result_2 = istream_control_p->isRunning ();
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    CBData_in.mediaFramework));
        return;
      }
    } // end SWITCH
#else
    stream_name_string = control_stream.name ();
    istream_control_p = &control_stream;
    istream_control_p->start ();
    result_2 = istream_control_p->isRunning ();
    if (!result_2)
      goto continue_2;
    stream_name_string = mavlink_stream.name ();
    istream_control_p = &mavlink_stream;
    istream_control_p->start ();
    result_2 = istream_control_p->isRunning ();
    if (!result_2)
      goto continue_2;
    stream_name_string = navdata_stream.name ();
    istream_control_p = &navdata_stream;
    istream_control_p->start ();
    result_2 = istream_control_p->isRunning ();
    if (!result_2)
      goto continue_2;
    stream_name_string =
      (useReactor_in ? video_stream.name ()
                     : asynch_video_stream.name ());
    if (useReactor_in)
      istream_control_p = &video_stream;
    else
      istream_control_p = &asynch_video_stream;
    istream_control_p->start ();
    result_2 = istream_control_p->isRunning ();
continue_2:
#endif
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to start \"%s\" stream, returning\n")));
      goto clean;
    } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (CBData_in.mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        istream_control_p = &directshow_control_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        istream_control_p = &directshow_mavlink_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        istream_control_p = &directshow_navdata_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        if (useReactor_in)
          istream_control_p = &directshow_video_stream;
        else
          istream_control_p = &directshow_asynch_video_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        istream_control_p = &mediafoundation_control_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        istream_control_p = &mediafoundation_mavlink_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        istream_control_p = &mediafoundation_navdata_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        if (useReactor_in)
          istream_control_p = &mediafoundation_video_stream;
        else
          istream_control_p = &mediafoundation_asynch_video_stream;
        istream_control_p->wait (true,
                                 false,
                                 false);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    CBData_in.mediaFramework));
        return;
      }
    } // end SWITCH
#else
    istream_control_p = &control_stream;
    istream_control_p->wait (true,
                             false,
                             false);
    istream_control_p = &mavlink_stream;
    istream_control_p->wait (true,
                             false,
                             false);
    istream_control_p = &navdata_stream;
    istream_control_p->wait (true,
                             false,
                             false);
    istream_control_p = &video_stream;
    istream_control_p->wait (true,
                             false,
                             false);
#endif
  } // end IF
  else
    gtk_manager_p->wait ();

  //Common_Tools::dispatchEvents (useReactor_in,
  //                              group_id);

  // step9: clean up
//  result = event_handler_module.close ();
//  if (result == -1)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to ACE_Module::close(): \"%m\", continuing\n"),
//                event_handler_module.name ()));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working\n")));

  // clean up
  timer_manager_p->stop ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (CBData_in.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_connection_manager_p->stop ();
      directshow_connection_manager_p->abort (true); // wait for completion ?
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_connection_manager_p->stop ();
      mediafoundation_connection_manager_p->abort (true); // wait for completion ?
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  CBData_in.mediaFramework));
      return;
    }
  } // end SWITCH
#else
  connection_manager_p->stop ();
  connection_manager_p->abort (true); // wait for completion ?
#endif
  Common_Tools::finalizeEventDispatch (useReactor_in,
                                       !useReactor_in,
                                       group_id);

  return;

clean:
  if (WLAN_monitor_p)
    WLAN_monitor_p->stop (true,  // wait for completion ?
                          true); // locked access ?
  Common_Tools::finalizeEventDispatch (useReactor_in,
                                       !useReactor_in,
                                       group_id);
  if (timer_manager_p)
    timer_manager_p->stop ();
  if (!UIInterfaceDefinitionFile_in.empty ())
    ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::main"));

  int result;
  std::string configuration_path;
  std::string path;
  std::string interface_definition_file;
  std::string address_string;
  ACE_INET_Addr address;
  unsigned int buffer_size;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  bool show_console;
#endif
  bool debug_scanner;
  bool fullscreen;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _GUID interface_identifier;
#else
  std::string interface_identifier;
#endif
  bool log_to_file;
  unsigned short port_number;
  bool use_reactor;
  std::string SSID_string;
  bool trace_information;
  bool print_version_and_exit;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
  bool debug_nl80211;
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
  ACE_Profile_Timer process_profile;
  struct Stream_ModuleConfiguration module_configuration;
  struct ARDrone_StreamConfiguration stream_configuration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_ModuleHandlerConfiguration directshow_video_modulehandler_configuration;
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_video_modulehandlerconfiguration_iterator;
  ARDrone_DirectShow_StreamConfiguration_t directshow_stream_configuration_2;
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_video_streamconfiguration_iterator;

  struct ARDrone_MediaFoundation_ModuleHandlerConfiguration mediafoundation_video_modulehandler_configuration;
  ARDrone_MediaFoundation_StreamConfiguration_t mediafoundation_stream_configuration_2;
#else
  struct ARDrone_ModuleHandlerConfiguration video_modulehandler_configuration;
  ARDrone_StreamConfiguration_t stream_configuration_2;
#endif
  ACE_Sig_Set signal_set (0);
  ACE_Sig_Set ignored_signal_set (0);
  Common_SignalActions_t previous_signal_actions;
  sigset_t previous_signal_mask;
  struct ARDrone_GtkCBData gtk_cb_data;
  //Common_Logger_t logger (&gtk_cb_data.logStack,
  //                        &gtk_cb_data.lock);
  std::string log_file_name;
  ARDrone_GtkBuilderDefinition_t ui_definition (argc_in,
                                                argv_in);
  struct ARDrone_GtkProgressData gtk_progress_data;
  ARDrone_SignalHandler signal_handler ((NET_EVENT_USE_REACTOR ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                               : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                        &gtk_cb_data.lock);
  struct ARDrone_Configuration configuration;
  struct ARDrone_UserData user_data;
  ACE_High_Res_Timer timer;
  std::string working_time_string;
  ACE_Time_Value working_time;
  ACE_Time_Value user_time;
  ACE_Time_Value system_time;
  std::string user_time_string;
  std::string system_time_string;

  // step-2: initialize NLS
#ifdef ENABLE_NLS
#ifdef HAVE_LOCALE_H
  setlocale (LC_ALL, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // step-1: initialize ACE ?
  result = ACE::init ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

#if defined (ARDRONE_ENABLE_VALGRIND_SUPPORT)
  if (RUNNING_ON_VALGRIND)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("running on valgrind...\n")));
#endif

  result = -1;
  // set default values
  address_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_DEFAULT_IP_ADDRESS);
  if (address_string.find (':') != std::string::npos)
    result = address.set (address_string.c_str (), 0);
  else
    result = address.set (ARDRONE_PORT_TCP_VIDEO,
                          address_string.c_str (),
                          1,
                          0);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (address_string.c_str ())));
    goto error;
  } // end IF
  buffer_size = ARDRONE_MESSAGE_BUFFER_SIZE;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  show_console           = false;
#endif
  debug_scanner          = COMMON_PARSER_DEFAULT_LEX_TRACE;
  fullscreen             = ARDRONE_DEFAULT_VIDEO_FULLSCREEN;
  log_to_file            = false;
  interface_identifier   =
    Net_Common_Tools::getDefaultInterface (NET_LINKLAYER_802_11);
  port_number            = ARDRONE_PORT_TCP_VIDEO;
  use_reactor            = NET_EVENT_USE_REACTOR;
  SSID_string            = ACE_TEXT_ALWAYS_CHAR (ARDRONE_DEFAULT_WLAN_SSID);
  trace_information      = false;
  print_version_and_exit = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
  debug_nl80211          = false;
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

  // step0: process profile
  result = process_profile.start ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::start(): \"%m\", aborting\n")));
    goto error;
  } // end IF

  // step1: process commandline options, if any
  configuration_path = Common_File_Tools::getWorkingDirectory ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  bool use_mediafoundation   =
    (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK == STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION);
#endif
  result =
    address.set (static_cast<u_short> (ARDRONE_PORT_TCP_VIDEO),                // (TCP) port number
                 static_cast<ACE_UINT32> (192 << 24 | 168 << 16 | 1 << 8 | 1), // IPv4 address
                 1,                                                            // encode ?
                 0);                                                           // map to IPv6 ?
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(): \"%m\", aborting\n")));
    goto error;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("video port (UDP-)address: %s\n"),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (address).c_str ())));
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_DIRECTORY);
  interface_definition_file = path;
  interface_definition_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  interface_definition_file +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_DEFINITION_FILE_NAME);
  if (!do_processArguments (argc_in,
                            argv_in,
                            address,
                            buffer_size,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            show_console,
#endif
                            debug_scanner,
                            fullscreen,
                            interface_identifier,
                            log_to_file,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            use_mediafoundation,
#endif
                            port_number,
                            use_reactor,
                            SSID_string,
                            trace_information,
                            interface_definition_file,
                            print_version_and_exit
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
                            ,debug_nl80211
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
                            ))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to do_processArguments(), aborting\n")));

    // help the user, print usage instructions
    do_printUsage (ACE::basename (argv_in[0]));

    goto error;
  } // end IF

  // step2: validate configuration
  if ((!interface_definition_file.empty () &&
       !Common_File_Tools::isReadable (interface_definition_file)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid configuration, aborting\n")));

    // help the user, print usage instructions
    do_printUsage (ACE::basename (argv_in[0]));

    goto error;
  } // end IF

  // step3: run program ?
  if (print_version_and_exit)
  {
    do_printVersion (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                          ACE_DIRECTORY_SEPARATOR_CHAR)));
    goto done;
  } // end IF

  // step4: initialize logging and/or tracing
  if (log_to_file)
    log_file_name =
      Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE),
                                         ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0],
                                                                              ACE_DIRECTORY_SEPARATOR_CHAR)));
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]),                      // program name
                                        log_file_name,                                   // log file name
                                        false,                                           // log to syslog ?
                                        false,                                           // trace messages ?
                                        trace_information,                               // debug messages ?
                                        NULL))
                                        //(interface_definition_file.empty () ? NULL
                                        //                                    : &logger))) // logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));
    goto error;
  } // end IF

  // step5: (pre-)initialize signal handling
  do_initializeSignals (use_reactor,
                        true,
                        signal_set,
                        ignored_signal_set);
  result = ACE_OS::sigemptyset (&previous_signal_mask);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::sigemptyset(): \"%m\", aborting\n")));

    // clean up
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF
  if (!Common_Signal_Tools::preInitialize (signal_set,
                                           use_reactor,
                                           previous_signal_actions,
                                           previous_signal_mask))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::preInitializeSignals(), aborting\n")));

    // clean up
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF

  // step6: initialize configuration
  configuration.userData = &user_data;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (gtk_cb_data.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_video_modulehandler_configuration.filterConfiguration =
        &configuration.directShowFilterConfiguration;
      directshow_video_modulehandler_configuration.fullScreen = fullscreen;

      directshow_stream_configuration_2.initialize (module_configuration,
                                                    directshow_video_modulehandler_configuration,
                                                    configuration.allocatorConfiguration,
                                                    stream_configuration);
      configuration.directShowStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
                                                                           directshow_stream_configuration_2));
      directshow_video_streamconfiguration_iterator =
        configuration.directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_video_streamconfiguration_iterator != configuration.directShowStreamConfigurations.end ());
      directshow_video_modulehandlerconfiguration_iterator =
        (*directshow_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_video_modulehandlerconfiguration_iterator != (*directshow_video_streamconfiguration_iterator).second.end ());

      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_video_modulehandler_configuration.fullScreen = fullscreen;

      mediafoundation_stream_configuration_2.initialize (module_configuration,
                                                         mediafoundation_video_modulehandler_configuration,
                                                         configuration.allocatorConfiguration,
                                                         stream_configuration);
      configuration.mediaFoundationStreamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
                                                                                mediafoundation_stream_configuration_2));

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  gtk_cb_data.mediaFramework));

      // clean up
      Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                  : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                     signal_set,
                                     previous_signal_actions,
                                     previous_signal_mask);
      Common_Tools::finalizeLogging ();

      goto error;
    }
  } // end SWITCH
#else
  video_modulehandler_configuration.fullScreen = fullscreen;
  video_modulehandler_configuration.pixelBufferLock = &gtk_cb_data.lock;

  stream_configuration_2.initialize (module_configuration,
                                     video_modulehandler_configuration,
                                     configuration.allocatorConfiguration,
                                     stream_configuration);
  configuration.streamConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING),
                                                             stream_configuration_2));
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  configuration.WLANMonitorConfiguration.interfaceIdentifier =
      interface_identifier;
#endif

  // step7: initialize user interface, if any
  gtk_cb_data.argc = argc_in;
  gtk_cb_data.argv = argv_in;
  gtk_cb_data.configuration = &configuration;
  //ACE_OS::memset (&gtk_cb_data.clientSensorBias,
  //                0,
  //                sizeof (gtk_cb_data.clientSensorBias));
#if defined (GTKGL_SUPPORT)
//  gtk_cb_data.openGLDoubleBuffered = ARDRONE_OPENGL_DOUBLE_BUFFERED;
#endif
  //ACE_OS::memset (gtk_cb_data.temperature,
  //                0,
  //                sizeof (gtk_cb_data.temperature));
  gtk_cb_data.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (interface_definition_file, static_cast<GtkBuilder*> (NULL));
  gtk_cb_data.finalizationHook = idle_finalize_ui_cb;
  gtk_cb_data.initializationHook = idle_initialize_ui_cb;
  gtk_cb_data.progressData = &gtk_progress_data;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  gtk_cb_data.mediaFramework =
    (use_mediafoundation ? STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION
                         : STREAM_MEDIAFRAMEWORK_DIRECTSHOW);
#endif
  gtk_cb_data.userData = &gtk_cb_data;
  gtk_progress_data.state = &gtk_cb_data;
  ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                             argv_in,
                                                             &gtk_cb_data,
                                                             &ui_definition);

  // step8: media framework, if any
  Stream_Module_Decoder_Tools::initialize ();
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_Module_Device_Tools::initialize ();
  bool result_2 = false;
  switch (gtk_cb_data.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      result_2 =
        do_initialize_directshow ((*directshow_video_modulehandlerconfiguration_iterator).second.second.graphBuilder,
                                  configuration.directShowPinConfiguration.format,
                                  true,
                                  directshow_video_modulehandler_configuration.fullScreen);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      do_initialize_mediafoundation (true);

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  gtk_cb_data.mediaFramework));

      // clean up
      Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                  : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                     signal_set,
                                     previous_signal_actions,
                                     previous_signal_mask);
      Common_Tools::finalizeLogging ();

      goto error;
    }
  } // end SWITCH
  if (!result_2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize media framework, returning\n")));

    // clean up
    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF
#endif

  // step9: run program
  timer.start ();
  do_work (argc_in,
           argv_in,
           address,
           buffer_size,
           debug_scanner,
           fullscreen,
           interface_identifier,
           use_reactor,
           SSID_string,
           interface_definition_file,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (NL80211_SUPPORT)
#if defined (_DEBUG)
           debug_nl80211,
#endif // _DEBUG
#endif // NL80211_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
           gtk_cb_data,
           signal_set,
           ignored_signal_set,
           previous_signal_actions,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           show_console,
#endif
           signal_handler);
  timer.stop ();

  // debug info
  timer.elapsed_time (working_time);
  Common_Timer_Tools::periodToString (working_time,
                                      working_time_string);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

done:
  process_profile.stop ();

  // debug info
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  result = process_profile.elapsed_time (elapsed_time);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    // clean up
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (use_mediafoundation)
      do_finalize_mediafoundation ();
    else
      do_finalize_directshow ((*directshow_video_modulehandlerconfiguration_iterator).second.second.graphBuilder,
                              (*directshow_video_modulehandlerconfiguration_iterator).second.second.inputFormat);
#endif
    Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                                : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                   signal_set,
                                   previous_signal_actions,
                                   previous_signal_mask);
    Common_Tools::finalizeLogging ();

    goto error;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  user_time.set (elapsed_rusage.ru_utime);
  system_time.set (elapsed_rusage.ru_stime);
  Common_Timer_Tools::periodToString (user_time,
                                      user_time_string);
  Common_Timer_Tools::periodToString (system_time,
                                      system_time_string);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
              elapsed_time.real_time,
              elapsed_time.user_time,
              elapsed_time.system_time,
              user_time_string.c_str (),
              system_time_string.c_str (),
              elapsed_rusage.ru_maxrss,
              elapsed_rusage.ru_ixrss,
              elapsed_rusage.ru_idrss,
              elapsed_rusage.ru_isrss,
              elapsed_rusage.ru_minflt,
              elapsed_rusage.ru_majflt,
              elapsed_rusage.ru_nswap,
              elapsed_rusage.ru_inblock,
              elapsed_rusage.ru_oublock,
              elapsed_rusage.ru_msgsnd,
              elapsed_rusage.ru_msgrcv,
              elapsed_rusage.ru_nsignals,
              elapsed_rusage.ru_nvcsw,
              elapsed_rusage.ru_nivcsw));
#endif

  // step10: clean up
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (use_mediafoundation)
    do_finalize_mediafoundation ();
  else
    do_finalize_directshow ((*directshow_video_modulehandlerconfiguration_iterator).second.second.graphBuilder,
                            (*directshow_video_modulehandlerconfiguration_iterator).second.second.inputFormat);
#endif
  Common_Signal_Tools::finalize ((use_reactor ? COMMON_SIGNAL_DISPATCH_REACTOR
                                              : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                 signal_set,
                                 previous_signal_actions,
                                 previous_signal_mask);
  Common_Tools::finalizeLogging ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;

error:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_FAILURE;
} // end main
