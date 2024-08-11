/***************************************************************************
*   Copyright (C) 2009 by Erik Sohns   *
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

#include <iostream>
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "initguid.h" // *NOTE*: this exports DEFINE_GUIDs (see stream_misc_common.h)
#include "mfapi.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Synch.h"
#include "ace/Version.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_tools.h"

#include "common_error_defines.h"
#include "common_error_tools.h"

#include "common_event_tools.h"

#include "common_log_tools.h"
//#include "common_logger.h"

#include "common_signal_tools.h"

#include "common_timer_manager_common.h"
#include "common_timer_tools.h"

#if defined (HAVE_CONFIG_H)
#include "ACEStream_config.h"
#endif // HAVE_CONFIG_H

#include "stream_allocatorheap.h"
#include "stream_control_message.h"
#include "stream_macros.h"

#include "stream_dec_defines.h"

#include "stream_lib_tools.h"

#include "stream_misc_defines.h"

#include "stream_vis_tools.h"

//#include "test_u_common.h"
//#include "test_u_defines.h"

#include "test_u_video_ui_defines.h"
#include "test_u_video_ui_eventhandler.h"
#include "test_u_video_ui_session_message.h"
#include "test_u_video_ui_signalhandler.h"
#include "test_u_video_ui_stream.h"

const char stream_name_string_[] = ACE_TEXT_ALWAYS_CHAR ("Test_U_Stream");

void
do_print_usage (const std::string& programName_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_print_usage"));

  // enable verbatim boolean output
  std::cout.setf (std::ios::boolalpha);

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-2          : use Direct2D renderer [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-3          : use Direct3D renderer [")
            << true
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c          : show console [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d          : enable debug heap [")
            << COMMON_ERROR_WIN32_DEBUGHEAP_DEFAULT_ENABLE
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
#else
//  std::cout << ACE_TEXT_ALWAYS_CHAR ("-1          : use X11 renderer [")
//            << (STREAM_VIS_RENDERER_VIDEO_DEFAULT == STREAM_VISUALIZATION_VIDEORENDERER_X11)
//            << ACE_TEXT_ALWAYS_CHAR ("])")
//            << std::endl;
#endif // ACE_WIN32 || ACE_WIN64
  std::string path = Common_File_Tools::getTempDirectory ();
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m          : use MediaFoundation framework [")
            << (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK == STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
#endif // ACE_WIN32 || ACE_WIN64
  struct Common_UI_DisplayDevice display_device_s =
    Common_UI_Tools::getDefaultDisplay ();
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o [STRING] : display device [\"")
            << display_device_s.description
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r          : use reactor [")
            << (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t          : trace information [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v          : print version information and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x          : test device for method support and exit [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                      bool& showConsole_out,
                      bool& enableDebugHeap_out,
#endif // ACE_WIN32 || ACE_WIN64
                      bool& logToFile_out,
                      struct Common_UI_DisplayDevice& displayDevice_out,
                      enum Stream_Visualization_VideoRenderer& renderer_out,
                      bool& useReactor_out,
                      bool& traceInformation_out,
                      enum Test_U_ProgramMode& mode_out)
{
  STREAM_TRACE (ACE_TEXT ("::do_process_arguments"));

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // initialize results
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  showConsole_out = false;
  enableDebugHeap_out = COMMON_ERROR_WIN32_DEBUGHEAP_DEFAULT_ENABLE;
#endif // ACE_WIN32 || ACE_WIN64
  logToFile_out = false;
  displayDevice_out = Common_UI_Tools::getDefaultDisplay ();
  renderer_out = STREAM_VISUALIZATION_VIDEORENDERER_GTK_WINDOW;
  useReactor_out =
    (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR);
  traceInformation_out = false;
  mode_out = TEST_U_PROGRAMMODE_NORMAL;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              ACE_TEXT ("23cdlo:rtv"),
#else
                              ACE_TEXT ("1lo:rtv"),
#endif // ACE_WIN32 || ACE_WIN64
                              1,                          // skip command name
                              1,                          // report parsing errors
                              ACE_Get_Opt::PERMUTE_ARGS,  // ordering
                              0);                         // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      case '2':
      {
        renderer_out = STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D;
        break;
      }
      case '3':
      {
        renderer_out = STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D;
        break;
      }
      case 'c':
      {
        showConsole_out = true;
        break;
      }
      case 'd':
      {
        enableDebugHeap_out = true;
        break;
      }
#else
      case '1':
      {
        renderer_out = STREAM_VISUALIZATION_VIDEORENDERER_X11;
        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'o':
      {
        displayDevice_out =
          Common_UI_Tools::getDisplay (ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ()));
        break;
      }
      case 'r':
      {
        useReactor_out = true;
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'v':
      {
        mode_out = TEST_U_PROGRAMMODE_PRINT_VERSION;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argumentParser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argumentParser.last_option ()));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    argumentParser.long_option ()));
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
do_initializeSignals (bool allowUserRuntimeConnect_in,
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

  // *PORTABILITY*: on Windows(TM) platforms most signals are not defined, and
  //                ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_out.sig_add (SIGINT);            // 2       /* interrupt */
  signals_out.sig_add (SIGILL);            // 4       /* illegal instruction - invalid function image */
  signals_out.sig_add (SIGFPE);            // 8       /* floating point exception */
  //  signals_out.sig_add (SIGSEGV);           // 11      /* segment violation */
  signals_out.sig_add (SIGTERM);           // 15      /* Software termination signal from kill */
  if (allowUserRuntimeConnect_in)
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
#if defined (DEBUG_DEBUGGER)
  signals_out.sig_del (SIGTRAP);           // 5       /* Trace trap (POSIX) */
#endif
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_out.sig_del (SIGKILL);           // 9       /* Kill signal */
  signals_out.sig_del (SIGSTOP);           // 19      /* Stop process */
  // ---------------------------------------------------------------------------
  if (!allowUserRuntimeConnect_in)
  {
    signals_out.sig_del (SIGUSR1);         // 10      /* User-defined signal 1 */
    ignoredSignals_out.sig_add (SIGUSR1);  // 10      /* User-defined signal 1 */
  } // end IF
  // *NOTE* core dump on SIGSEGV
  signals_out.sig_del (SIGSEGV);           // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_out.sig_del (SIGPIPE);           // 12      /* Broken pipe: write to pipe with no readers */

#ifdef ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_out.sig_del (SIGRTMAX);        // 64
#endif
#endif
}

void
do_work (bool useReactor_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         bool showConsole_in,
#endif // ACE_WIN32 || ACE_WIN64
         const struct Common_UI_DisplayDevice& displayDevice_in,
         struct Test_U_VideoUI_Configuration& configuration_in,
         enum Stream_Visualization_VideoRenderer renderer_in)
{
  STREAM_TRACE (ACE_TEXT ("::do_work"));

  // ********************** module configuration data **************************
  struct Stream_ModuleConfiguration module_configuration;
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration;
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration_io;
  struct Test_U_StreamConfiguration stream_configuration;
  Test_U_TCPConnectionConfiguration_t connection_configuration;
  Test_U_TCPConnectionManager_t* connection_manager_p = NULL;
  Test_U_EventHandler_t ui_event_handler;

  Test_U_StreamConfiguration_t::ITERATOR_T v4l_stream_iterator;
  Test_U_StreamConfiguration_t::ITERATOR_T v4l_stream_iterator_2;
  modulehandler_configuration.allocatorConfiguration =
    &configuration_in.allocatorConfiguration;
  modulehandler_configuration.connectionConfigurations =
      &configuration_in.connectionConfigurations;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  modulehandler_configuration.direct3DConfiguration =
    &configuration_in.direct3DConfiguration;
#endif // ACE_WIN32 || ACE_WIN64
  //  // *TODO*: turn these into an option
//  modulehandler_configuration.method = STREAM_DEV_CAM_V4L_DEFAULT_IO_METHOD;
  modulehandler_configuration.subscriber = &ui_event_handler;

  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Stream_AllocatorConfiguration> heap_allocator;
  Test_U_MessageAllocator_t message_allocator (0,               // maximum #buffers --> no limits
                                               &heap_allocator, // heap allocator handle
                                               true);           // block ?
  Test_U_Stream stream;
  Test_U_MessageHandler_Module message_handler (&stream,
                                                             ACE_TEXT_ALWAYS_CHAR (STREAM_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));

  stream_configuration.messageAllocator = &message_allocator;
  stream_configuration.module = &message_handler;
  // stream_configuration.renderer = renderer_in;

  if (!heap_allocator.initialize (configuration_in.allocatorConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize heap allocator, returning\n")));
    return;
  } // end IF

#if defined (FFMPEG_SUPPORT)
  struct Stream_MediaFramework_FFMPEG_CodecConfiguration codec_configuration;
  codec_configuration.codecId = AV_CODEC_ID_H264;
  codec_configuration.padInputBuffers = false; // data arrives fragmented !
  modulehandler_configuration.codecConfiguration = &codec_configuration;
#endif // FFMPEG_SUPPORT

  modulehandler_configuration.cascadeFile =
      Common_File_Tools::getWorkingDirectory ();
  modulehandler_configuration.cascadeFile += ACE_DIRECTORY_SEPARATOR_STR_A;
  modulehandler_configuration.cascadeFile +=
      ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  modulehandler_configuration.cascadeFile += ACE_DIRECTORY_SEPARATOR_STR_A;
  modulehandler_configuration.cascadeFile +=
    ACE_TEXT_ALWAYS_CHAR ("haarcascade_frontalface_default.xml");
  modulehandler_configuration.concurrency = STREAM_HEADMODULECONCURRENCY_ACTIVE;
  // *IMPORTANT NOTE*: is there a way to feed RGB24 data to Xlib;
  //                   XCreateImage() only 'likes' 32-bit data, regardless of
  //                   what 'depth' values are set (in fact, it requires BGRA on
  //                   little-endian platforms) --> convert
  modulehandler_configuration.outputFormat.format = AV_PIX_FMT_RGB32;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  modulehandler_configuration.outputFormat.resolution.cx = 640;
  modulehandler_configuration.outputFormat.resolution.cy = 480;
#else
  modulehandler_configuration.outputFormat.resolution.width = 640;
  modulehandler_configuration.outputFormat.resolution.height = 480;
#endif // ACE_WIN32 || ACE_WIN64
  modulehandler_configuration.outputFormat.frameRate.num = 30;

  modulehandler_configuration_io = modulehandler_configuration;
  modulehandler_configuration_io.concurrency =
    STREAM_HEADMODULECONCURRENCY_CONCURRENT;

  stream_configuration.format = modulehandler_configuration.outputFormat;

//  modulehandler_configuration.display = displayDevice_in;
  configuration_in.streamConfiguration.initialize (module_configuration,
                                                   modulehandler_configuration,
                                                   stream_configuration);
  configuration_in.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_CONVERTER_DEFAULT_NAME_STRING),
                                                               std::make_pair (&module_configuration,
                                                                               &modulehandler_configuration)));
  configuration_in.streamConfiguration.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_IO_DEFAULT_NAME_STRING),
                                                               std::make_pair (&module_configuration,
                                                                               &modulehandler_configuration_io)));

  v4l_stream_iterator =
    configuration_in.streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (v4l_stream_iterator != configuration_in.streamConfiguration.end ());

  // connection configuration
  connection_manager_p = TEST_U_TCP_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (connection_manager_p);
  connection_manager_p->initialize (std::numeric_limits<unsigned int>::max (),
                                    ACE_Time_Value (0, NET_STATISTIC_DEFAULT_VISIT_INTERVAL_MS * 1000));
//  struct Net_UserData net_user_data;

  connection_configuration.allocatorConfiguration =
    &configuration_in.allocatorConfiguration;
//  connection_configuration.generateUniqueIOModuleNames = true;
  connection_configuration.messageAllocator = &message_allocator;
//  connection_configuration.PDUSize =
//    std::max (bufferSize_in,
//              static_cast<unsigned int> (ARDRONE_MESSAGE_BUFFER_SIZE));
  connection_configuration.socketConfiguration.address =
      ACE_INET_Addr (ACE_TEXT_ALWAYS_CHAR ("192.168.1.1:0"), AF_INET);
  connection_configuration.socketConfiguration.address.set_port_number (ARDRONE_PORT_TCP_VIDEO,
                                                                        1);
  connection_configuration.socketConfiguration.bufferSize =
    NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
  connection_configuration.statisticReportingInterval =
    ACE_Time_Value (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL_S, 0);

  configuration_in.streamConfiguration.configuration_->module = NULL;
  connection_configuration.streamConfiguration = &configuration_in.streamConfiguration;

//  configuration_in.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
  configuration_in.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                    &connection_configuration));
  Net_ConnectionConfigurationsIterator_t connection_configurations_iterator =
      configuration_in.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (connection_configurations_iterator != configuration_in.connectionConfigurations.end ());
  struct Net_UserData user_data_s;
  connection_manager_p->set (connection_configuration,
                             &user_data_s); // passed to all handlers

  struct Common_TimerConfiguration timer_configuration;
  Common_Timer_Manager_t* timer_manager_p = NULL;

  // intialize timers
  timer_manager_p = COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start (NULL);

  struct Common_EventDispatchState dispatch_state_s;
  dispatch_state_s.configuration = &configuration_in.dispatchConfiguration;
  if (!Common_Event_Tools::startEventDispatch (dispatch_state_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, returning\n")));
    return;
  } // end IF

  // step0f: (initialize) processing stream

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  Stream_IStreamControlBase* stream_p = NULL;
  if (!stream.initialize (configuration_in.streamConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize stream, returning\n")));
    return;
  } // end IF
  stream_p = &stream;
  ACE_ASSERT (stream_p);
    // *NOTE*: this will block until the file has been copied...
  stream_p->start ();
//    if (!stream_p->isRunning ())
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to start stream, aborting\n")));
//      //timer_manager_p->stop ();
//      return;
//    } // end IF

  connection_manager_p->wait (true); // N/A

  stream_p->wait (true,   // wait for thread(s) ?
                  true,   // wait for upstream ?
                  false); // wait for downstream ?

  // step3: clean up
  timer_manager_p->stop ();

  connection_manager_p->stop (false); // wait for completion ?
  connection_manager_p->abort (true); // wait for completion ?
  Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                             true,   // wait ?
                                             false); // release singleton pro/reactors ?

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

COMMON_DEFINE_PRINTVERSION_FUNCTION(do_print_version,STREAM_MAKE_VERSION_STRING_VARIABLE(programName_in,ACE_TEXT_ALWAYS_CHAR (ACEStream_PACKAGE_VERSION_FULL),version_string),version_string)

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  STREAM_TRACE (ACE_TEXT ("::main"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  int result = -1;
#endif // ACE_WIN32 || ACE_WIN64

  // step0: initialize
  // *PORTABILITY*: on Windows, initialize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::init ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start ();

  // initialize framework(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::initialize (false,  // initialize COM ?
                            false); // initialize random number generator ?
#else
  Common_Tools::initialize (false); // initialize random number generator ?
#endif // ACE_WIN32 || ACE_WIN64
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_MediaFramework_Tools::initialize (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK);
  Stream_MediaFramework_DirectDraw_Tools::initialize ();
#endif // ACE_WIN32 || ACE_WIN64

  std::string configuration_path = Common_File_Tools::getWorkingDirectory ();

  // step1a set defaults
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  bool show_console = false;
  bool enable_debug_heap_b = COMMON_ERROR_WIN32_DEBUGHEAP_DEFAULT_ENABLE;
#endif // ACE_WIN32 || ACE_WIN64
  bool log_to_file = false;
  enum Stream_Visualization_VideoRenderer video_renderer_e =
      STREAM_VISUALIZATION_VIDEORENDERER_GTK_WINDOW;
  struct Common_UI_DisplayDevice display_device_s =
    Common_UI_Tools::getDefaultDisplay ();
  bool trace_information = false;
  enum Test_U_ProgramMode program_mode_e =
      TEST_U_PROGRAMMODE_NORMAL;
  bool use_reactor_b =
      COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR;

  // step1b: parse/process/validate configuration
  if (!do_process_arguments (argc_in,
                             argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                             show_console,
                             enable_debug_heap_b,
#endif // ACE_WIN32 || ACE_WIN64
                             log_to_file,
                             display_device_s,
                             video_renderer_e,
                             use_reactor_b,
                             trace_information,
                             program_mode_e))
  {
    do_print_usage (ACE::basename (argv_in[0]));
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Log_Tools::packageName = ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME);
  Common_Error_Tools::initialize (enable_debug_heap_b);
#else
  Common_Error_Tools::initialize ();
#endif // ACE_WIN32 || ACE_WIN64

  // step1c: validate arguments
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (0)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could (!) lead to a deadlock --> ensure the streaming elements are sufficiently efficient in this regard\n")));
  if (false)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid arguments, aborting\n")));

    do_print_usage (ACE::basename (argv_in[0]));
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF

  // step1d: initialize logging and/or tracing
  std::string log_file_name;
  if (log_to_file)
    log_file_name =
        Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                          ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR_A)));
  if (!Common_Log_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR_A)), // program name
                                     log_file_name,                                // log file name
                                     false,                                        // log to syslog ?
                                     false,                                        // trace messages ?
                                     trace_information,                            // debug messages ?
                                     NULL))                                        // (ui) logger ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initialize(), aborting\n")));

    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF

  // step1f: handle specific program modes
  switch (program_mode_e)
  {
    case TEST_U_PROGRAMMODE_PRINT_VERSION:
    {
      do_print_version (ACE::basename (argv_in[0]));

      Common_Log_Tools::finalize ();
      // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      result = ACE::fini ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
      return EXIT_SUCCESS;
    }
    case TEST_U_PROGRAMMODE_NORMAL:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown program mode (was: %d), aborting\n"),
                  program_mode_e));

      Common_Log_Tools::finalize ();
      // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      result = ACE::fini ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
      return EXIT_FAILURE;
    }
  } // end SWITCH

  // step1e: pre-initialize signal handling
  ACE_Sig_Set signal_set (false);
  ACE_Sig_Set ignored_signal_set (false);
  do_initializeSignals (false, // handle SIGUSR1/SIGBREAK ?
                        signal_set,
                        ignored_signal_set);
  Common_SignalActions_t previous_signal_actions;
  ACE_Sig_Set previous_signal_mask (false);
  if (!Common_Signal_Tools::preInitialize (signal_set,
                                           (use_reactor_b ? COMMON_SIGNAL_DISPATCH_REACTOR : COMMON_SIGNAL_DISPATCH_PROACTOR),
                                           true,
                                           false,
                                           previous_signal_actions,
                                           previous_signal_mask))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Signal_Tools::preInitialize(), aborting\n")));

    Common_Log_Tools::finalize ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
  ACE_SYNCH_RECURSIVE_MUTEX* lock_2 = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  Test_U_SignalHandler_t signal_handler;
#endif // ACE_WIN32 || ACE_WIN64

  struct Test_U_VideoUI_Configuration configuration;

  // event dispatch
  if (use_reactor_b)
  {
    configuration.dispatchConfiguration.dispatch =
      COMMON_EVENT_DISPATCH_REACTOR;
    configuration.dispatchConfiguration.numberOfReactorThreads =
      NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS;
    configuration.dispatchConfiguration.reactorType =
      (configuration.dispatchConfiguration.numberOfReactorThreads > 1) ? COMMON_REACTOR_THREAD_POOL : COMMON_REACTOR_ACE_DEFAULT;
  } // end IF
  else
    configuration.dispatchConfiguration.numberOfProactorThreads =
      NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS;
  if (!Common_Event_Tools::initializeEventDispatch (configuration.dispatchConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));

    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
//  if (video_renderer_e == STREAM_VISUALIZATION_VIDEORENDERER_GTK_WINDOW)
//    Common_UI_GTK_Tools::initialize (argc_in, argv_in);

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (use_reactor_b,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           show_console,
#endif // ACE_WIN32 || ACE_WIN64
           display_device_s,
           configuration,
           video_renderer_e);
  timer.stop ();

  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  working_time_string = Common_Timer_Tools::periodToString (working_time);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // stop profile timer...
  process_profile.stop ();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time (elapsed_time) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    Common_Log_Tools::finalize ();
    // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    result = ACE::fini ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset (&elapsed_rusage, 0, sizeof (elapsed_rusage));
  process_profile.elapsed_rusage (elapsed_rusage);
  ACE_Time_Value user_time (elapsed_rusage.ru_utime);
  ACE_Time_Value system_time (elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  user_time_string = Common_Timer_Tools::periodToString (user_time);
  system_time_string = Common_Timer_Tools::periodToString (system_time);

  // debug info
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
              ACE_TEXT (user_time_string.c_str ()),
              ACE_TEXT (system_time_string.c_str ()),
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
#endif // ACE_WIN32 || ACE_WIN64

  Common_Log_Tools::finalize ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Stream_MediaFramework_DirectDraw_Tools::finalize ();
  Stream_MediaFramework_Tools::finalize ();
#endif // ACE_WIN32 || ACE_WIN64
  Common_Error_Tools::finalize ();
  Common_Tools::finalize ();

  // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
