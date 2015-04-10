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

#include "stdafx.h"

#include <iostream>
#include <sstream>

#if defined (ENABLE_NLS)
#include <locale.h>
#include <libintl.h>
#endif
#include "gettext.h"

#if defined (OLINUXINO_ENABLE_VALGRIND_SUPPORT)
#include "valgrind/memcheck.h"
#endif

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_main.h"
#include "ace/Time_Value.h"

#include "olinuxino_config.h"

#include "common_file_tools.h"
#include "common_tools.h"

#include "common_ui_gtk_manager.h"

#include "stream_allocatorheap.h"

#include "net_configuration.h"
#include "net_defines.h"

#include "net_client_connector.h"
#include "net_client_asynchconnector.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_eventhandler.h"
#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_module_eventhandler.h"
#include "olimex_mod_mpu6050_network.h"
#include "olimex_mod_mpu6050_signalhandler.h"
#include "olimex_mod_mpu6050_stream_common.h"
#include "olimex_mod_mpu6050_types.h"
#include "olimex_mod_mpu6050_uidefinition.h"

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
  std::cout << ACE_TEXT ("-c           : client mode [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  std::cout << ACE_TEXT ("-g           : (netlink) multicast group [")
            << OLIMEX_MOD_MPU6050_DEFAULT_NETLINK_GROUP
            << ACE_TEXT ("]")
            << std::endl;
#endif
  std::cout << ACE_TEXT ("-l           : log to a file [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-r           : use reactor [")
            << OLIMEX_MOD_MPU6050_USE_REACTOR
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-s [IPv4]    : server address[:port]")
            << std::endl;
  std::cout << ACE_TEXT ("-t           : trace information [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-u [PATH]    : (libglade) interface definition file")
            << std::endl;
  std::cout << ACE_TEXT ("-v           : print version information and exit [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
  std::cout << ACE_TEXT ("-x           : no gui [")
            << false
            << ACE_TEXT ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     bool& clientMode_out,
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
                     unsigned int& netlinkGroup_out,
#endif
                     bool& logToFile_out,
                     bool& useReactor_out,
                     ACE_INET_Addr& peerAddress_out,
                     bool& traceInformation_out,
                     std::string& interfaceDefinitionFile_out,
                     bool& printVersionAndExit_out,
                     bool& consoleMode_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::do_processArguments"));

  // init results
  clientMode_out              = false;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  netlinkGroup_out            = OLIMEX_MOD_MPU6050_DEFAULT_NETLINK_GROUP;
#endif
  logToFile_out               = false;
  useReactor_out              = OLIMEX_MOD_MPU6050_USE_REACTOR;
  traceInformation_out        = false;
  interfaceDefinitionFile_out = ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_DEFINITION_FILE_NAME);
  printVersionAndExit_out     = false;
  consoleMode_out             = false;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
                              ACE_TEXT ("cg:lrs:tu:vx"),
#else
                              ACE_TEXT ("clrs:tu:vx"),
#endif
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
      case 'c':
      {
        clientMode_out = true;
        break;
      }
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      case 'g':
      {
        converter.str (ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ()));
        converter >> netlinkGroup_out;

        break;
      }
#endif
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
      case 'x':
      {
        consoleMode_out = true;
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

  // *TODO*: improve valgrind support
#ifdef OLINUXINO_ENABLE_VALGRIND_SUPPORT
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
         bool clientMode_in,
         const ACE_INET_Addr& peerAddress_in,
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
         const ACE_Netlink_Addr& netlinkAddress_in,
#endif
         bool useAsynchConnector_in,
         bool useReactor_in,
         const std::string& interfaceDefinitionFile_in,
         bool consoleMode_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::do_work"));

  bool result = false;

  // step1: initialize gtk cb data
  Olimex_Mod_MPU6050_GtkCBData_t gtk_cb_data;
  gtk_cb_data.argc = argc_in;
  gtk_cb_data.argv = argv_in;
  gtk_cb_data.clientMode = clientMode_in;
  ACE_OS::memset (&gtk_cb_data.clientSensorBias,
                  0,
                  sizeof (gtk_cb_data.clientSensorBias));
  gtk_cb_data.openGLDoubleBuffered = OLIMEX_MOD_MPU6050_OPENGL_DOUBLE_BUFFERED;
  ACE_OS::memset (gtk_cb_data.temperature,
                  0,
                  sizeof (gtk_cb_data.temperature));

  // step2: initialize stream
  Olimex_Mod_MPU6050_EventHandler event_handler (&gtk_cb_data);
  std::string module_name = ACE_TEXT_ALWAYS_CHAR ("EventHandler");
  Olimex_Mod_MPU6050_Module_EventHandler_Module event_handler_module (module_name,
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
  event_handler_impl->initialize ();
  event_handler_impl->subscribe (&event_handler);

  Stream_AllocatorHeap heap_allocator;
  Olimex_Mod_MPU6050_MessageAllocator_t message_allocator (OLIMEX_MOD_MPU6050_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                                                           &heap_allocator,
                                                           false); // do not block
  gtk_cb_data.allocator = &message_allocator;
  Net_Configuration_t configuration;
  ACE_OS::memset (&configuration, 0, sizeof (configuration));
  // ******************* socket configuration data ****************************
  configuration.socketConfiguration.bufferSize =
   OLIMEX_MOD_MPU6050_SOCKET_RECEIVE_BUFFER_SIZE;
  configuration.socketConfiguration.peerAddress = peerAddress_in;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  configuration.socketConfiguration.netlinkAddress = netlinkAddress_in;
  configuration.socketConfiguration.netlinkProtocol =
      OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL;
#endif
  //  configuration.socketConfiguration.useLoopbackDevice = false;
  // ******************** stream configuration data ***************************
  configuration.streamConfiguration.messageAllocator = &message_allocator;
  configuration.streamConfiguration.bufferSize =
   OLIMEX_MOD_MPU6050_STREAM_BUFFER_SIZE;
  configuration.streamConfiguration.useThreadPerConnection = false;
  //configuration.streamConfiguration.serializeOutput = false;
  configuration.streamConfiguration.notificationStrategy = NULL;
  configuration.streamConfiguration.module = &event_handler_module;
  configuration.streamConfiguration.deleteModule = false;
//  configuration.streamConfiguration.moduleConfiguration.streamState = NULL;
  configuration.streamConfiguration.moduleConfiguration.userData = &consoleMode_in;
  configuration.streamConfiguration.statisticReportingInterval = 0;
  configuration.streamConfiguration.printFinalReport = false;
  // ******************* protocol configuration data ***************************
  configuration.protocolConfiguration.bufferSize =
      OLIMEX_MOD_MPU6050_STREAM_BUFFER_SIZE;

  // step3: init event dispatch
  bool serialize_output;
  if (!Common_Tools::initializeEventDispatch (useReactor_in,
                                              1,
                                              serialize_output))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeEventDispatch(), returning\n")));
    return;
  } // end IF

  // step4: init client connector
  Olimex_Mod_MPU6050_IConnector_t* connector_p = NULL;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  Olimex_Mod_MPU6050_INetlinkConnector_t* netlink_connector_p = NULL;
#endif
  if (useAsynchConnector_in)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_NEW_NORETURN (connector_p,
                      Olimex_Mod_MPU6050_AsynchConnector_t (&configuration,
                                                            CONNECTIONMANAGER_SINGLETON::instance (),
                                                            OLIMEX_MOD_MPU6050_STATISTICS_REPORTING_INTERVAL));
#else
    if (clientMode_in)
      ACE_NEW_NORETURN (connector_p,
                        Olimex_Mod_MPU6050_AsynchConnector_t (&configuration,
                                                              CONNECTIONMANAGER_SINGLETON::instance (),
                                                              OLIMEX_MOD_MPU6050_STATISTICS_REPORTING_INTERVAL));
    else
      ACE_NEW_NORETURN (netlink_connector_p,
                        Olimex_Mod_MPU6050_AsynchNetlinkConnector_t (&configuration,
                                                                     NETLINK_CONNECTIONMANAGER_SINGLETON::instance (),
                                                                     OLIMEX_MOD_MPU6050_STATISTICS_REPORTING_INTERVAL));
#endif
  } // end IF
  else
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_NEW_NORETURN (connector_p,
                      Olimex_Mod_MPU6050_Connector_t (&configuration,
                                                      CONNECTIONMANAGER_SINGLETON::instance (),
                                                      OLIMEX_MOD_MPU6050_STATISTICS_REPORTING_INTERVAL));
#else
    if (clientMode_in)
      ACE_NEW_NORETURN (connector_p,
                        Olimex_Mod_MPU6050_Connector_t (&configuration,
                                                        CONNECTIONMANAGER_SINGLETON::instance (),
                                                        OLIMEX_MOD_MPU6050_STATISTICS_REPORTING_INTERVAL));
    else
      ACE_NEW_NORETURN (netlink_connector_p,
                        Olimex_Mod_MPU6050_NetlinkConnector_t (&configuration,
                                                               NETLINK_CONNECTIONMANAGER_SINGLETON::instance (),
                                                               OLIMEX_MOD_MPU6050_STATISTICS_REPORTING_INTERVAL));
#endif
  } // end ELSE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (clientMode_in && !connector_p)
#else
  if (( clientMode_in && !connector_p) ||
      (!clientMode_in && !netlink_connector_p))
#endif
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, returning\n")));
    return;
  } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (connector_p);
#else
  ACE_ASSERT (connector_p || netlink_connector_p);
#endif

  // step5: init connection manager (s)
  Net_UserData_t session_data;
  ACE_OS::memset (&session_data, 0, sizeof (session_data));
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  CONNECTIONMANAGER_SINGLETON::instance ()->initialize (std::numeric_limits<unsigned int>::max ());
  CONNECTIONMANAGER_SINGLETON::instance ()->set (configuration,
                                                 &session_data); // will be passed to all handlers
#else
  if (clientMode_in)
  {
    CONNECTIONMANAGER_SINGLETON::instance ()->initialize (std::numeric_limits<unsigned int>::max ());
    CONNECTIONMANAGER_SINGLETON::instance ()->set (configuration,
                                                   &session_data); // will be passed to all handlers
  } // end IF
  else
  {
    NETLINK_CONNECTIONMANAGER_SINGLETON::instance ()->initialize (std::numeric_limits<unsigned int>::max ());
    NETLINK_CONNECTIONMANAGER_SINGLETON::instance ()->set (configuration,
                                                           &session_data); // will be passed to all handlers
  } // end ELSE
#endif

  // step6: init signal handling
  Olimex_Mod_MPU6050_SignalHandler signal_handler (peerAddress_in,  // peer address
                                                   connector_p,     // connector
                                                   useReactor_in,   // use reactor ?
                                                   consoleMode_in); // console mode ?
  ACE_Sig_Set signal_set (0);
  do_initSignals (signal_set);
  Common_SignalActions_t previous_signal_actions;
  if (!Common_Tools::preInitializeSignals (signal_set,
                                           useReactor_in,
                                           previous_signal_actions))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::preInitializeSignals(), aborting\n")));

    // clean up
    if (clientMode_in)
      delete connector_p;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    else
      delete netlink_connector_p;
#endif

    return;
  } // end IF
  if (!Common_Tools::initializeSignals (signal_set,
                                        &signal_handler,
                                        previous_signal_actions))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize signal handling, aborting\n")));

    // clean up
    if (clientMode_in)
      delete connector_p;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    else
      delete netlink_connector_p;
#endif

    return;
  } // end IF

  // step7: start event loop(s):
  // - catch SIGINT/SIGQUIT/SIGTERM/... signals (connect / perform orderly shutdown)
  // [- signal timer expiration to perform server queries] (see above)

  // step7a: start GTK event loop ?
  Olimex_Mod_MPU6050_GTKUIDefinition interface_definition (argc_in,
                                                           argv_in,
                                                           &gtk_cb_data);
  if (!consoleMode_in)
  {
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

      // clean up
      if (clientMode_in)
        delete connector_p;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      else
        delete netlink_connector_p;
#endif

      return;
    } // end IF
  } // end IF

  // step7b: init worker(s)
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
    if (!consoleMode_in)
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    Common_Tools::finalizeSignals (signal_set,
                                   useReactor_in,
                                   previous_signal_actions);

    // clean up
    if (clientMode_in)
      delete connector_p;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    else
      delete netlink_connector_p;
#endif

    return;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("started event dispatch...\n")));

  // step8: connect
  if (clientMode_in)
    result = connector_p->connect (peerAddress_in);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  else
    result = netlink_connector_p->connect (netlinkAddress_in);
#endif
  if (!result)
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
    if (!consoleMode_in)
      COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();
    Common_Tools::finalizeSignals (signal_set,
                                   useReactor_in,
                                   previous_signal_actions);

    // clean up
    if (clientMode_in)
      delete connector_p;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    else
      delete netlink_connector_p;
#endif

    return;
  }
  // *WARNING*: from this point on, clean up any remote connections !

  // step9: dispatch events
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

  // step10: clean up
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
  if (clientMode_in)
    delete connector_p;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  else
    delete netlink_connector_p;
#endif

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

#if defined (OLINUXINO_ENABLE_VALGRIND_SUPPORT)
  if (RUNNING_ON_VALGRIND)
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("running on valgrind...\n")));
#endif

  // step1: process commandline options (if any)
  bool client_mode            = false;
  bool console_mode           = false;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  unsigned int netlink_group  = OLIMEX_MOD_MPU6050_DEFAULT_NETLINK_GROUP;
#endif
  bool log_to_file            = false;
  bool use_reactor            = OLIMEX_MOD_MPU6050_USE_REACTOR;
  ACE_INET_Addr peer_address;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  ACE_Netlink_Addr netlink_address;
#endif
  bool trace_information      = false;
  std::string interface_definition_file =
      ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_DEFINITION_FILE_NAME);
  bool print_version_and_exit = false;
  if (!do_processArguments (argc_in,
                            argv_in,
                            client_mode,
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
                            netlink_group,
#endif
                            log_to_file,
                            use_reactor,
                            peer_address,
                            trace_information,
                            interface_definition_file,
                            print_version_and_exit,
                            console_mode))
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
  bool use_asynch_connector =
   (!use_reactor ? true 
                 : OLIMEX_MOD_MPU6050_USE_ASYNCH_CONNECTOR);
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  if ((client_mode && peer_address.is_any ()) ||
      (use_reactor && use_asynch_connector) ||
      (!console_mode && !Common_File_Tools::isReadable (interface_definition_file)) ||
      (!netlink_group || (netlink_group > sizeof (unsigned int) * 8)))
#else
  if ((client_mode && peer_address.is_any ()) ||
      (use_reactor && use_asynch_connector)   ||
      (!console_mode && !Common_File_Tools::isReadable (interface_definition_file)))
#endif
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid configuration, aborting\n")));

    do_printUsage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, ACE needs finalization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  else
    if (!client_mode)
      netlink_address.set (ACE_OS::getpid (),
                           (1 << (netlink_group - 1)));
#endif

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
             client_mode,
             peer_address,
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
             netlink_address,
#endif
             use_asynch_connector,
             use_reactor,
             interface_definition_file,
             console_mode);

  // step6: clean u
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
