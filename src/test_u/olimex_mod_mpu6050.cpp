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

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_main.h"
#include "ace/Time_Value.h"

#ifdef HAVE_CONFIG_H
#include "olinuxino_config.h"
#endif

#include "common_tools.h"

#include "common_ui_gtk_manager.h"

#include "stream_allocatorheap.h"

#include "net_configuration.h"

#include "net_client_connector.h"
#include "net_client_asynchconnector.h"

#include "olimex_mod_mpu6050_callbacks.h"
#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_eventhandler.h"
#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_module_eventhandler.h"
#include "olimex_mod_mpu6050_network.h"
#include "olimex_mod_mpu6050_signalhandler.h"
#include "olimex_mod_mpu6050_stream_common.h"
#include "olimex_mod_mpu6050_types.h"

void
do_printVersion (const std::string& programName_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::do_printVersion"));

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
do_printUsage (const std::string& programName_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT ("usage: ")
            << programName_in
            << ACE_TEXT (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT ("-l           : log to a file [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-r           : use reactor [")
            << OLIMEX_MOD_MPU6050_USE_REACTOR
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-s           : server address[:port] (IPv4)")
            << std::endl;
  std::cout << ACE_TEXT ("-t           : trace information [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-u           : interface definition file")
            << std::endl;
  std::cout << ACE_TEXT ("-v           : print version information and exit [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     bool& logToFile_out,
                     bool& useReactor_out,
                     ACE_INET_Addr& peerAddress_out,
                     bool& traceInformation_out,
                     std::string& interfaceDefinitionFile_out,
                     bool& printVersionAndExit_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::do_processArguments"));

  // init results
  logToFile_out               = false;
  useReactor_out              = OLIMEX_MOD_MPU6050_USE_REACTOR;
  traceInformation_out        = false;
  interfaceDefinitionFile_out = ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_DEFINITION_FILE_NAME);
  printVersionAndExit_out     = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("lrs:tu:v"),
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
      case 's':
      {
        int result = -1;
        std::string address = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        if (address.find (':') != std::string::npos)
          result = peerAddress_out.set (address.c_str (), 0);
        else
          result = peerAddress_out.set (OLIMEX_MOD_MPU6050_DEFAULT_PORT,
                                        address.c_str (),
                                        1,
                                        0);
        if (result == -1)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", aborting\n"),
                      ACE_TEXT (argumentParser.opt_arg ())));
          return false;
        } // end IF
        break;
      }
      case 't':
      {
        traceInformation_out = true;
        break;
      }
      case 'u':
      {
        interfaceDefinitionFile_out =
            ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("option \"%c\" requires an argument, aborting\n"),
                    argumentParser.opt_opt ()));
        return false;
      }
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argumentParser.last_option ())));
        return false;
      }
      case 0:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("found long option \"%s\", aborting\n"),
                    ACE_TEXT (argumentParser.long_option ())));
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
do_initSignals (ACE_Sig_Set& signals_inout)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::do_initSignals"));

  // init return value(s)
  if (signals_inout.empty_set () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::empty_set(): \"%m\", aborting\n")));
    return;
  } // end IF

  // *PORTABILITY*: on Windows most signals are not defined,
  // and ACE_Sig_Set::fill_set() doesn't really work as specified
  // --> add valid signals (see <signal.h>)...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  signals_inout.sig_add (SIGINT);         // 2       /* interrupt */
  signals_inout.sig_add (SIGILL);         // 4       /* illegal instruction - invalid function image */
  signals_inout.sig_add (SIGFPE);         // 8       /* floating point exception */
  //signals_inout.sig_add (SIGSEGV);        // 11      /* segment violation */
  signals_inout.sig_add (SIGTERM);        // 15      /* Software termination signal from kill */
  //signals_inout.sig_add (SIGBREAK);       // 21      /* Ctrl-Break sequence */
  signals_inout.sig_add (SIGABRT);        // 22      /* abnormal termination triggered by abort call */
  signals_inout.sig_add (SIGABRT_COMPAT); // 6       /* SIGABRT compatible with other platforms, same as SIGABRT */
#else
  if (signals_inout.fill_set () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Sig_Set::fill_set(): \"%m\", aborting\n")));
    return;
  } // end IF
  // *NOTE*: cannot handle some signals --> registration fails for these...
  signals_inout.sig_del (SIGKILL);        // 9       /* Kill signal */
  signals_inout.sig_del (SIGSTOP);        // 19      /* Stop process */
  // ---------------------------------------------------------------------------
  //signals_inout.sig_del (SIGUSR1);        // 10      /* User-defined signal 1 */
  // *NOTE* core dump on SIGSEGV
  signals_inout.sig_del (SIGSEGV);        // 11      /* Segmentation fault: Invalid memory reference */
  // *NOTE* don't care about SIGPIPE
  signals_inout.sig_del (SIGPIPE);        // 12      /* Broken pipe: write to pipe with no readers */

  // *TODO*
#ifdef ENABLE_VALGRIND_SUPPORT
  // *NOTE*: valgrind uses SIGRT32 (--> SIGRTMAX ?) and apparently will not work
  // if the application installs its own handler (see documentation)
  if (RUNNING_ON_VALGRIND)
    signals_inout.sig_del (SIGRTMAX);     // 64
#endif
#endif
}

void
do_work (int argc_in,
         ACE_TCHAR** argv_in,
         const ACE_INET_Addr& peerAddress_in,
         bool useAsynchConnector_in,
         bool useReactor_in,
         const std::string& interfaceDefinitionFile_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::do_work"));

  // step1: init stream
  Olimex_Mod_MPU6050_GtkCBData_t gtk_cb_data;
  Olimex_Mod_MPU6050_EventHandler event_handler (&gtk_cb_data);
  Olimex_Mod_MPU6050_Module_EventHandler_Module event_handler_module (std::string ("EventHandler"),
                                                                      NULL);
  Olimex_Mod_MPU6050_Module_EventHandler* event_handler_impl = NULL;
  event_handler_impl =
      dynamic_cast<Olimex_Mod_MPU6050_Module_EventHandler*> (event_handler_module.writer ());
  if (!event_handler_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_EventHandler> failed, returning\n")));
    return;
  } // end IF
  event_handler_impl->initialize (&gtk_cb_data.subscribers,
                                  &gtk_cb_data.lock);
  event_handler_impl->subscribe (&event_handler);
  Stream_AllocatorHeap heap_allocator;
  Olimex_Mod_MPU6050_MessageAllocator_t message_allocator (OLIMEX_MOD_MPU6050_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                           &heap_allocator);
  //Olimex_Mod_MPU6050_SessionData_t session_data;
  Net_SessionData_t session_data;
  ACE_OS::memset (&session_data, 0, sizeof (session_data));
  Stream_State_t stream_state;
  ACE_OS::memset (&stream_state, 0, sizeof (stream_state));
  //Olimex_Mod_MPU6050_StreamSessionData_t stream_session_data (&session_data,
  Net_StreamSessionData_t stream_session_data (&session_data,
                                               false,
                                               &stream_state,
                                               ACE_Time_Value::zero,
                                               false);
  Net_Configuration_t configuration;
  ACE_OS::memset (&configuration, 0, sizeof (configuration));
  // ******************* socket configuration data ****************************
  configuration.socketConfiguration.bufferSize = OLIMEX_MOD_MPU6050_SOCKET_RECEIVE_BUFFER_SIZE;
  configuration.socketConfiguration.peerAddress = peerAddress_in;
//  configuration.socketConfiguration.useLoopbackDevice = false;
  // ******************** stream configuration data ***************************
  configuration.streamConfiguration.messageAllocator = &message_allocator;
  configuration.streamConfiguration.bufferSize = OLIMEX_MOD_MPU6050_STREAM_BUFFER_SIZE;
  configuration.streamConfiguration.useThreadPerConnection = false;
  //configuration.streamConfiguration.serializeOutput = false;
  configuration.streamConfiguration.notificationStrategy = NULL;
  configuration.streamConfiguration.module = &event_handler_module;
  configuration.streamConfiguration.deleteModule = false;
  // *WARNING*: set at runtime, by the appropriate connection handler
  configuration.streamConfiguration.statisticsReportingInterval = 0; // == off
  configuration.streamConfiguration.printFinalReport = false;
//  // ******************** protocol configuration data ***********************
//  configuration.protocolConfiguration.peerPingInterval = 0; // don't ping the server
//  configuration.protocolConfiguration.pingAutoAnswer = false;
//  configuration.protocolConfiguration.printPongMessages = false;

  //// *************************** runtime data **********************************
  //configuration.sessionID = 0; // (== socket handle !)
  ////  configuration.currentStatistics = {};
  //configuration.lastCollectionTimestamp = ACE_Time_Value::zero;

  // step2: init event dispatch
  bool serialize_output;
  if (!Common_Tools::initializeEventDispatch (useReactor_in,
                                              1,
                                              serialize_output))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    return;
  } // end IF

  // step3: init client connector
//  Net_IUDPConnectionManager_t* connection_manager_p = ;
  Olimex_Mod_MPU6050_IConnector_t* connector = NULL;
  if (useAsynchConnector_in)
  {
    ACE_NEW_NORETURN (connector,
                      Olimex_Mod_MPU6050_AsynchConnector_t (CONNECTIONMANAGER_SINGLETON::instance (),
                                                            &configuration));
  } // end IF
  else
    ACE_NEW_NORETURN (connector,
                      Olimex_Mod_MPU6050_Connector_t (CONNECTIONMANAGER_SINGLETON::instance (),
                                                      &configuration));
  if (!connector)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return;
  } // end IF

  // step4: init connection manager
  CONNECTIONMANAGER_SINGLETON::instance ()->initialize (std::numeric_limits<unsigned int>::max ());
  CONNECTIONMANAGER_SINGLETON::instance ()->set (configuration,
                                                 stream_session_data); // will be passed to all handlers

  // step5: init signal handling
  Olimex_Mod_MPU6050_SignalHandler signal_handler (peerAddress_in, // peer address
                                                   connector,      // connector
                                                   useReactor_in); // use reactor ?
  ACE_Sig_Set signal_set (0);
  do_initSignals (signal_set);
  Common_SignalActions_t previous_signal_actions;
  if (!Common_Tools::preInitializeSignals (signal_set,
                                           useReactor_in,
                                           previous_signal_actions))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::preInitializeSignals(), aborting\n")));
    delete connector;
    return;
  } // end IF
  if (!Common_Tools::initializeSignals (signal_set,
                                        &signal_handler,
                                        previous_signal_actions))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handling, aborting\n")));
    delete connector;
    return;
  } // end IF

  // event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step6a: start GTK event loop
  Olimex_Mod_MPU6050_GTKUIDefinition interface_definition (argc_in,
                                                           argv_in,
                                                           &gtk_cb_data);
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                            argv_in,
                                                            interfaceDefinitionFile_in,
                                                            &interface_definition);
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->start ();
  ACE_OS::sleep (ACE_Time_Value (0, OLIMEX_MOD_MPU6050_UI_INITIALIZATION_DELAY));
  if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start GTK event dispatch, aborting\n")));
    Common_Tools::finalizeSignals (signal_set,
                                   useReactor_in,
                                   previous_signal_actions);
    delete connector;
    return;
  } // end IF

  // step6b: init worker(s)
  int group_id = -1;
  if (!Common_Tools::startEventDispatch (useReactor_in,
                                         1,
                                         group_id))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start event dispatch, aborting\n")));
//		{ // synch access
//			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard  (CBData_in.lock);

//			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin ();
//					 iterator != CBData_in.event_source_ids.end ();
//					 iterator++)
//				g_source_remove (*iterator);
//		} // end lock scope
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    Common_Tools::finalizeSignals (signal_set,
                                   useReactor_in,
                                   previous_signal_actions);
    delete connector;
    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

  // step7: connect
  if (!connector->connect (peerAddress_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to connect, aborting\n")));
    Common_Tools::finalizeEventDispatch (useReactor_in,
                                         !useReactor_in,
                                         group_id);
    //		{ // synch access
    //			ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard  (CBData_in.lock);

    //			for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin ();
    //					 iterator != CBData_in.event_source_ids.end ();
    //					 iterator++)
    //				g_source_remove (*iterator);
    //		} // end lock scope
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    Common_Tools::finalizeSignals (signal_set,
                                   useReactor_in,
                                   previous_signal_actions);
    delete connector;
    return;
  }
  // *WARNING*: from this point on, clean up any remote connections !

  // step8: dispatch events
  // *NOTE*: when using a thread pool, handle things differently...
  if (useReactor_in)
  {
/*      // *WARNING*: restart system calls (after e.g. SIGINT) for the reactor
      ACE_Reactor::instance()->restart(1);
*/
    if (ACE_Reactor::instance ()->run_reactor_event_loop (0) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to handle events: \"%m\", aborting\n")));
  } // end IF
  else
    if (ACE_Proactor::instance ()->proactor_run_event_loop (0) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to handle events: \"%m\", aborting\n")));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished event dispatch...\n")));

  // step9: clean up
  // *NOTE*: any action timer has been cancelled, connections have been
  // aborted and any GTK event dispatcher has returned by now...
  Common_Tools::finalizeSignals (signal_set,
                                 useReactor_in,
                                 previous_signal_actions);
//  { // synch access
//    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (CBData_in.lock);

//		for (Net_GTK_EventSourceIDsIterator_t iterator = CBData_in.event_source_ids.begin ();
//				 iterator != CBData_in.event_source_ids.end ();
//				 iterator++)
//			g_source_remove (*iterator);
//	} // end lock scope
//  CLIENT_GTK_MANAGER_SINGLETON::instance ()->stop ();
  delete connector;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
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
  bool log_to_file            = false;
  bool use_asynch_connector   = OLIMEX_MOD_MPU6050_USE_ASYNCH_CONNECTOR;
  bool use_reactor            = OLIMEX_MOD_MPU6050_USE_REACTOR;
  ACE_INET_Addr peer_address;
  bool trace_information      = false;
  std::string interface_definition_file =
      ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_DEFINITION_FILE_NAME);
  bool print_version_and_exit = false;
  if (!do_processArguments (argc_in,
                            argv_in,
                            log_to_file,
                            use_reactor,
                            peer_address,
                            trace_information,
                            interface_definition_file,
                            print_version_and_exit))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to do_processArguments(), aborting\n")));

    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step2: validate configuration
  if (peer_address.is_any () ||
      (use_reactor && use_asynch_connector))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to validate configuration, aborting\n")));

    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step3: initialize logging and/or tracing
  char buffer[PATH_MAX];
  if (!ACE_OS::getcwd (buffer, sizeof (buffer)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getcwd(): \"%m\", aborting\n")));

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
  log_file += ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_LOG_FILE_NAME);
  if (!log_to_file) log_file.clear ();
  if (!Common_Tools::initializeLogging (ACE::basename (argv_in[0]),
                                        log_file,
                                        false,
                                        trace_information,
                                        true,
                                        NULL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize logging, aborting\n")));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step4: init NLS
#ifdef ENABLE_NLS
#ifdef HAVE_LOCALE_H
  setlocale (LC_ALL, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");
  textdomain (PACKAGE);
#endif

  // step5: run program
  if (print_version_and_exit)
    do_printVersion (ACE::basename (argv_in[0]));
  else
    do_work (argc_in,
             argv_in,
             peer_address,
             use_asynch_connector,
             use_reactor,
             interface_definition_file);

  // step6: clean up
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
