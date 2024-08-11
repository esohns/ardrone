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
#include "ace/OS.h"
#include "ace/Profile_Timer.h"
#include "ace/Sig_Handler.h"
#include "ace/Signal.h"
#include "ace/Version.h"

#if defined (HAVE_CONFIG_H)
#include "Common_config.h"
#endif // HAVE_CONFIG_H

#include "common_tools.h"
#include "common_file_tools.h"

#include "common_log_tools.h"
//#include "common_logger.h"

#include "common_signal_tools.h"

#include "common_timer_manager_common.h"
#include "common_timer_tools.h"

#include "common_ui_tools.h"

#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager_common.h"

#if defined (HAVE_CONFIG_H)
#include "ACEStream_config.h"
#endif // HAVE_CONFIG_H

#include "stream_allocatorheap.h"
#include "stream_control_message.h"
#include "stream_macros.h"

#include "stream_dev_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_mediafoundation_tools.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "stream_dev_tools.h"

#include "stream_lib_tools.h"

#include "stream_misc_defines.h"

#include "stream_vis_tools.h"

#include "test_u_defines.h"
#include "test_u_eventhandler.h"
#include "test_u_gtk_callbacks.h"
#include "test_u_session_message.h"
#include "test_u_stream.h"

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
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c          : show console [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("])")
            << std::endl;
#endif // ACE_WIN32 || ACE_WIN64
  std::string UI_file = configuration_path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_DEFINITION_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g[[STRING]]: UI file [\"")
            << UI_file
            << ACE_TEXT_ALWAYS_CHAR ("\"] {\"\" --> no GUI}")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-l          : log to a file [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
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
  //std::cout << ACE_TEXT_ALWAYS_CHAR ("-y          : run stress-test [")
  //  << false
  //  << ACE_TEXT_ALWAYS_CHAR ("]")
  //  << std::endl;
}

bool
do_process_arguments (int argc_in,
                      ACE_TCHAR** argv_in, // cannot be const...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                      bool& showConsole_out,
#endif // ACE_WIN32 || ACE_WIN64
                      std::string& UIDefinition_out,
                      bool& logToFile_out,
                      struct Common_UI_DisplayDevice& displayDevice_out,
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
#endif // ACE_WIN32 || ACE_WIN64
  UIDefinition_out = configuration_path;
  UIDefinition_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIDefinition_out += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
  UIDefinition_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIDefinition_out += ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_DEFINITION_FILE);
  logToFile_out = false;
  displayDevice_out = Common_UI_Tools::getDefaultDisplay ();
  useReactor_out =
    (COMMON_EVENT_DEFAULT_DISPATCH == COMMON_EVENT_DISPATCH_REACTOR);
  traceInformation_out = false;
  mode_out = TEST_U_PROGRAMMODE_NORMAL;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              ACE_TEXT ("cg::lo:rtv"),
#else
                              ACE_TEXT ("g::lo:rtv"),
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
      case 'c':
      {
        showConsole_out = true;
        break;
      }
#endif // ACE_WIN32 || ACE_WIN64
      case 'g':
      {
        ACE_TCHAR* opt_arg = argumentParser.opt_arg ();
        if (opt_arg)
          UIDefinition_out = ACE_TEXT_ALWAYS_CHAR (opt_arg);
        else
          UIDefinition_out.clear ();
        break;
      }
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
do_work (bool useReactor_in,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
         bool showConsole_in,
#endif // ACE_WIN32 || ACE_WIN64
         const struct Common_UI_DisplayDevice& displayDevice_in,
         struct Test_U_Configuration& configuration_in
#if defined (GUI_SUPPORT)
         ,struct Test_U_UI_CBData& CBData_in
#endif // GUI_SUPPORT
        )
{
  STREAM_TRACE (ACE_TEXT ("::do_work"));

  // ********************** module configuration data **************************
  struct Stream_ModuleConfiguration module_configuration;
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration;
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration_2;
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration_io; // NetIO
  struct Test_U_ModuleHandlerConfiguration modulehandler_configuration_source; // NetSource
  struct Test_U_StreamConfiguration stream_configuration;
  struct Test_U_StreamConfiguration stream_configuration_2;
  struct Test_U_StreamConfiguration stream_configuration_net;
  Test_U_TCPConnectionConfiguration_t tcp_connection_configuration;
  Test_U_UDPConnectionConfiguration_t udp_connection_configuration, udp_connection_configuration_2;
  Test_U_TCPConnectionManager_t* tcp_connection_manager_p = NULL;
  Test_U_UDPConnectionManager_t* udp_connection_manager_p = NULL;
#if defined (GUI_SUPPORT)
  Test_U_EventHandler_t ui_event_handler (&CBData_in);
#else
  Test_U_EventHandler_t ui_event_handler;
#endif // GUI_SUPPORT

  modulehandler_configuration.allocatorConfiguration =
    &configuration_in.allocatorConfiguration;
  modulehandler_configuration.concurrency =
      STREAM_HEADMODULECONCURRENCY_ACTIVE;
  modulehandler_configuration.connectionConfigurations =
    &configuration_in.connectionConfigurations;
  modulehandler_configuration.parserConfiguration =
    &configuration_in.parserConfiguration;
  modulehandler_configuration.subscriber = &ui_event_handler;

  modulehandler_configuration_2 = modulehandler_configuration;
  modulehandler_configuration_2.connectionConfigurations =
      &configuration_in.connectionConfigurations_2;
  modulehandler_configuration_2.parserConfiguration =
    &configuration_in.parserConfiguration_2;

  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Stream_AllocatorConfiguration> heap_allocator;
  Test_U_MessageAllocator_t message_allocator (0,               // maximum #buffers --> no limits
                                               &heap_allocator, // heap allocator handle
                                               true);           // block ?
  Test_U_Stream stream;
  Test_U_ControlStream control_stream;
  Test_U_MessageHandler_Module message_handler (&stream,
                                                ACE_TEXT_ALWAYS_CHAR (STREAM_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING));

  stream_configuration.dispatchConfiguration =
    &configuration_in.dispatchConfiguration;
  stream_configuration.messageAllocator = &message_allocator;
  stream_configuration.module = &message_handler;
  stream_configuration.initializeNavData = &ui_event_handler;
  stream_configuration.CBData = &CBData_in;

  if (!heap_allocator.initialize (configuration_in.allocatorConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize heap allocator, returning\n")));
    return;
  } // end IF

  modulehandler_configuration.CBData = &CBData_in;
//  modulehandler_configuration.display = displayDevice_in;
  configuration_in.streamConfiguration.initialize (module_configuration,
                                                   modulehandler_configuration,
                                                   stream_configuration);

  stream_configuration_2 = stream_configuration;
  configuration_in.streamConfiguration_2.initialize (module_configuration,
                                                     modulehandler_configuration_2,
                                                     stream_configuration_2);

  modulehandler_configuration_io = modulehandler_configuration;
  modulehandler_configuration_io.concurrency =
      STREAM_HEADMODULECONCURRENCY_CONCURRENT;
  stream_configuration_net = stream_configuration;
  stream_configuration_net.module = NULL;
  configuration_in.streamConfiguration_net.initialize (module_configuration,
                                                       modulehandler_configuration_io,
                                                       stream_configuration_net);

  // connection configuration
  tcp_connection_manager_p =
      TEST_U_TCP_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (tcp_connection_manager_p);
  tcp_connection_manager_p->initialize (std::numeric_limits<unsigned int>::max (),
                                        ACE_Time_Value (0, NET_STATISTIC_DEFAULT_VISIT_INTERVAL_MS * 1000));
  udp_connection_manager_p =
      TEST_U_UDP_CONNECTIONMANAGER_SINGLETON::instance ();
  ACE_ASSERT (udp_connection_manager_p);
  udp_connection_manager_p->initialize (std::numeric_limits<unsigned int>::max (),
                                        ACE_Time_Value (0, NET_STATISTIC_DEFAULT_VISIT_INTERVAL_MS * 1000));

  tcp_connection_configuration.messageAllocator = &message_allocator;
  tcp_connection_configuration.socketConfiguration.bufferSize =
    NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
  udp_connection_configuration.messageAllocator = &message_allocator;
  udp_connection_configuration.socketConfiguration.bufferSize =
    NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
  udp_connection_configuration_2.messageAllocator = &message_allocator;
  udp_connection_configuration_2.socketConfiguration.bufferSize =
    NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE;
  //  connection_configuration.statisticReportingInterval =
  //    ACE_Time_Value (NET_STREAM_DEFAULT_STATISTIC_REPORTING_INTERVAL, 0);

  std::string default_interface_identifier_string =
      Net_Common_Tools::getDefaultInterface (NET_LINKLAYER_802_11);
  ACE_ASSERT (!default_interface_identifier_string.empty ());
  ACE_INET_Addr ip_address, gateway_address;
  if (!Net_Common_Tools::interfaceToIPAddress (default_interface_identifier_string,
                                               ip_address,
                                               gateway_address))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
                ACE_TEXT (default_interface_identifier_string.c_str ())));
    return;
  } // end IF
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\" --> \"%s\" (gateway: \"%s\")\n"),
              ACE_TEXT (default_interface_identifier_string.c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (ip_address, true, false).c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (gateway_address, true, false).c_str ())));
  udp_connection_configuration.socketConfiguration.listenAddress = ip_address;
  udp_connection_configuration.socketConfiguration.listenAddress.set_port_number (ARDRONE_PORT_UDP_NAVDATA,
                                                                                  1);
  udp_connection_configuration.socketConfiguration.reuseAddress = true;

  tcp_connection_configuration.socketConfiguration.address = gateway_address;
  tcp_connection_configuration.socketConfiguration.address.set_port_number (ARDRONE_PORT_TCP_CONTROL,
                                                                            1);
  tcp_connection_configuration.allocatorConfiguration = &configuration_in.allocatorConfiguration;
  tcp_connection_configuration.streamConfiguration = &configuration_in.streamConfiguration_net;

  udp_connection_configuration.allocatorConfiguration = &configuration_in.allocatorConfiguration;
  udp_connection_configuration.streamConfiguration = &configuration_in.streamConfiguration_net;
  udp_connection_configuration_2.allocatorConfiguration = &configuration_in.allocatorConfiguration;
  udp_connection_configuration_2.streamConfiguration = &configuration_in.streamConfiguration_net;

  configuration_in.connectionConfigurations_2.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (""),
                                                                      &tcp_connection_configuration));

  configuration_in.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                    &udp_connection_configuration));

  udp_connection_configuration_2.socketConfiguration.peerAddress = gateway_address;
  udp_connection_configuration_2.socketConfiguration.peerAddress.set_port_number (ARDRONE_PORT_UDP_CONTROL_CONFIGURATION,
                                                                                  1);
  udp_connection_configuration_2.socketConfiguration.connect = !useReactor_in;
  udp_connection_configuration_2.socketConfiguration.writeOnly = true;
  udp_connection_configuration_2.delayRead = true;
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("control peer: %s\n"),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (udp_connection_configuration_2.socketConfiguration.peerAddress).c_str ())));
  configuration_in.connectionConfigurations.insert (std::make_pair (ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MODULE_CONTROLLER_NAME_STRING),
                                                                    &udp_connection_configuration_2));
//  Net_ConnectionConfigurationsIterator_t connection_configurations_iterator =
//      configuration_in.connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
//  ACE_ASSERT (connection_configurations_iterator != configuration_in.connectionConfigurations.end ());
  tcp_connection_manager_p->set (tcp_connection_configuration,
                                 NULL); // passed to all handlers
  udp_connection_manager_p->set (udp_connection_configuration,
                                 NULL); // passed to all handlers

  struct Common_TimerConfiguration timer_configuration;
  Common_Timer_Manager_t* timer_manager_p = NULL;

  // intialize timers
  timer_manager_p = COMMON_TIMERMANAGER_SINGLETON::instance ();
  ACE_ASSERT (timer_manager_p);
  timer_manager_p->initialize (timer_configuration);
  timer_manager_p->start (NULL);

  // event dispatch
  if (useReactor_in)
  {
    configuration_in.dispatchConfiguration.dispatch =
      COMMON_EVENT_DISPATCH_REACTOR;
    configuration_in.dispatchConfiguration.numberOfReactorThreads =
      NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS;
    configuration_in.dispatchConfiguration.reactorType =
      (configuration_in.dispatchConfiguration.numberOfReactorThreads > 1) ? COMMON_REACTOR_THREAD_POOL : COMMON_REACTOR_ACE_DEFAULT;
  } // end IF
  else
    configuration_in.dispatchConfiguration.numberOfProactorThreads =
      NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS;
  if (!Common_Event_Tools::initializeEventDispatch (configuration_in.dispatchConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    return;
  } // end IF

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  struct Common_EventDispatchState dispatch_state_s;
  dispatch_state_s.configuration = &configuration_in.dispatchConfiguration;
  if (!Common_Event_Tools::startEventDispatch (dispatch_state_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, returning\n")));
    return;
  } // end IF

  // UI
  //ACE_thread_t thread_id;
#if defined (GUI_SUPPORT)
  CBData_in.stream = &stream;
#if defined (GTK_USE)
  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  gtk_manager_p->start (NULL);
#endif // GTK_USE
#endif // GUI_SUPPORT

  // step0f: (initialize) processing stream
  Stream_IStreamControlBase* stream_p = NULL;
  if (!stream.initialize (configuration_in.streamConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize navdata stream, returning\n")));
    return;
  } // end IF
  Stream_Module_t* module_p =
      const_cast<Stream_Module_t*> (stream.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MODULE_CONTROLLER_NAME_STRING)));
  ACE_ASSERT (module_p);
  stream_configuration_2.deviceConfiguration =
    dynamic_cast<ARDrone_IDeviceConfiguration*> (module_p->writer ());
  ACE_ASSERT (stream_configuration_2.deviceConfiguration);
//  (*stream_iterator).second.second.deviceConfiguration =
//      dynamic_cast<ARDrone_IDeviceConfiguration*> (module_p->writer ());
//  ACE_ASSERT ((*stream_iterator).second.second.deviceConfiguration);
  CBData_in.controller =
    dynamic_cast<ARDrone_IController*> (module_p->writer ());
  ACE_ASSERT (CBData_in.controller);
  stream_configuration_2.initializeControl = &ui_event_handler;
  modulehandler_configuration_2.deviceConfiguration =
      dynamic_cast<ARDrone_IDeviceConfiguration*> (module_p->writer ());
  ACE_ASSERT (modulehandler_configuration_2.deviceConfiguration);
  if (!control_stream.initialize (configuration_in.streamConfiguration_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize control stream, returning\n")));
    return;
  } // end IF

  stream_p = &control_stream;
  ACE_ASSERT (stream_p);
  stream_p->start ();

  stream_p = &stream;
  ACE_ASSERT (stream_p);
  stream_p->start ();
//    if (!stream_p->isRunning ())
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to start stream, aborting\n")));
//      //timer_manager_p->stop ();
//      return;
//    } // end IF

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  gtk_manager_p->wait (false);
#endif // GTK_USE

  tcp_connection_manager_p->stop (false);  // wait for completion ?
  udp_connection_manager_p->stop (false);  // wait for completion ?
  tcp_connection_manager_p->abort (false); // wait for completion ?
  udp_connection_manager_p->abort (false); // wait for completion ?
  tcp_connection_manager_p->wait ();
  udp_connection_manager_p->wait ();

  stream_p->wait (true, false, false);
#else
  stream_p->wait (true, false, false);

  tcp_connection_manager_p->stop (false);  // wait for completion ?
  udp_connection_manager_p->stop (false);  // wait for completion ?
  tcp_connection_manager_p->abort (false); // wait for completion ?
  udp_connection_manager_p->abort (false); // wait for completion ?
  tcp_connection_manager_p->wait ();
  udp_connection_manager_p->wait ();
#endif // GUI_SUPPORT
  control_stream.stop (true, false, false);

  // step3: clean up
  timer_manager_p->stop ();

  Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                             true,  // wait ?
                                             false); // close singletons ?

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

  ACE_Sig_Set signal_set (false);
  Common_SignalActions_t previous_signal_actions_a;
  ACE_Sig_Set previous_signal_mask;
  Common_Signal_Tools::preInitialize (signal_set,
                                      COMMON_SIGNAL_DISPATCH_PROACTOR,
                                      true,
                                      false,
                                      previous_signal_actions_a,
                                      previous_signal_mask);

  std::string configuration_path =
    Common_File_Tools::getWorkingDirectory ();

  // initialize framework(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_Tools::initialize (false,  // intialize COM ?
                            false); // initialize random number generator ?
  Stream_MediaFramework_Tools::initialize (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK);
#else
  Common_Tools::initialize (false); // initialize random number generator ?
#endif // ACE_WIN32 || ACE_WIN64

  // step1a set defaults
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    bool show_console = false;
#endif // ACE_WIN32 || ACE_WIN64
    std::string UI_definition_filename = configuration_path;
    UI_definition_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    UI_definition_filename += COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY;
    UI_definition_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    UI_definition_filename +=
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_DEFINITION_FILE);
    bool log_to_file = false;
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
#endif // ACE_WIN32 || ACE_WIN64
                             UI_definition_filename,
                             log_to_file,
                             display_device_s,
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

  // step1c: validate arguments
  // *IMPORTANT NOTE*: iff the number of message buffers is limited, the
  //                   reactor/proactor thread could (dead)lock on the
  //                   allocator lock, as it cannot dispatch events that would
  //                   free slots
  if (0)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("limiting the number of message buffers could (!) lead to a deadlock --> ensure the streaming elements are sufficiently efficient in this regard\n")));
  if ((!UI_definition_filename.empty () &&
       !Common_File_Tools::isReadable (UI_definition_filename)))
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
                                          ACE::basename (argv_in[0]));
  if (!Common_Log_Tools::initialize (ACE::basename (argv_in[0]),                   // program name
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

  struct Test_U_Configuration configuration;

#if defined (GUI_SUPPORT)
  struct Test_U_UI_CBData ui_cb_data_s;
  ui_cb_data_s.configuration = &configuration;
#if defined (GTK_USE)
  Common_UI_GtkBuilderDefinition_t gtk_ui_definition;
  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  Common_UI_GTK_State_t& state_r =
    const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR ());
  state_r.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (UI_definition_filename, static_cast<GtkBuilder*> (NULL));

  ui_cb_data_s.UIState = &state_r;

  configuration.GTKConfiguration.argc = argc_in;
  configuration.GTKConfiguration.argv = argv_in;
  configuration.GTKConfiguration.CBData = &ui_cb_data_s;
  configuration.GTKConfiguration.eventHooks.finiHook =
    idle_finalize_UI_cb;
  configuration.GTKConfiguration.eventHooks.initHook =
    idle_initialize_UI_cb;
  configuration.GTKConfiguration.definition = &gtk_ui_definition;

  bool result_2 = gtk_manager_p->initialize (configuration.GTKConfiguration);
  ACE_ASSERT (result_2);
#elif defined (QT_USE)
#endif // GTK_USE
#endif // GUI_SUPPORT

  ACE_High_Res_Timer timer;
  timer.start ();
  // step2: do actual work
  do_work (use_reactor_b,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
           show_console,
#endif // ACE_WIN32 || ACE_WIN64
           display_device_s,
           configuration
#if defined (GUI_SUPPORT)
           ,ui_cb_data_s
#endif // GUI_SUPPORT
          );
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

  // *PORTABILITY*: on Windows, finalize ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE::fini ();
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
