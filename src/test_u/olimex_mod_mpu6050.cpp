/*  I2C kernel module driver test for the Olimex MOD-MPU6050 UEXT module
    (see https://www.olimex.com/Products/Modules/Sensors/MOD-MPU6050/open-source-hardware,
         http://www.invensense.com/mems/gyro/mpu6050.html)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#if defined(_MSC_VER)
#include "stdafx.h"
#endif

#include <iostream>
#include <sstream>

#if defined(ENABLE_NLS)
#include <locale.h>
#include <libintl.h>
#endif
#include "gettext.h"

#include "ace/OS.h"
#include "ace/OS_main.h"
#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"

#ifdef HAVE_CONFIG_H
#include "olinuxino_config.h"
#endif

#include "rpg_stream_allocatorheap.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_connection_manager.h"
#include "rpg_net_stream_messageallocator.h"
#include "rpg_net_module_eventhandler.h"

#include "rpg_net_client_defines.h"
#include "rpg_net_client_connector.h"
#include "rpg_net_client_asynchconnector.h"

#include "rpg_net_server_defines.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_eventhandler.h"
#include "olimex_mod_mpu6050_types.h"

void
do_printVersion (const std::string& programName_in)
{
  // step1: program version
  //   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in
            << ACE_TEXT_ALWAYS_CHAR (": ")
#ifdef HAVE_CONFIG_H
            << OLINUXINO_VERSION
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
do_printUsage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT("-l           : log to a file [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-r           : use reactor [")
            << DEF_USE_REACTOR
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-t           : trace information [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  std::cout << ACE_TEXT("-v           : print version information and exit [")
            << false
            << ACE_TEXT("]")
            << std::endl;
}

bool
do_processArguments(const int& argc_in,
                    ACE_TCHAR** argv_in, // cannot be const...
                    bool& logToFile_out,
                    bool& useReactor_out,
                    bool& traceInformation_out,
                    bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::do_processArguments"));

  // init results
  logToFile_out           = false;
  useReactor_out          = DEF_USE_REACTOR;
  traceInformation_out    = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("lrtv"),
                             1,                          // skip command name
                             1,                          // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS,  // ordering
                             0);                         // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'l':
      {
        logToFile_out = true;
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
        printVersionAndExit_out = true;
        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("option \"%c\" requires an argument, aborting\n"),
                   argumentParser.opt_opt()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   ACE_TEXT(argumentParser.last_option())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("found long option \"%s\", aborting\n"),
                   ACE_TEXT(argumentParser.long_option())));
        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("parse error, aborting\n")));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_work(bool useReactor_in)
{
  // step0: init stream
  Olimex_Mod_MPU6050_GtkCBData_t CBData;
  Olimex_Mod_MPU6050_EventHandler event_handler(&CBData);
  RPG_Net_Module_EventHandler_Module event_handler(std::string("EventHandler"),
                                                   NULL);
  RPG_Net_Module_EventHandler* eventHandler_impl = NULL;
  eventHandler_impl =
      dynamic_cast<RPG_Net_Module_EventHandler*>(event_handler.writer());
  if (!eventHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("dynamic_cast<RPG_Net_Module_EventHandler> failed, aborting\n")));
    return;
  } // end IF
  eventHandler_impl->init(&CBData_in.subscribers,
                          &CBData_in.lock);
  eventHandler_impl->subscribe(&ui_event_handler);
  RPG_Stream_AllocatorHeap heapAllocator;
  RPG_Net_StreamMessageAllocator messageAllocator(RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                  &heapAllocator);
  RPG_Net_ConfigPOD configuration;
  ACE_OS::memset(&configuration, 0, sizeof(configuration));
  // ************ connection config data ************
  configuration.peerPingInterval =
      ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? 0
                                                                   : serverPingInterval_in);
  configuration.pingAutoAnswer = true;
  configuration.printPongMessages = true;
  configuration.streamSocketConfiguration.socketBufferSize = RPG_NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE;
  configuration.streamSocketConfiguration.messageAllocator = &messageAllocator;
  configuration.streamSocketConfiguration.bufferSize = RPG_NET_STREAM_BUFFER_SIZE;
//  config.useThreadPerConnection = false;
//  config.serializeOutput = false;
  // ************ stream config data ************
//  config.notificationStrategy = NULL;
  configuration.streamSocketConfiguration.module = (hasUI_in ? &event_handler
                                                             : NULL);
//  config.delete_module = false;
  // *WARNING*: set at runtime, by the appropriate connection handler
//  config.sessionID = 0; // (== socket handle !)
//  config.statisticsReportingInterval = 0; // == off
//	config.printFinalReport = false;
  // ************ runtime data ************
//	config.currentStatistics = {};
//	config.lastCollectionTimestamp = ACE_Time_Value::zero;

  // step0d: init event dispatch
  if (!RPG_Net_Common_Tools::initEventDispatch(useReactor_in,
                                               numDispatchThreads_in,
                                               configuration.streamSocketConfiguration.serializeOutput))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init event dispatch, aborting\n")));
    return;
  } // end IF

  // step1: init client connector
  RPG_Net_Client_IConnector* connector = NULL;
  if (useReactor_in)
    ACE_NEW_NORETURN(connector, RPG_Net_Client_Connector());
  else
    ACE_NEW_NORETURN(connector, RPG_Net_Client_AsynchConnector());
  if (!connector)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));
    return;
  } // end IF

  // step2: init connection manager
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->init(std::numeric_limits<unsigned int>::max());
  RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->set(configuration); // will be passed to all handlers

  // step3: init action timer ?
  ACE_INET_Addr peer_address(serverPortNumber_in,
                             serverHostname_in.c_str(),
                             AF_INET);
  Net_Client_TimeoutHandler timeout_handler((hasUI_in ? Net_Client_TimeoutHandler::ACTION_STRESS
                                                      : actionMode_in),
                                            maxNumConnections_in,
                                            peer_address,
                                            connector);
  CBData_in.timeout_handler = &timeout_handler;
  CBData_in.timer_id = -1;
  if (!hasUI_in)
  {
    // schedule action interval timer
    ACE_Event_Handler* event_handler = &timeout_handler;
    ACE_Time_Value interval(((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? (NET_CLIENT_DEF_SERVER_STRESS_INTERVAL / 1000)
                                                                                         : connectionInterval_in),
                            ((actionMode_in == Net_Client_TimeoutHandler::ACTION_STRESS) ? ((NET_CLIENT_DEF_SERVER_STRESS_INTERVAL % 1000) * 1000)
                                                                                         : 0));
    CBData_in.timer_id =
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->schedule(event_handler,                       // event handler
                                                              NULL,                                // ACT
                                                              RPG_COMMON_TIME_POLICY() + interval, // first wakeup time
                                                              interval);                           // interval
    if (CBData_in.timer_id == -1)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("failed to schedule action timer: \"%m\", aborting\n")));

      // clean up
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      delete connector;

      return;
    } // end IF
  } // end IF

  // step4: init signal handling
  Net_Client_SignalHandler signal_handler(CBData_in.timer_id, // action timer id
                                          peer_address,       // remote SAP
                                          connector,          // connector
                                          useReactor_in);     // use reactor ?
  if (!RPG_Common_Tools::initSignals(signalSet_inout,
                                     &signal_handler,
                                     previousSignalActions_inout))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init signal handling, aborting\n")));

    // clean up
    if (CBData_in.timer_id != -1)
    {
      const void* act = NULL;
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(CBData_in.timer_id,
                                                                &act) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   CBData_in.timer_id));
    } // end IF
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    connector->abort();
    delete connector;

    return;
  } // end IF

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step5a: start GTK event loop ?
  if (hasUI_in)
  {
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->start();
    if (!RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->isRunning())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to start GTK event dispatch, aborting\n")));

      // clean up
      if (CBData_in.timer_id != -1)
      {
        const void* act = NULL;
        if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(CBData_in.timer_id,
                                                                  &act) <= 0)
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                     CBData_in.timer_id));
      } // end IF
      RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
      connector->abort();
      delete connector;
      RPG_Common_Tools::finiSignals(signalSet_inout,
                                    useReactor_in,
                                    previousSignalActions_inout);

      return;
    } // end IF
  } // end IF

  // step5b: init worker(s)
  int group_id = -1;
  if (!RPG_Net_Common_Tools::startEventDispatch(useReactor_in,
                                                numDispatchThreads_in,
                                                group_id))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start event dispatch, aborting\n")));

    // clean up
    if (CBData_in.timer_id != -1)
    {
      const void* act = NULL;
      if (RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->cancel(CBData_in.timer_id,
                                                                &act) <= 0)
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("failed to cancel timer (ID: %d): \"%m\", continuing\n"),
                   CBData_in.timer_id));
    } // end IF
//		{ // synch access
//			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

//			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
//					 iterator != CBData_in.event_source_ids.end();
//					 iterator++)
//				g_source_remove(*iterator);
//		} // end lock scope
    RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->stop();
    RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
    connector->abort();
    delete connector;
    RPG_Common_Tools::finiSignals(signalSet_inout,
                                  useReactor_in,
                                  previousSignalActions_inout);

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started event dispatch...\n")));

  // step5c: connect immediately ?
  if (!hasUI_in && (connectionInterval_in == 0))
    connector->connect(peer_address);

  // *NOTE*: from this point on, we need to clean up any remote connections !

  // step6: dispatch events
  // *NOTE*: when using a thread pool, handle things differently...
  if (numDispatchThreads_in > 1)
  {
    if (ACE_Thread_Manager::instance()->wait_grp(group_id) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                 group_id));
  } // end IF
  else
  {
    if (useReactor_in)
    {
/*      // *WARNING*: restart system calls (after e.g. SIGINT) for the reactor
      ACE_Reactor::instance()->restart(1);
*/
      if (ACE_Reactor::instance()->run_reactor_event_loop(0) == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to handle events: \"%m\", aborting\n")));
    } // end IF
    else
      if (ACE_Proactor::instance()->proactor_run_event_loop(0) == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to handle events: \"%m\", aborting\n")));
  } // end ELSE

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished event dispatch...\n")));

  // step7: clean up
  // *NOTE*: any action timer has been cancelled, connections have been
  // aborted and any GTK event dispatcher has returned by now...
  RPG_Common_Tools::finiSignals(signalSet_inout,
                                useReactor_in,
                                previousSignalActions_inout);
//  { // synch access
//    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(CBData_in.lock);

//		for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin();
//				 iterator != CBData_in.event_source_ids.end();
//				 iterator++)
//			g_source_remove(*iterator);
//	} // end lock scope
//  RPG_CLIENT_GTK_MANAGER_SINGLETON::instance()->stop();
  RPG_COMMON_TIMERMANAGER_SINGLETON::instance()->stop();
  delete connector;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR** argv_in)
{
  // step0: init ACE
  // *PORTABILITY*: on Windows, ACE needs initialization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // step1: process commandline options (if any)
  log_to_file            = false;
  use_reactor            = DEF_USE_REACTOR;
  trace_information      = false;
  print_version_and_exit = false;
  if (!do_processArguments (argc_in,
                            argv_in,
                            log_to_file,
                            use_reactor,
                            trace_information,
                            print_version_and_exit))
  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to do_processArguments(), aborting\n")));

    do_printVersion (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step2: initialize logging and/or tracing
  char buffer[PATH_MAX];
  if (ACE::getcwd (buffer, sizeof (buffer)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::getcwd(): \"%m\", aborting\n")));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  std::string log_file = buffer;
  log_file += ACE_DIRECTORY_SEPARATOR_STR;
  log_file += ACE_TEXT_ALWAYS_CHAR (DEF_LOG_FILE);
  // *NOTE*: default log target is stderr
  u_long options_flags = ACE_Log_Msg::STDERR;
  if (log_to_file)
  {
    options_flags |= ACE_Log_Msg::OSTREAM;

    ACE_OSTREAM_TYPE* log_stream;
    std::ios_base::openmode open_mode = (std::ios_base::out |
                                         std::ios_base::trunc);
    ACE_NEW_NORETURN(log_stream,
                     std::ofstream (log_file.c_str (),
                                    open_mode));
    if (!log_stream)
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("failed to allocate memory: \"%m\", aborting\n")));

      // *PORTABILITY*: on Windows, ACE needs finalization...
  #if defined (ACE_WIN32) || defined (ACE_WIN64)
      if (ACE::fini () == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
  #endif

      return EXIT_FAILURE;
    } // end IF
    if (log_stream->fail ())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to initialize logfile: \"%m\", aborting\n")));

      // clean up
      delete log_stream;

      // *PORTABILITY*: on Windows, ACE needs finalization...
  #if defined (ACE_WIN32) || defined (ACE_WIN64)
      if (ACE::fini () == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
  #endif

      return EXIT_FAILURE;
    } // end IF

    // *NOTE*: the logger singleton assumes ownership of the stream lifecycle
    ACE_LOG_MSG->msg_ostream (log_stream, 1);
  } // end IF
  if (ACE_LOG_MSG->open (ACE_TEXT_CHAR_TO_TCHAR (ACE::basename (argv_in[0])),
                         options_flags,
                         NULL) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT ("failed to ACE_Log_Msg::open(\"%s\", %u): \"%m\", aborting\n"),
               ACE_TEXT (ACE::basename (argv_in[0])),
               options_flags));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // set new mask...
  u_long process_priority_mask = (LM_SHUTDOWN |
                                  LM_TRACE    |
                                  LM_DEBUG    |
                                  LM_INFO     |
                                  LM_NOTICE   |
                                  LM_WARNING  |
                                  LM_STARTUP  |
                                  LM_ERROR    |
                                  LM_CRITICAL |
                                  LM_ALERT    |
                                  LM_EMERGENCY);
  if (!trace_information)
    process_priority_mask &= ~LM_TRACE;
  ACE_LOG_MSG->priority_mask(process_priority_mask,
                             ACE_Log_Msg::PROCESS);

  // step3: init NLS
#ifdef ENABLE_NLS
#ifdef HAVE_LOCALE_H
  setlocale (LC_ALL, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  // step4: run program
  if (print_version_and_exit)
    do_printVersion (ACE::basename (argv_in[0]));
  else
    do_work (use_reactor);

  // step5: clean up
  // *PORTABILITY*: on Windows, must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
