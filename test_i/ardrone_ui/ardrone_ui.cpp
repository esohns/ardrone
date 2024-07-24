#include "ace/Synch.h"
#include "ardrone_ui.h"

#include "ardrone_common.h"
#include "ardrone_message.h"
#include "ardrone_sessionmessage.h"

//////////////////////////////////////////

void
process_stream_events (struct ARDrone_UI_CBData_Base* CBData_in,
                       bool& finished_out)
{
  STREAM_TRACE (ACE_TEXT ("::process_stream_events"));

  // initialize return value(s)
  finished_out = false;

  // sanity check(s)
  ACE_ASSERT (CBData_in);
  wxAppConsole* wx_app_console_p = wxApp::GetInstance ();
  ACE_ASSERT (wx_app_console_p);

  struct ARDrone_Configuration_Base* configuration_base_p = NULL;
  enum Stream_Visualization_VideoRenderer renderer_e =
    STREAM_VISUALIZATION_VIDEORENDERER_INVALID;
  struct ARDrone_UI_wxWidgets_State* ui_state_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_UI_CBData* directshow_cb_data_p = NULL;
  struct ARDrone_MediaFoundation_UI_CBData* mediafoundation_cb_data_p =
    NULL;
  struct Stream_MediaFramework_Direct3D_Configuration* direct3DConfiguration_p =
    NULL;
  ARDrone_DirectShow_WxWidgetsIApplication_t* directshow_iapplication_p = NULL;
  ARDrone_MediaFoundation_WxWidgetsIApplication_t* mediafoundation_iapplication_p =
    NULL;
  switch (CBData_in->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_UI_CBData*> (CBData_in);
      configuration_base_p = directshow_cb_data_p->configuration;
      direct3DConfiguration_p =
        &directshow_cb_data_p->configuration->direct3DConfiguration;

      ARDrone_DirectShow_StreamConfigurationsIterator_t streams_iterator =
        directshow_cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (streams_iterator != directshow_cb_data_p->configuration->streamConfigurations.end ());
      renderer_e = (*streams_iterator).second.configuration_.renderer;

      directshow_iapplication_p =
        dynamic_cast<ARDrone_DirectShow_WxWidgetsIApplication_t*> (wx_app_console_p);
      ACE_ASSERT (directshow_iapplication_p);
      ui_state_p =
        &const_cast<struct ARDrone_UI_wxWidgets_State&> (directshow_iapplication_p->getR ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_UI_CBData*> (CBData_in);
      configuration_base_p = mediafoundation_cb_data_p->configuration;
      direct3DConfiguration_p =
        &mediafoundation_cb_data_p->configuration->direct3DConfiguration;

      ARDrone_MediaFoundation_StreamConfigurationsIterator_t streams_iterator =
        mediafoundation_cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (streams_iterator != mediafoundation_cb_data_p->configuration->streamConfigurations.end ());
      renderer_e = (*streams_iterator).second.configuration_.renderer;

      mediafoundation_iapplication_p =
        dynamic_cast<ARDrone_MediaFoundation_WxWidgetsIApplication_t*> (wx_app_console_p);
      ACE_ASSERT (mediafoundation_iapplication_p);
      ui_state_p =
        &const_cast<struct ARDrone_UI_wxWidgets_State&> (mediafoundation_iapplication_p->getR ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  CBData_in->mediaFramework));
      return;
    }
  } // end SWITCH
#else
  struct ARDrone_UI_CBData* cb_data_p =
    static_cast<struct ARDrone_UI_CBData*> (CBData_in);
  configuration_base_p = cb_data_p->configuration;

  ARDrone_StreamConfigurationsIterator_t streams_iterator =
    cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != cb_data_p->configuration->streamConfigurations.end ());
  renderer_e = (*streams_iterator).second.configuration_.renderer;

  ARDrone_WxWidgetsIApplication_t* directshow_iapplication_p =
    dynamic_cast<ARDrone_WxWidgetsIApplication_t*> (wx_app_console_p);
  ACE_ASSERT (iapplication_p);
  ui_state_p =
    &const_cast<struct ARDrone_UI_wxWidgets_State&> (iapplication_p->getR ());
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (configuration_base_p);
  ACE_ASSERT (ui_state_p);

  ARDrone_Event_t* event_p = NULL;
  ARDrone_Event_t event_2;
  Common_UI_wxWidgets_XmlResourcesIterator_t iterator;
  wxDialog* dialog_p = NULL;
  wxSpinCtrl* spin_control_p = NULL;
  int result = -1;

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, ui_state_p->lock);
    iterator =
      ui_state_p->resources.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
    ACE_ASSERT (iterator != ui_state_p->resources.end ());
    dialog_p = dynamic_cast<wxDialog*> ((*iterator).second.second);
    ACE_ASSERT (dialog_p);

    for (ARDrone_Events_t::ITERATOR iterator_2 (ui_state_p->eventStack);
         iterator_2.next (event_p);
         iterator_2.advance ())
    { ACE_ASSERT (event_p);
      switch (event_p->second)
      {
        case COMMON_UI_EVENT_STARTED:
        {
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_messages_session"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (1);
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_messages_data"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (0);
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_data"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (0);
          break;
        }
        case COMMON_UI_EVENT_DATA:
        {
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_messages_data"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (spin_control_p->GetValue () + 1);
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_data"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (CBData_in->progressData.statistic.bytes);
          break;
        }
        case COMMON_UI_EVENT_FINISHED:
        {
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_messages_session"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (spin_control_p->GetValue () + 1);

          wxToggleButton* toggle_button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("togglebutton_record"),
                     wxToggleButton);
          ACE_ASSERT (toggle_button_p);
          toggle_button_p->Enable (true);
          wxBitmapButton* button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("button_snapshot"),
                     wxBitmapButton);
          ACE_ASSERT (button_p);
          button_p->Enable (false);
          button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("button_cut"),
                     wxBitmapButton);
          ACE_ASSERT (button_p);
          button_p->Enable (false);
#if defined (_DEBUG)
          button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("button_report"),
                     wxBitmapButton);
          ACE_ASSERT (button_p);
          button_p->Enable (false);
#endif // _DEBUG

          wxChoice* choice_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("choice_source"),
                     wxChoice);
          ACE_ASSERT (choice_p);
          choice_p->Enable (true);
          choice_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("choice_format"),
                     wxChoice);
          ACE_ASSERT (choice_p);
          choice_p->Enable (true);
          choice_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("choice_resolution"),
                     wxChoice);
          ACE_ASSERT (choice_p);
          choice_p->Enable (true);
          choice_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("choice_framerate"),
                     wxChoice);
          ACE_ASSERT (choice_p);
          choice_p->Enable (true);
          button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("button_reset_format"),
                     wxBitmapButton);
          ACE_ASSERT (button_p);
          button_p->Enable (true);

          toggle_button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("togglebutton_save"),
                     wxToggleButton);
          ACE_ASSERT (toggle_button_p);
          toggle_button_p->Enable (true);
          wxTextCtrl* text_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("textcontrol_filename"),
                     wxTextCtrl);
          ACE_ASSERT (text_p);
          text_p->Enable (true);
          wxDirPickerCtrl* picker_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("directorypicker_save"),
                     wxDirPickerCtrl);
          ACE_ASSERT (picker_p);
          picker_p->Enable (true);

          toggle_button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("togglebutton_display"),
                     wxToggleButton);
          ACE_ASSERT (toggle_button_p);
          toggle_button_p->Enable (true);
          // *NOTE*: the stream will reset the device to 'desktop' mode, if that
          //         was the original setting; reset the control accordingly
          toggle_button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("togglebutton_fullscreen"),
                     wxToggleButton);
          ACE_ASSERT (toggle_button_p);
          switch (renderer_e)
          {
            case STREAM_VISUALIZATION_VIDEORENDERER_NULL:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
            case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
              ACE_ASSERT (false); // *TODO*
              ACE_NOTSUP;
              ACE_NOTREACHED (break;)
            case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
              ACE_ASSERT (direct3DConfiguration_p);
              toggle_button_p->SetValue (!direct3DConfiguration_p->presentationParameters.Windowed);
              break;
            case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
            case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
            case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
              ACE_ASSERT (false); // *TODO*
              ACE_NOTSUP;
              ACE_NOTREACHED (break;)
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GTK_SUPPORT)
            case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
            case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
              ACE_ASSERT (false); // *TODO*
              ACE_NOTSUP;
              ACE_NOTREACHED (break;)
#endif // GTK_SUPPORT
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown video renderer (was: %d), returning\n"),
                          renderer_e));
              return;
            }
          } // end SWITCH
          toggle_button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("togglebutton_display"),
                     wxToggleButton);
          ACE_ASSERT (toggle_button_p);
          choice_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("choice_display"),
                     wxChoice);
          ACE_ASSERT (choice_p);
          choice_p->Enable (toggle_button_p->GetValue ());

          wxGauge* gauge_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("gauge_progress"),
                     wxGauge);
          ACE_ASSERT (gauge_p);
          gauge_p->SetValue (0);
          gauge_p->Enable (false);

          finished_out = true;

          break;
        }
        case COMMON_UI_EVENT_RESET:
        {
          switch (renderer_e)
          {
            case STREAM_VISUALIZATION_VIDEORENDERER_NULL:
              break;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
            case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
              break;
            case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
            {
              ACE_ASSERT (direct3DConfiguration_p);
              ACE_ASSERT (direct3DConfiguration_p->handle);
              if (!Stream_MediaFramework_DirectDraw_Tools::reset (direct3DConfiguration_p->handle,
                                                                  *direct3DConfiguration_p))
              {
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("failed to Stream_MediaFramework_DirectDraw_Tools::reset(), returning\n")));
                return;
              } // end IF
              break;
            }
            case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
            case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
            case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
              break;
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GTK_SUPPORT)
            case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
            case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
              break;
#endif // GTK_SUPPORT
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown video renderer (was: %d), returning\n"),
                          renderer_e));
              return;
            }
          } // end SWITCH
          break;
        }
        case COMMON_UI_EVENT_STATISTIC:
        {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_frames_captured"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (CBData_in->progressData.statistic.capturedFrames);
#endif // ACE_WIN32 || ACE_WIN64
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_frames_dropped"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (CBData_in->progressData.statistic.droppedFrames);
          spin_control_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("spincontrol_messages_session"),
                     wxSpinCtrl);
          ACE_ASSERT (spin_control_p);
          spin_control_p->SetValue (spin_control_p->GetValue () + 1);
          break;
        }
        //////////////////////////////////
        case NET_WLAN_EVENT_INTERFACE_HOTPLUG:
        case NET_WLAN_EVENT_INTERFACE_REMOVE:
          break;
        case NET_WLAN_EVENT_SCAN_COMPLETE:
          break;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#else
        case NET_WLAN_EVENT_ASSOCIATE:
        case NET_WLAN_EVENT_DISASSOCIATE:
          break;
//#endif // ACE_WIN32 || ACE_WIN64
        case NET_WLAN_EVENT_CONNECT:
        {
          wxToggleButton* toggle_button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("togglebutton_connect"),
                     wxToggleButton);
          ACE_ASSERT (toggle_button_p);
          ARDrone_WLANMonitor_t* WLAN_monitor_p =
            ARDRONE_WLANMONITOR_SINGLETON::instance ();
          ACE_ASSERT (WLAN_monitor_p);
          toggle_button_p->Enable (!ACE_OS::strcmp (WLAN_monitor_p->SSID ().c_str (),
                                                    configuration_base_p->WLANMonitorConfiguration.SSID.c_str ()));
          break;
        }
        case NET_WLAN_EVENT_DISCONNECT:
        {
          wxToggleButton* toggle_button_p =
            XRCCTRL (*dialog_p,
                     ACE_TEXT_ALWAYS_CHAR ("togglebutton_connect"),
                     wxToggleButton);
          ACE_ASSERT (toggle_button_p);
          ARDrone_WLANMonitor_t* WLAN_monitor_p =
            ARDRONE_WLANMONITOR_SINGLETON::instance ();
          ACE_ASSERT (WLAN_monitor_p);
          if (!ACE_OS::strcmp (WLAN_monitor_p->SSID ().c_str (),
                               configuration_base_p->WLANMonitorConfiguration.SSID.c_str ()))
          {
            toggle_button_p->Enable (false);
            toggle_button_p->SetValue (false);
          } // end IF
          break;
        }
        case NET_WLAN_EVENT_SIGNAL_QUALITY_CHANGED:
          break;
        //////////////////////////////////
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
                      *event_p));
          break;
        }
      } // end SWITCH
      event_p = NULL;
    } // end FOR

    // clean up
    while (!ui_state_p->eventStack.is_empty ())
    {
      result = ui_state_p->eventStack.pop (event_2);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Unbounded_Stack::pop(): \"%m\", continuing\n")));
    } // end WHILE
  } // end lock scope
}

ACE_THR_FUNC_RETURN
stream_processing_thread (void* arg_in)
{
  STREAM_TRACE (ACE_TEXT ("::stream_processing_thread"));

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("stream processing thread (id: %t) starting...\n")));
#endif // _DEBUG

  ACE_THR_FUNC_RETURN result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = std::numeric_limits<unsigned long>::max ();
#else
  result = arg_in;
#endif // ACE_WIN32 || ACE_WIN64

  // sanity check(s)
  struct ARDrone_ThreadData* thread_data_p =
      static_cast<struct ARDrone_ThreadData*> (arg_in);
  ACE_ASSERT (thread_data_p);
  ACE_ASSERT (thread_data_p->CBData);
  wxAppConsole* wx_app_console_p = wxApp::GetInstance ();
  ACE_ASSERT (wx_app_console_p);

  Common_UI_wxWidgets_XmlResourcesIterator_t iterator;
  wxDialog* dialog_p = NULL;
  wxToggleButton* toggle_button_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  Common_IGetR_T<ARDrone_DirectShow_SessionData_t>* iget_p = NULL;
  const ARDrone_DirectShow_SessionData_t* directshow_session_data_container_p =
    NULL;
  const ARDrone_DirectShow_SessionData* directshow_session_data_p = NULL;
#else
  Common_IGetR_T<ARDrone_SessionData_t>* iget_p = NULL;
  const ARDrone_SessionData_t* session_data_container_p = NULL;
  const ARDrone_SessionData* session_data_p = NULL;
#endif // ACE_WIN32 || ACE_WIN64
  ARDrone_IController* icontroller_p = NULL;
  //ACE_Time_Value session_start_timeout =
  //    COMMON_TIME_NOW + ACE_Time_Value (3, 0);
  Common_IGetP_T<ARDrone_IController>* iget_2 = NULL;
  ARDrone_StreamsIterator_t streams_iterator;
  Stream_IStreamControlBase* istream_base_p = NULL;
  std::vector<Stream_IStreamControlBase*> streams_a;
  struct ARDrone_UI_wxWidgets_State* ui_state_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_UI_CBData* directshow_cb_data_p = NULL;
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_iterator;
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_2;

  struct ARDrone_MediaFoundation_UI_CBData* mediafoundation_cb_data_p =
    NULL;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_iterator;
  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_2;

  ARDrone_DirectShow_WxWidgetsIApplication_t* directshow_iapplication_p = NULL;
  ARDrone_MediaFoundation_WxWidgetsIApplication_t* mediafoundation_iapplication_p =
    NULL;
  switch (thread_data_p->CBData->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_UI_CBData*> (thread_data_p->CBData);
      ACE_ASSERT (directshow_cb_data_p->configuration);

      directshow_iapplication_p =
        dynamic_cast<ARDrone_DirectShow_WxWidgetsIApplication_t*> (wx_app_console_p);
      ACE_ASSERT (directshow_iapplication_p);
      ui_state_p =
        &const_cast<struct ARDrone_UI_wxWidgets_State&> (directshow_iapplication_p->getR ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_UI_CBData*> (thread_data_p->CBData);
      ACE_ASSERT (mediafoundation_cb_data_p->configuration);

      mediafoundation_iapplication_p =
        dynamic_cast<ARDrone_MediaFoundation_WxWidgetsIApplication_t*> (wx_app_console_p);
      ACE_ASSERT (mediafoundation_iapplication_p);
      ui_state_p =
        &const_cast<struct ARDrone_UI_wxWidgets_State&> (mediafoundation_iapplication_p->getR ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  thread_data_p->CBData->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  struct ARDrone_UI_CBData* cb_data_p =
    static_cast<struct ARDrone_UI_CBData*> (thread_data_p->CBData);
  ACE_ASSERT (cb_data_p->configuration);

  ARDrone_StreamConfigurationsIterator_t iterator;
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_2;

  iapplication_p =
    dynamic_cast<ARDrone_WxWidgetsIApplication_t*> (wx_app_console_p);
  ACE_ASSERT (iapplication_p);
  ui_state_p =
    &const_cast<struct ARDrone_UI_wxWidgets_State&> (iapplication_p->getR ());
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (ui_state_p);

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, ui_state_p->lock, result);
    iterator =
      ui_state_p->resources.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
    ACE_ASSERT (iterator != ui_state_p->resources.end ());
    dialog_p = dynamic_cast<wxDialog*> ((*iterator).second.second);
  } // end lock scope
  ACE_ASSERT (dialog_p);
  toggle_button_p =
    XRCCTRL (*dialog_p,
             ACE_TEXT_ALWAYS_CHAR ("togglebutton_display_video"),
             wxToggleButton);
  ACE_ASSERT (toggle_button_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (thread_data_p->CBData->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    { 
      // *IMPORTANT NOTE*: the control stream initialization needs a handle to
      //                   the controller module of the navdata stream
      //                   --> initialize that first
      // navdata
      streams_iterator =
          directshow_cb_data_p->streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
      ACE_ASSERT (streams_iterator != directshow_cb_data_p->streams.end ());
      iget_2 =
        dynamic_cast<Common_IGetP_T<ARDrone_IController>*> ((*streams_iterator).second);
      ACE_ASSERT (iget_2);
      icontroller_p = const_cast<ARDrone_IController*> (iget_2->getP ());
      ACE_ASSERT (icontroller_p);

      ARDrone_DirectShow_NavDataStream_t::IINITIALIZE_T* iinitialize_2 =
        dynamic_cast<ARDrone_DirectShow_NavDataStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
      ACE_ASSERT (iinitialize_2);
      directshow_iterator =
        directshow_cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator != directshow_cb_data_p->configuration->streamConfigurations.end ());
      if (!iinitialize_2->initialize ((*directshow_iterator).second))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize %s, aborting\n"),
                    ACE_TEXT (ARDRONE_NAVDATA_STREAM_NAME_STRING)));
        goto error;
      } // end IF
      iget_p =
        dynamic_cast<Common_IGetR_T<ARDrone_DirectShow_SessionData_t>*> ((*streams_iterator).second);
      ACE_ASSERT (iget_p);
      directshow_session_data_container_p =
        &const_cast<ARDrone_DirectShow_SessionData_t&> (iget_p->getR ());
      directshow_session_data_p =
        &const_cast<ARDrone_DirectShow_SessionData&> (directshow_session_data_container_p->getR ());
      thread_data_p->sessionId = directshow_session_data_p->sessionId;

      istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
      ACE_ASSERT (istream_base_p);
      streams_a.push_back (istream_base_p);

      // control
      streams_iterator =
        directshow_cb_data_p->streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
      ACE_ASSERT (streams_iterator != directshow_cb_data_p->streams.end ());

      ARDrone_DirectShow_ControlStream_t::IINITIALIZE_T* iinitialize_p =
        dynamic_cast<ARDrone_DirectShow_ControlStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
      ACE_ASSERT (iinitialize_p);
      directshow_iterator =
        directshow_cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator != directshow_cb_data_p->configuration->streamConfigurations.end ());
      ACE_ASSERT (!(*directshow_iterator).second.configuration_.deviceConfiguration);
      (*directshow_iterator).second.configuration_.deviceConfiguration =
        icontroller_p;
      if (!iinitialize_p->initialize ((*directshow_iterator).second))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize %s, aborting\n"),
                    ACE_TEXT (ARDRONE_CONTROL_STREAM_NAME_STRING)));
        goto error;
      } // end IF
      istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
      ACE_ASSERT (istream_base_p);
      streams_a.push_back (istream_base_p);

      // mavlink
      streams_iterator =
        directshow_cb_data_p->streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
      ACE_ASSERT (streams_iterator != directshow_cb_data_p->streams.end ());

      ARDrone_DirectShow_MAVLinkStream_t::IINITIALIZE_T* iinitialize_3 =
        dynamic_cast<ARDrone_DirectShow_MAVLinkStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
      ACE_ASSERT (iinitialize_3);
      directshow_iterator =
        directshow_cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator != directshow_cb_data_p->configuration->streamConfigurations.end ());
      //directshow_iterator_2 =
      //  const_cast<ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T&> ((*directshow_iterator).second.find (ACE_TEXT_ALWAYS_CHAR ("")));
      //ACE_ASSERT (directshow_iterator_2 != (*directshow_iterator).second.end ());
      if (!iinitialize_3->initialize ((*directshow_iterator).second))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize %s, aborting\n"),
                    ACE_TEXT (ARDRONE_MAVLINK_STREAM_NAME_STRING)));
        goto error;
      } // end IF
      istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
      ACE_ASSERT (istream_base_p);
      streams_a.push_back (istream_base_p);

      if (!toggle_button_p->GetValue ())
        goto continue_;

      // video
      streams_iterator =
        directshow_cb_data_p->streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (streams_iterator != directshow_cb_data_p->streams.end ());

      ARDrone_DirectShow_VideoStream_t::IINITIALIZE_T* iinitialize_4 =
        dynamic_cast<ARDrone_DirectShow_VideoStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
      ACE_ASSERT (iinitialize_4);
      directshow_iterator =
        directshow_cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator != directshow_cb_data_p->configuration->streamConfigurations.end ());
      //directshow_iterator_2 =
      //  const_cast<ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T&> ((*directshow_iterator).second.find (ACE_TEXT_ALWAYS_CHAR ("")));
      //ACE_ASSERT (directshow_iterator_2 != (*directshow_iterator).second.end ());
      if (!iinitialize_4->initialize ((*directshow_iterator).second))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to initialize %s, aborting\n"),
                    ACE_TEXT (ARDRONE_VIDEO_STREAM_NAME_STRING)));
        goto error;
      } // end IF
      istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
      ACE_ASSERT (istream_base_p);
      streams_a.push_back (istream_base_p);

continue_:
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      //if (!mediafoundation_cb_data_p->stream->initialize (mediafoundation_cb_data_p->configuration->streamConfiguration))
      //{
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("failed to initialize stream, aborting\n")));
      //  goto error;
      //} // end IF
      //stream_p = mediafoundation_cb_data_p->stream;
      //session_data_container_p = &mediafoundation_cb_data_p->stream->getR ();
      //ACE_ASSERT (session_data_container_p);
      //session_data_p = &session_data_container_p->getR ();
      //thread_data_p->sessionId = session_data_p->sessionId;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  thread_data_p->CBData->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  //if (!cb_data_p->stream->initialize (cb_data_p->configuration->streamConfiguration))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to ARDrone_Stream::initialize(), aborting\n")));
  //  goto error;
  //} // end IF
  //stream_p = cb_data_p->stream;
  //session_data_container_p = &cb_data_p->stream->getR ();
  ACE_ASSERT (session_data_container_p);
  session_data_p = &session_data_container_p->getR ();
  thread_data_p->sessionId = session_data_p->sessionId;
#endif // ACE_WIN32 || ACE_WIN64

  for (std::vector<Stream_IStreamControlBase*>::iterator iterator = streams_a.begin ();
       iterator != streams_a.end ();
       ++iterator)
    (*iterator)->start ();
  // *NOTE*: blocks until 'finished'
  for (std::vector<Stream_IStreamControlBase*>::iterator iterator = streams_a.begin ();
       iterator != streams_a.end ();
       ++iterator)
    (*iterator)->wait (true,
                       false,
                       false);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = 0;
#else
  result = NULL;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, ui_state_p->lock, -1);
#else
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, ui_state_p->lock, std::numeric_limits<void*>::max ());
#endif // ACE_WIN32 || ACE_WIN64
    thread_data_p->CBData->progressData.completedActions.insert (thread_data_p->sessionId);
  } // end lock scope

error:
  // clean up
  delete thread_data_p; thread_data_p = NULL;

  return result;
}

/////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//wxIMPLEMENT_DYNAMIC_CLASS (ARDrone_DirectShow_WxWidgetsDialog_t, dialog_main)
wxClassInfo
ARDrone_DirectShow_WxWidgetsDialog_t::ms_classInfo (L"ARDrone_DirectShow_WxWidgetsDialog_t",
                                                    &dialog_main::ms_classInfo,
                                                    NULL,
                                                    (int) sizeof (ARDrone_DirectShow_WxWidgetsDialog_t),
                                                    ARDrone_DirectShow_WxWidgetsDialog_t::wxCreateObject);
wxClassInfo*
ARDrone_DirectShow_WxWidgetsDialog_t::GetClassInfo () const
{
  return &ARDrone_DirectShow_WxWidgetsDialog_t::ms_classInfo;
}

wxObject*
ARDrone_DirectShow_WxWidgetsDialog_t::wxCreateObject ()
{
  return new ARDrone_DirectShow_WxWidgetsDialog_t;
}

//wxIMPLEMENT_DYNAMIC_CLASS (ARDrone_MediaFoundation_WxWidgetsDialog_t, dialog_main)
wxClassInfo
ARDrone_MediaFoundation_WxWidgetsDialog_t::ms_classInfo (L"ARDrone_MediaFoundation_WxWidgetsDialog_t",
                                                         &dialog_main::ms_classInfo,
                                                         NULL,
                                                         (int) sizeof (ARDrone_MediaFoundation_WxWidgetsDialog_t),
                                                         ARDrone_MediaFoundation_WxWidgetsDialog_t::wxCreateObject);
wxClassInfo*
ARDrone_MediaFoundation_WxWidgetsDialog_t::GetClassInfo () const
{
  return &ARDrone_MediaFoundation_WxWidgetsDialog_t::ms_classInfo;
}

wxObject*
ARDrone_MediaFoundation_WxWidgetsDialog_t::wxCreateObject ()
{
  return new ARDrone_MediaFoundation_WxWidgetsDialog_t;
}

wxBEGIN_EVENT_TABLE (ARDrone_DirectShow_WxWidgetsDialog_t, dialog_main)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_connect"), ARDrone_DirectShow_WxWidgetsDialog_t::togglebutton_connect_toggled_cb)
 EVT_BUTTON (XRCID ("button_calibrate"), ARDrone_DirectShow_WxWidgetsDialog_t::button_calibrate_clicked_cb)
 EVT_BUTTON (XRCID ("button_animate_leds"), ARDrone_DirectShow_WxWidgetsDialog_t::button_animate_LEDs_clicked_cb)
#if defined (_DEBUG)
 EVT_BUTTON (XRCID ("button_report"), ARDrone_DirectShow_WxWidgetsDialog_t::button_report_clicked_cb)
#endif // _DEBUG
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_associate"), ARDrone_DirectShow_WxWidgetsDialog_t::togglebutton_associate_toggled_cb)
 EVT_CHOICE (XRCID ("choice_interface"), ARDrone_DirectShow_WxWidgetsDialog_t::choice_interface_selected_cb)
 EVT_SPINCTRL (XRCID ("spincontrol_port"), ARDrone_DirectShow_WxWidgetsDialog_t::spincontrol_port_changed_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_display"), ARDrone_DirectShow_WxWidgetsDialog_t::togglebutton_display_toggled_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_fullscreen"), ARDrone_DirectShow_WxWidgetsDialog_t::togglebutton_fullscreen_toggled_cb)
 EVT_BUTTON (XRCID ("button_switch_camera"), ARDrone_DirectShow_WxWidgetsDialog_t::button_switch_camera_clicked_cb)
 EVT_CHOICE (XRCID ("choice_display_video_format"), ARDrone_DirectShow_WxWidgetsDialog_t::choice_display_video_format_selected_cb)
 EVT_CHOICE (XRCID ("choice_adapter"), ARDrone_DirectShow_WxWidgetsDialog_t::choice_adapter_selected_cb)
 EVT_CHOICE (XRCID ("choice_display"), ARDrone_DirectShow_WxWidgetsDialog_t::choice_display_selected_cb)
 EVT_BUTTON (XRCID ("button_display_settings"), ARDrone_DirectShow_WxWidgetsDialog_t::button_display_settings_clicked_cb)
 EVT_CHOICE (XRCID ("choice_display_resolution"), ARDrone_DirectShow_WxWidgetsDialog_t::choice_display_resolution_selected_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_save_video"), ARDrone_DirectShow_WxWidgetsDialog_t::togglebutton_save_video_toggled_cb)
 EVT_CHOICE (XRCID ("choice_save_video_format"), ARDrone_DirectShow_WxWidgetsDialog_t::choice_save_video_format_selected_cb)
 EVT_BUTTON (XRCID ("button_snapshot"), ARDrone_DirectShow_WxWidgetsDialog_t::button_snapshot_clicked_cb)
 EVT_DIRPICKER_CHANGED (XRCID ("directorypicker_save_video"), ARDrone_DirectShow_WxWidgetsDialog_t::directorypicker_save_video_changed_cb)
 EVT_BUTTON (XRCID ("button_about"), ARDrone_DirectShow_WxWidgetsDialog_t::button_about_clicked_cb)
 EVT_BUTTON (XRCID ("button_quit"), ARDrone_DirectShow_WxWidgetsDialog_t::button_quit_clicked_cb)
 EVT_IDLE (ARDrone_DirectShow_WxWidgetsDialog_t::dialog_main_idle_cb)
 EVT_CHAR_HOOK (ARDrone_DirectShow_WxWidgetsDialog_t::dialog_main_keydown_cb)
wxEND_EVENT_TABLE ()

wxBEGIN_EVENT_TABLE (ARDrone_MediaFoundation_WxWidgetsDialog_t, dialog_main)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_connect"), ARDrone_MediaFoundation_WxWidgetsDialog_t::togglebutton_connect_toggled_cb)
 EVT_BUTTON (XRCID ("button_calibrate"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_calibrate_clicked_cb)
 EVT_BUTTON (XRCID ("button_animate_leds"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_animate_LEDs_clicked_cb)
#if defined (_DEBUG)
 EVT_BUTTON (XRCID ("button_report"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_report_clicked_cb)
#endif // _DEBUG
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_associate"), ARDrone_MediaFoundation_WxWidgetsDialog_t::togglebutton_associate_toggled_cb)
 EVT_CHOICE (XRCID ("choice_interface"), ARDrone_MediaFoundation_WxWidgetsDialog_t::choice_interface_selected_cb)
 EVT_SPINCTRL (XRCID ("spincontrol_port"), ARDrone_MediaFoundation_WxWidgetsDialog_t::spincontrol_port_changed_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_display"), ARDrone_MediaFoundation_WxWidgetsDialog_t::togglebutton_display_toggled_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_fullscreen"), ARDrone_MediaFoundation_WxWidgetsDialog_t::togglebutton_fullscreen_toggled_cb)
 EVT_BUTTON (XRCID ("button_switch_camera"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_switch_camera_clicked_cb)
 EVT_CHOICE (XRCID ("choice_display_video_format"), ARDrone_MediaFoundation_WxWidgetsDialog_t::choice_display_video_format_selected_cb)
 EVT_CHOICE (XRCID ("choice_adapter"), ARDrone_MediaFoundation_WxWidgetsDialog_t::choice_adapter_selected_cb)
 EVT_CHOICE (XRCID ("choice_display"), ARDrone_MediaFoundation_WxWidgetsDialog_t::choice_display_selected_cb)
 EVT_BUTTON (XRCID ("button_display_settings"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_display_settings_clicked_cb)
 EVT_CHOICE (XRCID ("choice_display_resolution"), ARDrone_MediaFoundation_WxWidgetsDialog_t::choice_display_resolution_selected_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_save_video"), ARDrone_MediaFoundation_WxWidgetsDialog_t::togglebutton_save_video_toggled_cb)
 EVT_CHOICE (XRCID ("choice_save_video_format"), ARDrone_MediaFoundation_WxWidgetsDialog_t::choice_save_video_format_selected_cb)
 EVT_BUTTON (XRCID ("button_snapshot"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_snapshot_clicked_cb)
 EVT_DIRPICKER_CHANGED (XRCID ("directorypicker_save_video"), ARDrone_MediaFoundation_WxWidgetsDialog_t::directorypicker_save_video_changed_cb)
 EVT_BUTTON (XRCID ("button_about"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_about_clicked_cb)
 EVT_BUTTON (XRCID ("button_quit"), ARDrone_MediaFoundation_WxWidgetsDialog_t::button_quit_clicked_cb)
 EVT_IDLE (ARDrone_MediaFoundation_WxWidgetsDialog_t::dialog_main_idle_cb)
 EVT_CHAR_HOOK (ARDrone_MediaFoundation_WxWidgetsDialog_t::dialog_main_keydown_cb)
wxEND_EVENT_TABLE ()
#else
//wxIMPLEMENT_DYNAMIC_CLASS (ARDrone_V4L_WxWidgetsDialog_t, dialog_main)
wxClassInfo
ARDrone_V4L_WxWidgetsDialog_t::ms_classInfo (L"ARDrone_V4L_WxWidgetsDialog_t",
                                             &dialog_main::ms_classInfo,
                                             NULL,
                                             (int) sizeof (ARDrone_V4L_WxWidgetsDialog_t),
                                             ARDrone_V4L_WxWidgetsDialog_t::wxCreateObject);
wxClassInfo*
ARDrone_V4L_WxWidgetsDialog_t::GetClassInfo () const
{
  return &ARDrone_V4L_WxWidgetsDialog_t::ms_classInfo;
}

template <>
wxObject*
ARDrone_V4L_WxWidgetsDialog_t::wxCreateObject ()
{
  return new ARDrone_V4L_WxWidgetsDialog_t;
}

wxBEGIN_EVENT_TABLE (ARDrone_V4L_WxWidgetsDialog_t, dialog_main)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_record"), ARDrone_V4L_WxWidgetsDialog_t::togglebutton_record_toggled_cb)
 EVT_BUTTON (XRCID ("button_snapshot"), ARDrone_V4L_WxWidgetsDialog_t::button_snapshot_click_cb)
 EVT_BUTTON (XRCID ("button_cut"), ARDrone_V4L_WxWidgetsDialog_t::button_cut_click_cb)
#if defined (_DEBUG)
 EVT_BUTTON (XRCID ("button_report"), ARDrone_V4L_WxWidgetsDialog_t::button_report_click_cb)
#endif // _DEBUG
 EVT_CHOICE (XRCID ("choice_source"), ARDrone_V4L_WxWidgetsDialog_t::choice_source_selected_cb)
 EVT_BUTTON (XRCID ("button_hardware_settings"), ARDrone_V4L_WxWidgetsDialog_t::button_hardware_settings_click_cb)
 EVT_CHOICE (XRCID ("choice_format"), ARDrone_V4L_WxWidgetsDialog_t::choice_format_selected_cb)
 EVT_CHOICE (XRCID ("choice_resolution"), ARDrone_V4L_WxWidgetsDialog_t::choice_resolution_selected_cb)
 EVT_CHOICE (XRCID ("choice_framerate"), ARDrone_V4L_WxWidgetsDialog_t::choice_framerate_selected_cb)
 EVT_BUTTON (XRCID ("button_reset_format"), ARDrone_V4L_WxWidgetsDialog_t::button_reset_format_click_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_save"), ARDrone_V4L_WxWidgetsDialog_t::togglebutton_save_toggled_cb)
 EVT_DIRPICKER_CHANGED (XRCID ("directorypicker_save"), ARDrone_V4L_WxWidgetsDialog_t::picker_directory_save_changed_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_display"), ARDrone_V4L_WxWidgetsDialog_t::togglebutton_display_toggled_cb)
 EVT_TOGGLEBUTTON (XRCID ("togglebutton_fullscreen"), ARDrone_V4L_WxWidgetsDialog_t::togglebutton_fullscreen_toggled_cb)
 EVT_CHOICE (XRCID ("choice_adapter"), ARDrone_V4L_WxWidgetsDialog_t::choice_adapter_selected_cb)
 EVT_CHOICE (XRCID ("choice_display"), ARDrone_V4L_WxWidgetsDialog_t::choice_display_selected_cb)
 EVT_BUTTON (XRCID ("button_display_settings"), ARDrone_V4L_WxWidgetsDialog_t::button_display_settings_click_cb)
 EVT_CHOICE (XRCID ("choice_resolution_2"), ARDrone_V4L_WxWidgetsDialog_t::choice_resolution_2_selected_cb)
 EVT_BUTTON (XRCID ("button_about"), ARDrone_V4L_WxWidgetsDialog_t::button_about_click_cb)
 EVT_BUTTON (XRCID ("button_quit"), ARDrone_V4L_WxWidgetsDialog_t::button_quit_click_cb)
 EVT_IDLE (ARDrone_V4L_WxWidgetsDialog_t::dialog_main_idle_cb)
 EVT_CHAR_HOOK (ARDrone_V4L_WxWidgetsDialog_t::dialog_main_keydown_cb)
wxEND_EVENT_TABLE ()
#endif // ACE_WIN32 || ACE_WIN64
