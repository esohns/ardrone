#if defined (ACE_WIN64) || defined (ACE_WIN32)
#include <OleCtl.h>
// *TODO*: find a way to include uuids.h here
#if defined (UUIDS_H)
#else
#ifndef OUR_GUID_ENTRY
    #define OUR_GUID_ENTRY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
#endif
OUR_GUID_ENTRY(CLSID_VideoInputDeviceCategory,
0x860BB310,0x5D01,0x11d0,0xBD,0x3B,0x00,0xA0,0xC9,0x11,0xCE,0x86)
#define UUIDS_H
//#include <uuids.h>
#endif // UUIDS_H
#endif // ACE_WIN64 || ACE_WIN32

#include "wx/aboutdlg.h"

#include "ace/Date_Time.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_Thread.h"

#include "common_file_tools.h"
#include "common_tools.h"

#include "common_ui_tools.h"

#include "common_ui_wxwidgets_tools.h"

#include "common_test_u_tools.h"

#if defined (HAVE_CONFIG_H)
#include "ACEStream_config.h"
#endif // HAVE_CONFIG_H
#include "stream_macros.h"

#if defined (ACE_WIN64) || defined (ACE_WIN32)
#include "stream_dev_directshow_tools.h"
#endif // ACE_WIN64 || ACE_WIN32

#include "stream_vis_tools.h"

#include "net_common_tools.h"

#include "test_u_defines.h"

#include "ardrone_network_common.h"
#include "ardrone_stream.h"

template <typename InterfaceType>
ARDrone_WxWidgetsDialog_T<InterfaceType>::ARDrone_WxWidgetsDialog_T (wxWindow* parent_in)
 : inherited (parent_in)
 , application_ (NULL)
 , initializing_ (true)
 , reset_ (false)
 , untoggling_ (false)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::ARDrone_WxWidgetsDialog_T"));

#if defined (ACE_WIN64) || defined (ACE_WIN32)
  inherited::MSWSetOldWndProc (NULL);
#endif // ACE_WIN64 || ACE_WIN32
}

template <typename InterfaceType>
bool
ARDrone_WxWidgetsDialog_T<InterfaceType>::OnInit_2 (IAPPLICATION_T* iapplication_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::OnInit_2"));

  // sanity check(s)
  ACE_ASSERT (!application_);

  application_ = dynamic_cast<InterfaceType*> (iapplication_in);
  ACE_ASSERT (application_);

  togglebutton_record = XRCCTRL (*this, "togglebutton_record", wxToggleButton);
  button_snapshot = XRCCTRL (*this, "button_snapshot", wxBitmapButton);
  button_cut = XRCCTRL (*this, "button_cut", wxBitmapButton);
  button_report = XRCCTRL (*this, "button_report", wxBitmapButton);
  button_snapshot = XRCCTRL (*this, "button_snapshot", wxBitmapButton);
  spincontrol_frames_captured = XRCCTRL (*this, "spincontrol_frames_captured", wxSpinCtrl);
  spincontrol_frames_dropped = XRCCTRL (*this, "spincontrol_frames_dropped", wxSpinCtrl);
  spincontrol_messages_session = XRCCTRL (*this, "spincontrol_messages_session", wxSpinCtrl);
  spincontrol_messages_data = XRCCTRL (*this, "spincontrol_messages_data", wxSpinCtrl);
  spincontrol_data = XRCCTRL (*this, "spincontrol_data", wxSpinCtrl);
  spincontrol_buffer = XRCCTRL (*this, "spincontrol_buffer", wxSpinCtrl);
  choice_interface = XRCCTRL (*this, "choice_interface", wxChoice);
  button_hardware_settings = XRCCTRL (*this, "button_hardware_settings", wxBitmapButton);
  choice_format = XRCCTRL (*this, "choice_format", wxChoice);
  choice_resolution = XRCCTRL (*this, "choice_resolution", wxChoice);
  choice_framerate = XRCCTRL (*this, "choice_framerate", wxChoice);
  button_reset = XRCCTRL (*this, "button_reset", wxBitmapButton);
  togglebutton_save = XRCCTRL (*this, "togglebutton_save", wxToggleButton);
  textcontrol_filename = XRCCTRL (*this, "textcontrol_filename", wxTextCtrl);
  directorypicker_save = XRCCTRL (*this, "directorypicker_save", wxDirPickerCtrl);
  togglebutton_display = XRCCTRL (*this, "togglebutton_display", wxToggleButton);
  togglebutton_fullscreen = XRCCTRL (*this, "togglebutton_fullscreen", wxToggleButton);
  choice_adapter = XRCCTRL (*this, "choice_adapter", wxChoice);
  choice_display = XRCCTRL (*this, "choice_display", wxChoice);
  button_display_settings = XRCCTRL (*this, "button_display_settings", wxBitmapButton);
  choice_resolution_2 = XRCCTRL (*this, "choice_resolution_2", wxChoice);
  panel_video = XRCCTRL (*this, "panel_video", wxPanel);
  button_about = XRCCTRL (*this, "button_about", wxBitmapButton);
  button_quit = XRCCTRL (*this, "button_quit", wxBitmapButton);
  gauge_progress = XRCCTRL (*this, "gauge_progress", wxGauge);

  // populate controls
#if defined (_DEBUG)
#else
  button_report->Show (false);
#endif // _DEBUG
  bool activate_source = true;
  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
  StreamType::CONFIGURATION_T::ITERATOR_T stream_iterator =
    configuration_r.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != configuration_r.configuration->streamConfiguration.end ());

  Stream_Device_List_t interfaces_a;
#if defined (ACE_WIN64) || defined (ACE_WIN32)
  switch (configuration_r.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      interfaces_a =
        Stream_Device_DirectShow_Tools::getCaptureDevices (CLSID_VideoInputDeviceCategory);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_ASSERT (false);
      ACE_NOTSUP_RETURN (false);
      ACE_NOTREACHED (return false;)
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  configuration_r.mediaFramework));
      return false;
    }
  } // end SWITCH
#else
  ACE_ASSERT (false);
  ACE_NOTSUP_RETURN (false);
  ACE_NOTREACHED (return false;)
#endif // ACE_WIN64 || ACE_WIN32

  int index_i = wxNOT_FOUND;
  for (Stream_Device_ListIterator_t iterator = interfaces_a.begin ();
       iterator != interfaces_a.end ();
       ++iterator)
  {
#if defined (ACE_WIN64) || defined (ACE_WIN32)
    switch (configuration_r.mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        index_i =
          choice_interface->Append (Stream_Device_DirectShow_Tools::devicePathToString (*iterator).c_str ());
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        ACE_ASSERT (false);
        ACE_NOTSUP_RETURN (false);
        ACE_NOTREACHED (return false;)
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                    configuration_r.mediaFramework));
        return false;
      }
    } // end SWITCH
#else
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (false);
    ACE_NOTREACHED (return false;)
#endif // ACE_WIN64 || ACE_WIN32
  } // end FOR
  if (unlikely (interfaces_a.empty ()))
    activate_source = false;
  else
    choice_interface->Enable (true);

  if (likely (activate_source))
  {
    index_i =
      (initializing_ ? choice_interface->FindString ((*stream_iterator).second.second.interfaceIdentifier.c_str ())
                     : 0);
    choice_interface->Select (index_i);
    wxCommandEvent event_s (wxEVT_COMMAND_CHOICE_SELECTED,
                            XRCID ("choice_interface"));
    event_s.SetInt (index_i);
    //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
    this->AddPendingEvent (&event_s);
  } // end IF

  return true;
}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::OnExit_2 ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::OnExit_2"));

  // sanity check(s)
  ACE_ASSERT (application_);
}

//////////////////////////////////////////

template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::dialog_main_idle_cb (wxIdleEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::dialog_main_idle_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.stream);

  if (configuration_r.stream->IsRunning ())
    gauge_progress->Pulse ();
}

template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::dialog_main_keydown_cb (wxKeyEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::dialog_main_keydown_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.stream);
}

//////////////////////////////////////////

template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::togglebutton_connect_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_connect_toggled_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::button_calibrate_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_calibrate_clicked_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::button_animate_LEDs_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_animate_LEDs_clicked_cb"));

}
#if defined (_DEBUG)
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::button_report_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_report_clicked_cb"));

}
#endif // _DEBUG
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::togglebutton_associate_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_associate_toggled_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::choice_interface_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_interface_selected_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
#if defined (ACE_WIN64) || defined (ACE_WIN32)
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#else
  typename StreamType::CONFIGURATION_T::ITERATOR_T stream_iterator =
    configuration_r.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != configuration_r.configuration->streamConfiguration.end ());
#endif // ACE_WIN64 || ACE_WIN32

  std::string interface_identifier;
  int index_i = -1;
#if defined (ACE_WIN64) || defined (ACE_WIN32)
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#else
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#endif // ACE_WIN64 || ACE_WIN32
  ACE_ASSERT (!interface_identifier.empty ());

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#else
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#endif // ACE_WIN32 || ACE_WIN64
}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::spincontrol_port_changed_cb (wxSpinEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::spincontrol_port_changed_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::togglebutton_display_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_display_toggled_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::togglebutton_fullscreen_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_fullscreen_toggled_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::button_switch_camera_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_switch_camera_clicked_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::choice_adapter_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_adapter_selected_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::choice_display_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_selected_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::button_display_settings_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_display_settings_clicked_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::choice_display_video_format_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_video_format_selected_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
#if defined (ACE_WIN64) || defined (ACE_WIN32)
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#else
  typename StreamType::CONFIGURATION_T::ITERATOR_T stream_iterator =
    configuration_r.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != configuration_r.configuration->streamConfiguration.end ());
#endif // ACE_WIN64 || ACE_WIN32
}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::togglebutton_save_video_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_save_video_toggled_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::choice_save_video_format_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_save_video_format_selected_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
#if defined (ACE_WIN64) || defined (ACE_WIN32)
  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
#else
  typename StreamType::CONFIGURATION_T::ITERATOR_T stream_iterator =
    configuration_r.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != configuration_r.configuration->streamConfiguration.end ());
#endif // ACE_WIN64 || ACE_WIN32
}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::directorypicker_save_video_changed_cb (wxFileDirPickerEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::directorypicker_save_video_changed_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::button_about_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_about_clicked_cb"));

}
template <typename InterfaceType>
void
ARDrone_WxWidgetsDialog_T<InterfaceType>::button_quit_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_quit_clicked_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  // step1: make sure the stream has stopped
  StreamType* stream_p = NULL;
  InterfaceType::CONFIGURATION_T& configuration_r =
    const_cast<InterfaceType::CONFIGURATION_T&> (application_->getR_2 ());
  stream_p = configuration_r.stream;
  ACE_ASSERT (stream_p);
  const enum Stream_StateMachine_ControlState& status_r =
    stream_p->status ();
  if ((status_r == STREAM_STATE_RUNNING) ||
      (status_r == STREAM_STATE_PAUSED))
    stream_p->stop (false, // wait for completion ?
                    false, // recurse upstream (if any) ?
                    true); // locked access ?

  // step2: close main window
  this->Close (true); // force ?

  // step3: initiate shutdown sequence
  int result = ACE_OS::raise (SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));
}

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::ARDrone_WxWidgetsDialog_T (wxWindow* parent_in)
 : inherited (parent_in)
 , application_ (NULL)
 , initializing_ (true)
 , reset_ (false)
 , untoggling_ (false)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::ARDrone_WxWidgetsDialog_T"));

#if defined (ACE_WIN64) || defined (ACE_WIN32)
  inherited::MSWSetOldWndProc (NULL);
#endif // ACE_WIN64 || ACE_WIN32
}

bool
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::OnInit_2 (IAPPLICATION_T* iapplication_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::OnInit_2"));

  // sanity check(s)
  ACE_ASSERT (!application_);

  application_ =
    dynamic_cast<ARDrone_DirectShow_WxWidgetsIApplication_t*> (iapplication_in);
  ACE_ASSERT (application_);

  togglebutton_connect = XRCCTRL (*this, "togglebutton_connect", wxToggleButton);
  button_calibrate = XRCCTRL (*this, "button_calibrate", wxBitmapButton);
  button_animate_leds = XRCCTRL (*this, "button_animate_leds", wxBitmapButton);
  button_report = XRCCTRL (*this, "button_report", wxBitmapButton);
  togglebutton_auto_associate = XRCCTRL (*this, "togglebutton_auto_associate", wxToggleButton);
  choice_interface = XRCCTRL (*this, "choice_interface", wxChoice);
  textcontrol_SSID = XRCCTRL (*this, "textcontrol_SSID", wxTextCtrl);
  textcontrol_address = XRCCTRL (*this, "textcontrol_address", wxTextCtrl);
  spincontrol_port_video = XRCCTRL (*this, "spincontrol_port_video", wxSpinCtrl);
  spincontrol_buffer = XRCCTRL (*this, "spincontrol_buffer", wxSpinCtrl);
  togglebutton_display_video = XRCCTRL (*this, "togglebutton_display_video", wxToggleButton);
  togglebutton_fullscreen = XRCCTRL (*this, "togglebutton_fullscreen", wxToggleButton);
  button_switch_camera = XRCCTRL (*this, "button_switch_camera", wxBitmapButton);
  choice_display_video_format = XRCCTRL (*this, "choice_display_video_format", wxChoice);
  choice_adapter = XRCCTRL (*this, "choice_adapter", wxChoice);
  choice_display = XRCCTRL (*this, "choice_display", wxChoice);
  button_display_settings = XRCCTRL (*this, "button_display_settings", wxBitmapButton);
  choice_display_resolution = XRCCTRL (*this, "choice_display_resolution", wxChoice);
  togglebutton_save_video = XRCCTRL (*this, "togglebutton_save_video", wxToggleButton);
  choice_save_video_format = XRCCTRL (*this, "choice_save_video_format", wxChoice);
  button_snapshot = XRCCTRL (*this, "button_snapshot", wxBitmapButton);
  textcontrol_save_video_filename = XRCCTRL (*this, "textcontrol_save_video_filename", wxTextCtrl);
  directorypicker_save_video = XRCCTRL (*this, "directorypicker_save_video", wxDirPickerCtrl);
  panel_video = XRCCTRL (*this, "panel_video", wxPanel);
  spincontrol_control_session_messages = XRCCTRL (*this, "spincontrol_control_session_messages", wxSpinCtrl);
  spincontrol_control_data_messages = XRCCTRL (*this, "spincontrol_control_data_messages", wxSpinCtrl);
  control_gauge = XRCCTRL (*this, "control_gauge", wxGauge);
  spincontrol_navdata_session_messages = XRCCTRL (*this, "spincontrol_navdata_session_messages", wxSpinCtrl);
  spincontrol_navdata_data_messages = XRCCTRL (*this, "spincontrol_navdata_data_messages", wxSpinCtrl);
  navdata_gauge = XRCCTRL (*this, "navdata_gauge", wxGauge);
  spincontrol_mavlink_session_messages = XRCCTRL (*this, "spincontrol_mavlink_session_messages", wxSpinCtrl);
  spincontrol_mavlink_data_messages = XRCCTRL (*this, "spincontrol_mavlink_data_messages", wxSpinCtrl);
  mavlink_gauge = XRCCTRL (*this, "mavlink_gauge", wxGauge);
  spincontrol_video_session_messages = XRCCTRL (*this, "spincontrol_video_session_messages", wxSpinCtrl);
  spincontrol_video_data_messages = XRCCTRL (*this, "spincontrol_video_data_messages", wxSpinCtrl);
  video_gauge = XRCCTRL (*this, "video_gauge", wxGauge);
  spincontrol_connections = XRCCTRL (*this, "spincontrol_connections", wxSpinCtrl);
  spincontrol_data = XRCCTRL (*this, "spincontrol_data", wxSpinCtrl);
  cache_gauge = XRCCTRL (*this, "cache_gauge", wxGauge);
  label_roll_value = XRCCTRL (*this, "label_roll_value", wxStaticText);
  label_pitch_value = XRCCTRL (*this, "label_pitch_value", wxStaticText);
  label_yaw_value = XRCCTRL (*this, "label_yaw_value", wxStaticText);
  button_about = XRCCTRL (*this, "button_about", wxBitmapButton);
  button_quit = XRCCTRL (*this, "button_quit", wxBitmapButton);
  animation_associating = XRCCTRL (*this, "animation_associating", wxAnimationCtrl);
  gauge_progress = XRCCTRL (*this, "gauge_progress", wxGauge);

  this->SetDefaultItem (togglebutton_connect);

  // populate controls
#if defined (_DEBUG)
#else
  button_report->Show (false);
#endif // _DEBUG
  spincontrol_port_video->SetRange (0,
                                    std::numeric_limits<int>::max ());
  spincontrol_buffer->SetRange (0,
                                std::numeric_limits<int>::max ());
  spincontrol_control_session_messages->SetRange (0,
                                                  std::numeric_limits<int>::max ());
  spincontrol_control_data_messages->SetRange (0,
                                               std::numeric_limits<int>::max ());
  spincontrol_navdata_session_messages->SetRange (0,
                                                  std::numeric_limits<int>::max ());
  spincontrol_navdata_data_messages->SetRange (0,
                                               std::numeric_limits<int>::max ());
  spincontrol_mavlink_session_messages->SetRange (0,
                                                  std::numeric_limits<int>::max ());
  spincontrol_mavlink_data_messages->SetRange (0,
                                               std::numeric_limits<int>::max ());
  spincontrol_video_session_messages->SetRange (0,
                                                std::numeric_limits<int>::max ());
  spincontrol_video_data_messages->SetRange (0,
                                             std::numeric_limits<int>::max ());
  spincontrol_connections->SetRange (0,
                                     std::numeric_limits<int>::max ());
  spincontrol_data->SetRange (0,
                              std::numeric_limits<int>::max ());

  bool activate_interface_b = true, activate_display_b = true;
  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_DirectShow_StreamConfigurationsIterator_t streams_iterator =
    configuration_r.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != configuration_r.configuration->streamConfigurations.end ());
  ARDrone_DirectShow_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator =
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != (*streams_iterator).second.end ());
  //ARDrone_DirectShow_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator_2 =
  //  (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName ((*streams_iterator).second.configuration_.renderer).c_str ()));
  //ACE_ASSERT (stream_iterator_2 != (*streams_iterator).second.end ());
  ARDrone_DirectShow_ConnectionConfigurationIterator_t iterator =
    configuration_r.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.configuration->connectionConfigurations.end ());
  ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t iterator_2 =
    (*iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (iterator_2 != (*iterator).second.end ());

  togglebutton_auto_associate->Enable (!InlineIsEqualGUID (configuration_r.configuration->WLANMonitorConfiguration.interfaceIdentifier, GUID_NULL));
  togglebutton_auto_associate->SetValue (configuration_r.configuration->WLANMonitorConfiguration.autoAssociate);

  Net_InterfaceIdentifiers_t interfaces_a =
    Net_WLAN_Tools::getInterfaces (ARDRONE_WLANMONITOR_SINGLETON::instance ()->get_2 ());

  int index_i = -1;
  wxStringClientData* client_data_p = NULL;
  NET_IFINDEX interface_index_i = 0;
  for (Net_InterfacesIdentifiersIterator_t iterator = interfaces_a.begin ();
       iterator != interfaces_a.end ();
       ++iterator)
  {
    client_data_p = NULL;
    ACE_NEW_NORETURN (client_data_p,
                      wxStringClientData ());
    ACE_ASSERT (client_data_p);
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
    client_data_p->SetData (Common_Tools::GUIDToString (*iterator).c_str ());
#else
    interface_index_i = Net_Common_Tools::interfaceToIndex (*iterator);
    ACE_ASSERT (interface_index_i);
    client_data_p->SetData (Common_Tools::GUIDToString (Net_Common_Tools::indexToInterface_2 (interface_index_i)).c_str ());
#endif // _WIN32_WINNT_VISTA
    index_i =
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
      choice_interface->Append (Net_Common_Tools::interfaceToString (*iterator).c_str (),
#else
      choice_interface->Append (Net_Common_Tools::interfaceToString (interface_index_i).c_str (),
#endif // _WIN32_WINNT_VISTA
                                client_data_p);
  } // end FOR
  if (unlikely (interfaces_a.empty ()))
    activate_interface_b = false;
  else
    choice_interface->Enable (true);
  textcontrol_SSID->Enable (!configuration_r.configuration->WLANMonitorConfiguration.SSID.empty ());
  textcontrol_SSID->SetValue (configuration_r.configuration->WLANMonitorConfiguration.SSID.c_str ());
  const struct Net_WLAN_AccessPointState access_point_s =
    ARDRONE_WLANMONITOR_SINGLETON::instance ()->get1RR (configuration_r.configuration->WLANMonitorConfiguration.SSID).second;
  textcontrol_address->Enable (!access_point_s.IPAddress.is_any ());
  textcontrol_address->SetValue (Net_Common_Tools::IPAddressToString (access_point_s.IPAddress, true).c_str ());
  spincontrol_port_video->Enable (true);
  spincontrol_port_video->SetValue ((*iterator_2).second.socketHandlerConfiguration.socketConfiguration_2.address.get_port_number ());
  spincontrol_buffer->Enable (true);
  spincontrol_buffer->SetValue ((*streams_iterator).second.allocatorConfiguration_.defaultBufferSize);

  Common_UI_DisplayAdapters_t display_adapters_a =
    Common_UI_Tools::getAdapters ();
  for (Common_UI_DisplayAdaptersIterator_t iterator = display_adapters_a.begin ();
       iterator != display_adapters_a.end ();
       ++iterator)
  {
    client_data_p = NULL;
    ACE_NEW_NORETURN (client_data_p,
                      wxStringClientData ());
    ACE_ASSERT (client_data_p);
    client_data_p->SetData ((*iterator).id);

    index_i =
      choice_adapter->Append ((*iterator).description.c_str (),
                              client_data_p);
  } // end FOR
  Common_UI_DisplayDevices_t display_interfaces_a =
    Common_UI_Tools::getDisplays ();
  for (Common_UI_DisplayDevicesIterator_t iterator = display_interfaces_a.begin ();
       iterator != display_interfaces_a.end ();
       ++iterator)
  {
    client_data_p = NULL;
    ACE_NEW_NORETURN (client_data_p,
                      wxStringClientData ());
    ACE_ASSERT (client_data_p);
    client_data_p->SetData ((*iterator).device);

    index_i =
      choice_display->Append ((*iterator).description.c_str (),
                              client_data_p);
  } // end FOR
  std::stringstream converter;
  client_data_p = NULL;
  ACE_NEW_NORETURN (client_data_p,
                    wxStringClientData ());
  ACE_ASSERT (client_data_p);
  converter << ARDRONE_VIDEOMODE_360P;
  client_data_p->SetData (converter.str ());
  index_i =
    choice_display_video_format->Append (ARDroneVideoModeToString (ARDRONE_VIDEOMODE_360P).c_str (),
                                         client_data_p);
  client_data_p = NULL;
  ACE_NEW_NORETURN (client_data_p,
                    wxStringClientData ());
  ACE_ASSERT (client_data_p);
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << ARDRONE_VIDEOMODE_720P;
  client_data_p->SetData (converter.str ());
  index_i =
    choice_display_video_format->Append (ARDroneVideoModeToString (ARDRONE_VIDEOMODE_720P).c_str (),
                                         client_data_p);
  if (likely (!display_interfaces_a.empty ()))
  {
    togglebutton_display_video->Enable (!(*stream_iterator).second.second.interfaceIdentifier.empty ());
    togglebutton_display_video->SetValue (!(*stream_iterator).second.second.interfaceIdentifier.empty ());
    togglebutton_fullscreen->Enable (togglebutton_display_video->GetValue ());
    ACE_ASSERT ((*stream_iterator).second.second.direct3DConfiguration);
    togglebutton_fullscreen->SetValue (!(*stream_iterator).second.second.direct3DConfiguration->presentationParameters.Windowed);
    choice_display_video_format->Enable (togglebutton_display_video->GetValue ());
    panel_video->Show (togglebutton_display_video->GetValue () &&
                       !togglebutton_fullscreen->GetValue ());
  } // end IF
  if (unlikely (interfaces_a.empty ()))
    activate_display_b = false;
  else
    choice_display->Enable (true);

  togglebutton_save_video->Enable (true);
  togglebutton_save_video->SetValue (!(*stream_iterator).second.second.targetFileName.empty ());
  choice_save_video_format->Enable (true);
  client_data_p = NULL;
  ACE_NEW_NORETURN (client_data_p,
                    wxStringClientData ());
  ACE_ASSERT (client_data_p);
  client_data_p->SetData (Common_Tools::GUIDToString (MEDIASUBTYPE_RGB24).c_str ());
  index_i =
    choice_save_video_format->Append (ACE_TEXT_ALWAYS_CHAR ("RGB AVI"),
                                      client_data_p);
  ACE_ASSERT (index_i != wxNOT_FOUND);
  choice_save_video_format->Select (index_i);
  wxCommandEvent event_s (wxEVT_COMMAND_CHOICE_SELECTED,
                          XRCID ("choice_save_video_format"));
  event_s.SetInt (index_i);
  //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
  this->AddPendingEvent (event_s);
  textcontrol_save_video_filename->Enable (togglebutton_save_video->GetValue ());
  textcontrol_save_video_filename->SetValue (ACE_TEXT_ALWAYS_CHAR ((*stream_iterator).second.second.targetFileName.empty () ? ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_FILE_NAME)
                                                                                                                            : ACE::basename ((*stream_iterator).second.second.targetFileName.c_str (),
                                                                                                                                             ACE_DIRECTORY_SEPARATOR_CHAR)));
  directorypicker_save_video->Enable (togglebutton_save_video->GetValue ());
  directorypicker_save_video->SetPath (ACE_TEXT_ALWAYS_CHAR ((*stream_iterator).second.second.targetFileName.empty () ? Common_File_Tools::getTempDirectory ()
                                                                                                                      : ACE::dirname ((*stream_iterator).second.second.targetFileName.c_str (),
                                                                                                                                      ACE_DIRECTORY_SEPARATOR_CHAR)));

  if (likely (activate_interface_b))
  {
    index_i =
      (initializing_ ? (!InlineIsEqualGUID (configuration_r.configuration->WLANMonitorConfiguration.interfaceIdentifier, GUID_NULL) ? Common_UI_WxWidgets_Tools::clientDataToIndex (choice_interface,
                                                                                                                                                                                    Common_Tools::GUIDToString (configuration_r.configuration->WLANMonitorConfiguration.interfaceIdentifier))
                                                                                                                                    : 0)
                     : 0);
    ACE_ASSERT (index_i != wxNOT_FOUND);
    choice_interface->Select (index_i);
    wxCommandEvent event_2 (wxEVT_COMMAND_CHOICE_SELECTED,
                            XRCID ("choice_interface"));
    event_2.SetInt (index_i);
    //choice_interface->GetEventHandler ()->ProcessEvent (event_2);
    this->AddPendingEvent (event_2);
  } // end IF
  application_->wait ();

  if (likely (activate_display_b))
  {
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << configuration_r.videoMode;
    index_i =
      (initializing_ ? ((configuration_r.videoMode != ARDRONE_VIDEOMODE_INVALID) ? Common_UI_WxWidgets_Tools::clientDataToIndex (choice_display_video_format,
                                                                                                                                 converter.str ())
                                                                                 : 0)
                     : 0);
    ACE_ASSERT (index_i != wxNOT_FOUND);
    choice_display_video_format->Select (index_i);
    wxCommandEvent event_2 (wxEVT_COMMAND_CHOICE_SELECTED,
                            XRCID ("choice_display_video_format"));
    event_2.SetInt (index_i);
    //choice_interface->GetEventHandler ()->ProcessEvent (event_2);
    this->AddPendingEvent (event_2);

    index_i =
      (initializing_ ? (!(*stream_iterator).second.second.interfaceIdentifier.empty () ? Common_UI_WxWidgets_Tools::clientDataToIndex (choice_display,
                                                                                                                                       (*stream_iterator).second.second.interfaceIdentifier)
                                                                                       : 0)
                     : 0);
    ACE_ASSERT (index_i != wxNOT_FOUND);
    choice_display->Select (index_i);
    wxCommandEvent event_3 (wxEVT_COMMAND_CHOICE_SELECTED,
                            XRCID ("choice_display"));
    event_3.SetInt (index_i);
    //choice_interface->GetEventHandler ()->ProcessEvent (event_3);
    this->AddPendingEvent (event_3);
    wxCommandEvent event_4 (wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
                            XRCID ("togglebutton_display"));
    event_4.SetInt (!(*stream_iterator).second.second.interfaceIdentifier.empty () ? 1
                                                                                   : 0);
    //togglebutton_display_video->GetEventHandler ()->ProcessEvent (event_4);
    this->AddPendingEvent (event_4);
    wxCommandEvent event_5 (wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
                            XRCID ("togglebutton_fullscreen"));
    ACE_ASSERT ((*stream_iterator).second.second.direct3DConfiguration);
    event_5.SetInt ((*stream_iterator).second.second.direct3DConfiguration->presentationParameters.Windowed ? 0 
                                                                                                            : 1);
    //togglebutton_fullscreen->GetEventHandler ()->ProcessEvent (event_5);
    this->AddPendingEvent (event_5);
  } // end IF

  button_about->Enable (true);
  button_quit->Enable (true);

  return true;
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::OnExit_2 ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::OnExit_2"));

  // sanity check(s)
  ACE_ASSERT (application_);
}

//////////////////////////////////////////

void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::dialog_main_idle_cb (wxIdleEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::dialog_main_idle_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ARDrone_StreamsIterator_t iterator = configuration_r.streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.streams.end ());
  Stream_IStreamControlBase* istream_p =
    dynamic_cast<Stream_IStreamControlBase*> ((*iterator).second);
  bool finished_b = false;

  // sanity check(s)
  ACE_ASSERT (istream_p);

  process_stream_events (&configuration_r,
                         finished_b);
  if (!finished_b &&
      istream_p->isRunning ())
    gauge_progress->Pulse ();
}

void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::dialog_main_keydown_cb (wxKeyEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::dialog_main_keydown_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);
  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ARDrone_StreamsIterator_t iterator = configuration_r.streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.streams.end ());
  Stream_IStreamControlBase* istream_p =
    dynamic_cast<Stream_IStreamControlBase*> ((*iterator).second);
  ACE_ASSERT (istream_p);

  switch (event_in.GetUnicodeKey ())
  {
    // It's a "normal" character. Notice that this includes control characters
    // in 1..31 range, e.g. WXK_RETURN or WXK_BACK, so check for them explicitly
    case 'c':
    case 'C':
    {
      wxCommandEvent event_s (wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
                              XRCID ("togglebutton_connect"));
      bool is_checked_b = togglebutton_connect->GetValue ();
      togglebutton_connect->SetValue (!is_checked_b);
      event_s.SetInt (togglebutton_connect->GetValue () ? 0 : 1);
      //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
      this->AddPendingEvent (event_s);
      break;
    }
    case 'f':
    case 'F':
    case WXK_ESCAPE:
    {
      // sanity check(s)
      if (!istream_p->isRunning ())
        return; // nothing to do

      // *NOTE*: escape does nothing when not fullscreen
      if ((event_in.GetUnicodeKey () == WXK_ESCAPE) &&
          !togglebutton_fullscreen->GetValue ())
        break;
      bool is_checked_b = togglebutton_fullscreen->GetValue ();
      togglebutton_fullscreen->SetValue (!is_checked_b);
      wxCommandEvent event_s (wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
                              XRCID ("togglebutton_fullscreen"));
      event_s.SetInt (is_checked_b ? 0 : 1);
      //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
      this->AddPendingEvent (event_s);
      break;
    }
    case 'h':
    case 'H':
    {
      // sanity check(s)
      if (!istream_p->isRunning ())
        return; // nothing to do

      wxCommandEvent event_s (wxEVT_COMMAND_BUTTON_CLICKED,
                              XRCID ("button_snapshot"));
      event_s.SetInt (1);
      //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
      this->AddPendingEvent (event_s);
      break;
    }
    case 's':
    case 'S':
    {
      // sanity check(s)
      if (!istream_p->isRunning ())
        return; // nothing to do

      wxCommandEvent event_s (wxEVT_COMMAND_BUTTON_CLICKED,
                              XRCID ("button_cut"));
      event_s.SetInt (1);
      //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
      this->AddPendingEvent (event_s);
      break;
    }
    //////////////////////////////////////
    case WXK_NONE: // no character value (i.e. control characters)
    {
      switch (event_in.GetKeyCode ())
      {
        //case :
        //{
        //  break;
        //}
        default:
          break;
      } // end SWITCH
      break;
    }
    default:
      break;
  } // end SWITCH
}

//////////////////////////////////////////

void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::togglebutton_connect_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_connect_toggled_cb"));

  // handle untoggle --> PLAY
  if (untoggling_)
  {
    untoggling_ = false;
    return; // done
  } // end IF

  // sanity check(s)
  ACE_ASSERT (application_);

  // --> user pressed play/pause/stop
  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ARDrone_StreamsIterator_t iterator = configuration_r.streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.streams.end ());
  Stream_IStreamControlBase* istream_p =
    dynamic_cast<Stream_IStreamControlBase*> ((*iterator).second);
  ACE_ASSERT (istream_p);
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_DirectShow_StreamConfigurationsIterator_t streams_iterator =
    configuration_r.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != configuration_r.configuration->streamConfigurations.end ());
  ARDrone_DirectShow_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator =
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != (*streams_iterator).second.end ());
  //Stream_CamSave_DirectShow_Stream::CONFIGURATION_T::ITERATOR_T stream_iterator_2 =
  //  configuration_r.configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName (configuration_r.configuration->streamConfiguration.configuration_.renderer).c_str ()));
  //ACE_ASSERT (stream_iterator_2 != configuration_r.configuration->streamConfiguration.end ());

  // toggle ?
  if (!event_in.IsChecked ())
  { // --> user pressed pause/stop
    ACE_ASSERT (istream_p->isRunning ());
    istream_p->stop (false, // wait ?
                     true,  // recurse upstream ?
                     true); // locked access ?

    // modify controls
    togglebutton_connect->Enable (false);

    return;
  } // end IF

  // --> user pressed connect

  struct ARDrone_ThreadData* thread_data_p = NULL;
  ACE_thread_t thread_id = std::numeric_limits<unsigned long>::max ();
  ACE_hthread_t thread_handle = ACE_INVALID_HANDLE;
  const char* thread_name_2 = NULL;
  ACE_Thread_Manager* thread_manager_p = NULL;

  // step1: reset progress reporting
  ACE_OS::memset (&configuration_r.progressData.statistic,
                  0,
                  sizeof (struct ARDrone_Statistic));

  // step2: update configuration
  // step2a: update display configuration
  (*streams_iterator).second.allocatorConfiguration_.defaultBufferSize =
    spincontrol_buffer->GetValue ();

  switch ((*streams_iterator).second.configuration_.renderer)
  {
    //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
    //  break;
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
    {
      // sanity check(s)
      Common_UI_Resolution_t resolution_2 =
        Stream_MediaFramework_DirectShow_Tools::toResolution ((*stream_iterator).second.second.outputFormat);
      configuration_r.configuration->direct3DConfiguration.presentationParameters.BackBufferWidth =
        resolution_2.cx;
      configuration_r.configuration->direct3DConfiguration.presentationParameters.BackBufferHeight =
        resolution_2.cy;
      break;
    }
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
    case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
      break;
    //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
    //  break;
#if defined (GTK_USE)
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
      break;
#endif // GTK_USE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                  (*streams_iterator).second.configuration_.renderer));
      return;
    }
  } // end SWITCH

  if (togglebutton_display_video->GetValue ())
  {
  //  wxRect rectangle_s = panel_video->GetClientRect ();
  //  (*stream_iterator).second.second.area.left = rectangle_s.GetX ();
  //  (*stream_iterator).second.second.area.right =
  //    (*stream_iterator).second.second.area.left + rectangle_s.GetWidth ();
  //  (*stream_iterator).second.second.area.top = rectangle_s.GetY ();
  //  (*stream_iterator).second.second.area.bottom =
  //    (*stream_iterator).second.second.area.top + rectangle_s.GetHeight ();
  //  (*stream_iterator_2).second.second.area =
  //    (*stream_iterator).second.second.area;
  //  client_data_p =
  //    dynamic_cast<wxStringClientData*> (choice_display->GetClientObject (choice_display->GetSelection ()));
  //  ACE_ASSERT (client_data_p);
  //  (*stream_iterator_2).second.second.interfaceIdentifier =
  //    client_data_p->GetData ().ToStdString ();

    switch ((*streams_iterator).second.configuration_.renderer)
    {
      //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
      //  break;
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
      {
        configuration_r.configuration->direct3DConfiguration.focusWindow =
          (HWND)panel_video->GetHandle ();
        configuration_r.configuration->direct3DConfiguration.presentationParameters.hDeviceWindow =
          (HWND)panel_video->GetHandle ();
        break;
      }
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
      case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
        break;
      //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
      //  break;
#if defined (GTK_USE)
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
        break;
#endif // GTK_USE
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                    (*streams_iterator).second.configuration_.renderer));
        return;
      }
    } // end SWITCH
  } // end IF
  else
  {
  //  ACE_OS::memset (&(*stream_iterator_2).second.second.area, 0, sizeof (struct tagRECT));
  //  (*stream_iterator).second.second.area =
  //    (*stream_iterator_2).second.second.area;
  //  (*stream_iterator_2).second.second.interfaceIdentifier.clear ();
  //  //ACE_ASSERT ((*stream_iterator_2).second.second.direct3DConfiguration);

    switch ((*streams_iterator).second.configuration_.renderer)
    {
      //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
      //  break;
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
      {
        configuration_r.configuration->direct3DConfiguration.focusWindow =
          NULL;
        configuration_r.configuration->direct3DConfiguration.presentationParameters.hDeviceWindow =
          NULL;
        break;
      }
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
      case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
        break;
      //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
      //  break;
#if defined (GTK_USE)
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
        break;
#endif // GTK_USE
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                    (*streams_iterator).second.configuration_.renderer));
        return;
      }
    } // end SWITCH
  } // end ELSE
  if (togglebutton_fullscreen->GetValue ())
  {
  //  struct Common_UI_DisplayDevice display_device_s =
  //    Common_UI_Tools::getDisplayDevice ((*stream_iterator).second.second.interfaceIdentifier);
  //  (*stream_iterator_2).second.second.area = display_device_s.clippingArea;
  //  (*stream_iterator_2).second.second.fullScreen = true;

    switch ((*streams_iterator).second.configuration_.renderer)
    {
      //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
      //  break;
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
      {
        configuration_r.configuration->direct3DConfiguration.focusWindow =
          GetAncestor ((HWND)panel_video->GetHandle (),
                       GA_ROOTOWNER);
        ACE_ASSERT (configuration_r.configuration->direct3DConfiguration.focusWindow == (HWND)this->GetHandle ());
        configuration_r.configuration->direct3DConfiguration.presentationParameters.hDeviceWindow =
          NULL;
        //configuration_r.configuration->direct3DConfiguration.presentationParameters.Windowed =
        //  FALSE;
        break;
      }
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
      case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
        break;
      //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
      //  break;
#if defined (GTK_USE)
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
        break;
#endif // GTK_USE
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                    (*streams_iterator).second.configuration_.renderer));
        return;
      }
    } // end SWITCH
  } // end IF
  else
  {
  //  wxRect rectangle_s = panel_video->GetClientRect ();
  //  (*stream_iterator_2).second.second.area.left = rectangle_s.GetX ();
  //  (*stream_iterator_2).second.second.area.right =
  //    (*stream_iterator_2).second.second.area.left + rectangle_s.GetWidth ();
  //  (*stream_iterator_2).second.second.area.top = rectangle_s.GetY ();
  //  (*stream_iterator_2).second.second.area.bottom =
  //    (*stream_iterator_2).second.second.area.top + rectangle_s.GetHeight ();
  //  (*stream_iterator_2).second.second.fullScreen = false;

    switch ((*streams_iterator).second.configuration_.renderer)
    {
      //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
      //  break;
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
      {
        configuration_r.configuration->direct3DConfiguration.focusWindow =
          (HWND)panel_video->GetHandle ();
        configuration_r.configuration->direct3DConfiguration.presentationParameters.hDeviceWindow =
          (HWND)panel_video->GetHandle ();
        //  configuration_r.configuration->direct3DConfiguration.presentationParameters.Windowed =
        //    TRUE;
        break;
      }
      case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
      case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
        break;
      //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
      //  break;
#if defined (GTK_USE)
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
      case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
        break;
#endif // GTK_USE
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                    (*streams_iterator).second.configuration_.renderer));
        return;
      }
    } // end SWITCH
  } // end ELSE

  // step3: set up device ?
  switch ((*streams_iterator).second.configuration_.renderer)
  {
    //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
    //  break;
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
    {
      if (!reset_)
        break;

      // sanity check(s)
      ACE_ASSERT (configuration_r.configuration->direct3DConfiguration.handle);
      ACE_ASSERT (ACE_OS::thr_equal (ACE_OS::thr_self (), configuration_r.configuration->direct3DConfiguration.threadId));

      // *TODO*: remove ASAP
      ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, configuration_r.configuration->direct3DConfiguration.lock);

      // *NOTE*: may toggle the device between windowed/fullscreen mode
      if (!Stream_MediaFramework_DirectDraw_Tools::reset (configuration_r.configuration->direct3DConfiguration.handle,
                                                          configuration_r.configuration->direct3DConfiguration))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Stream_MediaFramework_DirectDraw_Tools::reset(), returning\n")));
        return;
      } // end IF
      reset_ = false;
      break;
    }
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
    case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
      break;
    //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
    //  break;
#if defined (GTK_USE)
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
      break;
#endif // GTK_USE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                  (*streams_iterator).second.configuration_.renderer));
      return;
    }
  } // end SWITCH

  // step2b: update save configuration
  std::string filename_string;
  if (!togglebutton_save_video->GetValue ())
    goto continue_;
  filename_string = directorypicker_save_video->GetPath ();
  ACE_ASSERT (Common_File_Tools::isDirectory (filename_string));
  ACE_ASSERT (Common_File_Tools::isWriteable (filename_string));
  filename_string += ACE_DIRECTORY_SEPARATOR_STR;
  filename_string += textcontrol_save_video_filename->GetValue ();
  ACE_ASSERT (Common_File_Tools::isValidPath (filename_string));
continue_:
  (*stream_iterator).second.second.targetFileName = filename_string;

  // step4: start processing thread(s)
  ACE_Thread_ID thread_id_2;
  bool result =
    Common_Test_U_Tools::spawn<struct ARDrone_ThreadData,
                               ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T> (ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_THREAD_NAME),
                                                                                             ::stream_processing_thread,
                                                                                             COMMON_EVENT_REACTOR_THREAD_GROUP_ID + 1,
                                                                                             configuration_r,
                                                                                             thread_id_2);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Test_U_Tools::spawn(): \"%m\", returning\n")));
    return;
  } // end IF

  // step5: modify controls
  button_calibrate->Enable (true);
  button_animate_leds->Enable (true);
#if defined (_DEBUG)
  button_report->Enable (true);
#endif // _DEBUG
  togglebutton_auto_associate->Enable (true);
  choice_interface->Enable (false);
  togglebutton_display_video->Enable (true);
  togglebutton_fullscreen->Enable (true);
  button_switch_camera->Enable (true);
  choice_display_video_format->Enable (true);
  choice_adapter->Enable (true);
  choice_display->Enable (true);
  button_display_settings->Enable (true);
  choice_display_resolution->Enable (true);
  togglebutton_save_video->Enable (false);
  choice_save_video_format->Enable (false);
  button_snapshot->Enable (true);
  directorypicker_save_video->Enable (false);
  gauge_progress->Enable (true);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_calibrate_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_calibrate_clicked_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ARDrone_StreamsIterator_t iterator = configuration_r.streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.streams.end ());
  ARDrone_IStreamControl_t* istream_p =
    dynamic_cast<ARDrone_IStreamControl_t*> ((*iterator).second);
  ACE_ASSERT (istream_p);

  istream_p->control (STREAM_CONTROL_STEP_2,
                      true); // recurse upstream ?
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_animate_LEDs_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_animate_LEDs_clicked_cb"));

}
#if defined (_DEBUG)
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_report_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_report_clicked_cb"));

}
#endif // _DEBUG
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::togglebutton_associate_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_associate_toggled_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::choice_interface_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_interface_selected_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_WLANMonitor_t* wlan_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (wlan_monitor_p);

  wxStringClientData* client_data_p =
    dynamic_cast<wxStringClientData*> (choice_interface->GetClientObject (event_in.GetSelection ()));
  ACE_ASSERT (client_data_p);
  struct _GUID interface_identifier_s =
    Common_Tools::StringToGUID (client_data_p->GetData ().ToStdString ());
  bool wlan_monitor_was_running_b = wlan_monitor_p->isRunning ();
  bool restart_wlan_monitor_b =
    (wlan_monitor_was_running_b                             &&
     !InlineIsEqualGUID (interface_identifier_s, GUID_NULL) &&
     !InlineIsEqualGUID (interface_identifier_s, configuration_r.configuration->WLANMonitorConfiguration.interfaceIdentifier));
  configuration_r.configuration->WLANMonitorConfiguration.interfaceIdentifier =
    interface_identifier_s;
  ACE_ASSERT (!InlineIsEqualGUID (configuration_r.configuration->WLANMonitorConfiguration.interfaceIdentifier, GUID_NULL));
  if (wlan_monitor_was_running_b &&
      InlineIsEqualGUID (interface_identifier_s, GUID_NULL))
    wlan_monitor_p->stop (true,
                          true);
  if (restart_wlan_monitor_b)
  {
    if (wlan_monitor_was_running_b)
      wlan_monitor_p->stop (true,
                            true);
    wlan_monitor_p->start ();
  } // end IF
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::spincontrol_port_changed_cb (wxSpinEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::spincontrol_port_changed_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::togglebutton_display_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_display_toggled_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_DirectShow_StreamConfigurationsIterator_t streams_iterator =
    configuration_r.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != configuration_r.configuration->streamConfigurations.end ());
  ARDrone_DirectShow_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator =
    //(*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName ((*streams_iterator).second.configuration_.renderer).c_str ()));
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != (*streams_iterator).second.end ());

  bool is_checked_b = event_in.IsChecked ();
  togglebutton_fullscreen->Enable (is_checked_b);
  choice_display->Enable (is_checked_b);
  button_display_settings->Enable (is_checked_b);

  if (is_checked_b)
  {
    wxRect rectangle_s = panel_video->GetClientRect ();
    (*stream_iterator).second.second.area.left = rectangle_s.GetX ();
    (*stream_iterator).second.second.area.right =
      (*stream_iterator).second.second.area.left + rectangle_s.GetWidth ();
    (*stream_iterator).second.second.area.top = rectangle_s.GetY ();
    (*stream_iterator).second.second.area.bottom =
      (*stream_iterator).second.second.area.top + rectangle_s.GetHeight ();
    wxStringClientData* client_data_p =
      dynamic_cast<wxStringClientData*> (choice_display->GetClientObject (choice_display->GetSelection ()));
    ACE_ASSERT (client_data_p);
    (*stream_iterator).second.second.interfaceIdentifier =
      client_data_p->GetData ().ToStdString ();
  } // end IF
  else
  {
    ACE_OS::memset (&(*stream_iterator).second.second.area, 0, sizeof (struct tagRECT));
    (*stream_iterator).second.second.interfaceIdentifier.clear ();
  } // end ELSE

  switch ((*streams_iterator).second.configuration_.renderer)
  {
    //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
    //  break;
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
    {
      //ACE_ASSERT ((*stream_iterator).second.second.direct3DConfiguration);
      if (is_checked_b)
      {
        configuration_r.configuration->direct3DConfiguration.focusWindow =
          (HWND)panel_video->GetHandle ();
      } // end IF
      else
      {
        configuration_r.configuration->direct3DConfiguration.focusWindow =
          NULL;
        configuration_r.configuration->direct3DConfiguration.presentationParameters.hDeviceWindow =
          NULL;
        configuration_r.configuration->direct3DConfiguration.presentationParameters.Windowed =
          FALSE;
      } // end ELSE
      break;
    }
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
    case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
      break;
    //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
    //  break;
#if defined (GTK_USE)
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
      break;
#endif // GTK_USE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                  (*streams_iterator).second.configuration_.renderer));
      return;
    }
  } // end SWITCH
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::togglebutton_fullscreen_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_fullscreen_toggled_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ARDrone_StreamsIterator_t iterator = configuration_r.streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.streams.end ());
  Stream_IStreamControlBase* istream_base_p = NULL;
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_DirectShow_StreamConfigurationsIterator_t streams_iterator =
    configuration_r.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != configuration_r.configuration->streamConfigurations.end ());
  ARDrone_DirectShow_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator =
    //(*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName ((*streams_iterator).second.configuration_.renderer).c_str ()));
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != (*streams_iterator).second.end ());

  choice_display_resolution->Enable (event_in.IsChecked ());
  if (event_in.IsChecked ())
  { // --> toggle to fullscreen
    struct Common_UI_DisplayDevice display_device_s =
      Common_UI_Tools::getDisplay ((*stream_iterator).second.second.interfaceIdentifier);
    (*stream_iterator).second.second.area = display_device_s.clippingArea;
    ACE_ASSERT ((*stream_iterator).second.second.direct3DConfiguration);
    (*stream_iterator).second.second.direct3DConfiguration->presentationParameters.Windowed = FALSE;
  } // end IF
  else
  { // toggle to windowed
    wxRect rectangle_s = panel_video->GetClientRect ();
    (*stream_iterator).second.second.area.left = rectangle_s.GetX ();
    (*stream_iterator).second.second.area.right =
      (*stream_iterator).second.second.area.left + rectangle_s.GetWidth ();
    (*stream_iterator).second.second.area.top = rectangle_s.GetY ();
    (*stream_iterator).second.second.area.bottom =
      (*stream_iterator).second.second.area.top + rectangle_s.GetHeight ();
    ACE_ASSERT ((*stream_iterator).second.second.direct3DConfiguration);
    (*stream_iterator).second.second.direct3DConfiguration->presentationParameters.Windowed = TRUE;
  } // end ELSE

  std::string module_name_string =
    Stream_Visualization_Tools::rendererToModuleName ((*streams_iterator).second.configuration_.renderer);
  switch ((*streams_iterator).second.configuration_.renderer)
  {
    //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
    //  break;
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
    {
      //ACE_ASSERT ((*stream_iterator).second.second.direct3DConfiguration);
      if (event_in.IsChecked ())
      { // --> toggle to fullscreen
        configuration_r.configuration->direct3DConfiguration.presentationParameters.hDeviceWindow =
          NULL;
        configuration_r.configuration->direct3DConfiguration.presentationParameters.Windowed =
          FALSE;
      } // end IF
      else
      { // toggle to windowed
        configuration_r.configuration->direct3DConfiguration.presentationParameters.hDeviceWindow =
          (HWND)panel_video->GetHandle ();
        configuration_r.configuration->direct3DConfiguration.presentationParameters.Windowed =
          TRUE;
      } // end ELSE
      break;
    }
    case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
    case STREAM_VISUALIZATION_VIDEORENDERER_GDI:
      break;
    //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
    //  break;
#if defined (GTK_USE)
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_PIXBUF:
      break;
#endif // GTK_USE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown video renderer (was: %d), aborting\n"),
                  (*streams_iterator).second.configuration_.renderer));
      return;
    }
  } // end SWITCH

  istream_base_p =
    dynamic_cast<Stream_IStreamControlBase*> ((*iterator).second);
  ACE_ASSERT (istream_base_p);
  if (!istream_base_p->isRunning ())
    return;
  Stream_IStream_t* istream_p =
    dynamic_cast<Stream_IStream_t*> ((*iterator).second);
  ACE_ASSERT (istream_p);
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (istream_p->find (module_name_string.c_str ()));
  ACE_ASSERT (module_p);
  Common_UI_IFullscreen* ifullscreen_p =
    dynamic_cast<Common_UI_IFullscreen*> (module_p->writer ());
  if (!ifullscreen_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s:%s: dynamic_cast<Common_UI_IFullscreen> failed, aborting\n"),
                ACE_TEXT (istream_p->name ().c_str ()),
                ACE_TEXT (module_name_string.c_str ())));
    return;
  } // end IF
  ifullscreen_p->toggle ();
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_switch_camera_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_switch_camera_clicked_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::choice_display_video_format_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_video_format_selected_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::choice_adapter_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_adapter_selected_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::choice_display_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_selected_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_DirectShow_StreamConfigurationsIterator_t streams_iterator =
    configuration_r.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != configuration_r.configuration->streamConfigurations.end ());
  ARDrone_DirectShow_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator =
    //(*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName ((*streams_iterator).second.configuration_.renderer).c_str ()));
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != (*streams_iterator).second.end ());

  wxStringClientData* client_data_p =
    dynamic_cast<wxStringClientData*> (choice_display->GetClientObject (event_in.GetSelection ()));
  ACE_ASSERT (client_data_p);
  (*stream_iterator).second.second.interfaceIdentifier =
    client_data_p->GetData ().ToStdString ();
  ACE_ASSERT (!(*stream_iterator).second.second.interfaceIdentifier.empty ());
  static struct Common_UI_DisplayDevice display_device_s =
    Common_UI_Tools::getDisplay ((*stream_iterator).second.second.interfaceIdentifier);
  ACE_ASSERT (display_device_s.handle != NULL);
  static struct Common_UI_DisplayAdapter display_adapter_s =
    Common_UI_Tools::getAdapter (display_device_s);
  ACE_ASSERT (!display_adapter_s.id.empty ());
  int index_i = choice_adapter->FindString (display_adapter_s.description);
  ACE_ASSERT (index_i != wxNOT_FOUND);
  choice_adapter->Select (index_i);
  wxCommandEvent event_s (wxEVT_COMMAND_CHOICE_SELECTED,
                          XRCID ("choice_adapter"));
  event_s.SetInt (index_i);
  //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
  this->AddPendingEvent (event_s);
  //application_->wait ();

  button_display_settings->Enable (togglebutton_display_video->IsEnabled ());

  Common_UI_Resolutions_t resolutions_a =
    Common_UI_Tools::get ((*stream_iterator).second.second.interfaceIdentifier);
  ACE_ASSERT (!resolutions_a.empty ());
  Common_UI_Resolution_t resolution_s =
    Stream_MediaFramework_DirectShow_Tools::toResolution ((*stream_iterator).second.second.outputFormat);
  Common_UI_Resolution_t resolution_2;

  choice_display_resolution->SetSelection (wxNOT_FOUND);
  choice_display_resolution->Clear ();
  index_i = wxNOT_FOUND;
  std::stringstream converter;
  std::string resolution_string;
  for (Common_UI_ResolutionsConstIterator_t iterator = resolutions_a.begin ();
       iterator != resolutions_a.end ();
       ++iterator)
  {
    client_data_p = NULL;
    ACE_NEW_NORETURN (client_data_p,
                      wxStringClientData ());
    ACE_ASSERT (client_data_p);
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << (*iterator).cx;
    converter << ACE_TEXT_ALWAYS_CHAR (" ");
    converter << (*iterator).cy;
    client_data_p->SetData (converter.str ());

    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << (*iterator).cx;
    converter << ACE_TEXT_ALWAYS_CHAR (" x ");
    converter << (*iterator).cy;

    index_i = choice_display_resolution->Append (converter.str (),
                                                 client_data_p);
#if defined (_DEBUG)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\"%s\": supports: %dx%d\n"),
                ACE_TEXT (choice_display->GetString (choice_display->GetSelection ()).ToStdString ().c_str ()),
                (*iterator).cx, (*iterator).cy));
#endif // _DEBUG
  } // end FOR
  resolution_2 = Common_UI_Tools::nearest (resolutions_a,
                                           resolution_s);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("auto-selecting nearest fullscreen resolution (was: %dx%d): %dx%d\n"),
              resolution_s.cx, resolution_s.cy,
              resolution_2.cx, resolution_2.cy));
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << resolution_2.cx;
  converter << ACE_TEXT_ALWAYS_CHAR (" x ");
  converter << resolution_2.cy;
  resolution_string = converter.str ();
  ACE_ASSERT (!resolution_string.empty ());
  choice_display_resolution->Enable (!resolutions_a.empty () &&
                                     togglebutton_fullscreen->GetValue ());
  index_i = choice_display_resolution->FindString (resolution_string);
  choice_display_resolution->Select (index_i);
  wxCommandEvent event_2 (wxEVT_COMMAND_CHOICE_SELECTED,
                          XRCID ("choice_display_resolution"));
  event_2.SetInt (index_i);
  //choice_resolution_2->GetEventHandler ()->ProcessEvent (event_2);
  this->AddPendingEvent (event_2);
  //application_->wait ();

  if (initializing_)
    initializing_ = false;
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_display_settings_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_display_settings_clicked_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::choice_display_resolution_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_resolution_selected_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::togglebutton_save_video_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_save_video_toggled_cb"));

  bool is_checked_b = event_in.IsChecked ();
  textcontrol_save_video_filename->Enable (is_checked_b);
  directorypicker_save_video->Enable (is_checked_b);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::choice_save_video_format_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_save_video_format_selected_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_snapshot_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_snapshot_clicked_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::directorypicker_save_video_changed_cb (wxFileDirPickerEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::directorypicker_save_video_changed_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_about_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_about_clicked_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);
  ARDrone_DirectShow_WxWidgetsIApplication_t::STATE_T& state_r =
    const_cast<ARDrone_DirectShow_WxWidgetsIApplication_t::STATE_T&> (application_->getR ());
  ACE_ASSERT (state_r.argv);

  std::ostringstream converter;
  wxAboutDialogInfo about_dialog_info;
  about_dialog_info.SetName (_ (Common_File_Tools::basename (wxString (state_r.argv[0]).ToStdString (),
                                                             true).c_str ()));
#if defined (HAVE_CONFIG_H)
  about_dialog_info.SetVersion (_ (ACEStream_PACKAGE_VERSION));
  //about_dialog_info.SetDescription (_ (ACEStream_PACKAGE_DESCRIPTION));
  about_dialog_info.SetDescription (_ ("video capure stream test application"));
#endif // HAVE_CONFIG_H
  std::string copyright_string = ACE_TEXT_ALWAYS_CHAR ("(C) ");
  ACE_Date_Time date_time = COMMON_DATE_NOW;
  converter << date_time.year ();
  copyright_string += converter.str ();
#if defined (HAVE_CONFIG_H)
  copyright_string += ACE_TEXT_ALWAYS_CHAR (" ");
  copyright_string += ACE_TEXT_ALWAYS_CHAR (ACEStream_PACKAGE_MAINTAINER);
  copyright_string += ACE_TEXT_ALWAYS_CHAR (" <");
  copyright_string += ACE_TEXT_ALWAYS_CHAR (ACEStream_PACKAGE_BUGREPORT);
  copyright_string += ACE_TEXT_ALWAYS_CHAR (">");
#endif // HAVE_CONFIG_H
  about_dialog_info.SetCopyright (_ (copyright_string.c_str ()));
#if defined (HAVE_CONFIG_H)
  about_dialog_info.AddDeveloper (_ (ACEStream_PACKAGE_MAINTAINER));
#endif // HAVE_CONFIG_H
  wxAboutBox (about_dialog_info);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>::button_quit_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_quit_clicked_cb"));

  // step1: close main window
  this->Close (true); // force ?

  // step2: initiate shutdown sequence
  int result = ACE_OS::raise (SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));
}

// ---------------------------------------

ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::ARDrone_WxWidgetsDialog_T (wxWindow* parent_in)
 : inherited (parent_in)
 , application_ (NULL)
 , initializing_ (true)
 , untoggling_ (false)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::ARDrone_WxWidgetsDialog_T"));

#if defined (ACE_WIN64) || defined (ACE_WIN32)
  inherited::MSWSetOldWndProc (NULL);
#endif // ACE_WIN64 || ACE_WIN32
}

bool
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::OnInit_2 (IAPPLICATION_T* iapplication_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::OnInit_2"));

  // sanity check(s)
  ACE_ASSERT (!application_);

  application_ =
    dynamic_cast<ARDrone_MediaFoundation_WxWidgetsIApplication_t*> (iapplication_in);
  ACE_ASSERT (application_);

  togglebutton_connect = XRCCTRL (*this, "togglebutton_connect", wxToggleButton);
  button_calibrate = XRCCTRL (*this, "button_calibrate", wxBitmapButton);
  button_animate_leds = XRCCTRL (*this, "button_animate_leds", wxBitmapButton);
  button_report = XRCCTRL (*this, "button_report", wxBitmapButton);
  togglebutton_auto_associate = XRCCTRL (*this, "togglebutton_auto_associate", wxToggleButton);
  choice_interface = XRCCTRL (*this, "choice_interface", wxChoice);
  textcontrol_SSID = XRCCTRL (*this, "textcontrol_SSID", wxTextCtrl);
  textcontrol_address = XRCCTRL (*this, "textcontrol_address", wxTextCtrl);
  spincontrol_port_video = XRCCTRL (*this, "spincontrol_port_video", wxSpinCtrl);
  spincontrol_buffer = XRCCTRL (*this, "spincontrol_buffer", wxSpinCtrl);
  togglebutton_display_video = XRCCTRL (*this, "togglebutton_display_video", wxToggleButton);
  togglebutton_fullscreen = XRCCTRL (*this, "togglebutton_fullscreen", wxToggleButton);
  button_switch_camera = XRCCTRL (*this, "button_switch_camera", wxBitmapButton);
  choice_adapter = XRCCTRL (*this, "choice_adapter", wxChoice);
  choice_display = XRCCTRL (*this, "choice_display", wxChoice);
  button_display_settings = XRCCTRL (*this, "button_display_settings", wxBitmapButton);
  choice_display_video_format = XRCCTRL (*this, "choice_display_video_format", wxChoice);
  togglebutton_save_video = XRCCTRL (*this, "togglebutton_save_video", wxToggleButton);
  choice_save_video_format = XRCCTRL (*this, "choice_save_video_format", wxChoice);
  button_snapshot = XRCCTRL (*this, "button_snapshot", wxBitmapButton);
  textcontrol_save_video_filename = XRCCTRL (*this, "textcontrol_save_video_filename", wxTextCtrl);
  directorypicker_save_video = XRCCTRL (*this, "directorypicker_save_video", wxDirPickerCtrl);
  panel_video = XRCCTRL (*this, "panel_video", wxPanel);
  spincontrol_control_session_messages = XRCCTRL (*this, "spincontrol_control_session_messages", wxSpinCtrl);
  spincontrol_control_data_messages = XRCCTRL (*this, "spincontrol_control_data_messages", wxSpinCtrl);
  control_gauge = XRCCTRL (*this, "control_gauge", wxGauge);
  spincontrol_navdata_session_messages = XRCCTRL (*this, "spincontrol_navdata_session_messages", wxSpinCtrl);
  spincontrol_navdata_data_messages = XRCCTRL (*this, "spincontrol_navdata_data_messages", wxSpinCtrl);
  navdata_gauge = XRCCTRL (*this, "navdata_gauge", wxGauge);
  spincontrol_mavlink_session_messages = XRCCTRL (*this, "spincontrol_mavlink_session_messages", wxSpinCtrl);
  spincontrol_mavlink_data_messages = XRCCTRL (*this, "spincontrol_mavlink_data_messages", wxSpinCtrl);
  mavlink_gauge = XRCCTRL (*this, "mavlink_gauge", wxGauge);
  spincontrol_video_session_messages = XRCCTRL (*this, "spincontrol_video_session_messages", wxSpinCtrl);
  spincontrol_video_data_messages = XRCCTRL (*this, "spincontrol_video_data_messages", wxSpinCtrl);
  video_gauge = XRCCTRL (*this, "video_gauge", wxGauge);
  spincontrol_connections = XRCCTRL (*this, "spincontrol_connections", wxSpinCtrl);
  spincontrol_data = XRCCTRL (*this, "spincontrol_data", wxSpinCtrl);
  cache_gauge = XRCCTRL (*this, "cache_gauge", wxGauge);
  label_roll_value = XRCCTRL (*this, "label_roll_value", wxStaticText);
  label_pitch_value = XRCCTRL (*this, "label_pitch_value", wxStaticText);
  label_yaw_value = XRCCTRL (*this, "label_yaw_value", wxStaticText);
  button_about = XRCCTRL (*this, "button_about", wxBitmapButton);
  button_quit = XRCCTRL (*this, "button_quit", wxBitmapButton);
  animation_associating = XRCCTRL (*this, "animation_associating", wxAnimationCtrl);
  gauge_progress = XRCCTRL (*this, "gauge_progress", wxGauge);

  this->SetDefaultItem (togglebutton_connect);

  // populate controls
#if defined (_DEBUG)
#else
  button_report->Show (false);
#endif // _DEBUG
  spincontrol_port_video->SetRange (0,
                                    std::numeric_limits<int>::max ());
  spincontrol_buffer->SetRange (0,
                                std::numeric_limits<int>::max ());
  spincontrol_control_session_messages->SetRange (0,
                                                  std::numeric_limits<int>::max ());
  spincontrol_control_data_messages->SetRange (0,
                                               std::numeric_limits<int>::max ());
  spincontrol_navdata_session_messages->SetRange (0,
                                                  std::numeric_limits<int>::max ());
  spincontrol_navdata_data_messages->SetRange (0,
                                               std::numeric_limits<int>::max ());
  spincontrol_mavlink_session_messages->SetRange (0,
                                                  std::numeric_limits<int>::max ());
  spincontrol_mavlink_data_messages->SetRange (0,
                                               std::numeric_limits<int>::max ());
  spincontrol_video_session_messages->SetRange (0,
                                                std::numeric_limits<int>::max ());
  spincontrol_video_data_messages->SetRange (0,
                                             std::numeric_limits<int>::max ());
  spincontrol_connections->SetRange (0,
                                     std::numeric_limits<int>::max ());
  spincontrol_data->SetRange (0,
                              std::numeric_limits<int>::max ());

  bool activate_interface_b = true, activate_display_b = true;
  ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t streams_iterator =
    configuration_r.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != configuration_r.configuration->streamConfigurations.end ());
  ARDrone_MediaFoundation_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator =
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_iterator != (*streams_iterator).second.end ());
  ARDrone_MediaFoundation_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator_2 =
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName ((*streams_iterator).second.configuration_.renderer).c_str ()));
  ACE_ASSERT (stream_iterator_2 != (*streams_iterator).second.end ());
  ARDrone_MediaFoundation_ConnectionConfigurationIterator_t iterator =
    configuration_r.configuration->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.configuration->connectionConfigurations.end ());
  ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t iterator_2 =
    (*iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (iterator_2 != (*iterator).second.end ());

  togglebutton_auto_associate->Enable (!(*stream_iterator).second.second.interfaceIdentifier.empty ());
  togglebutton_auto_associate->SetValue (configuration_r.configuration->WLANMonitorConfiguration.autoAssociate);

  Net_InterfaceIdentifiers_t interfaces_a =
    Net_WLAN_Tools::getInterfaces (ARDRONE_WLANMONITOR_SINGLETON::instance ()->get_2 ());

  int index_i = -1;
  wxStringClientData* client_data_p = NULL;
  for (Net_InterfacesIdentifiersIterator_t iterator = interfaces_a.begin ();
       iterator != interfaces_a.end ();
       ++iterator)
  {
    client_data_p = NULL;
    ACE_NEW_NORETURN (client_data_p,
                      wxStringClientData ());
    ACE_ASSERT (client_data_p);
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
    client_data_p->SetData (Common_Tools::GUIDToString (*iterator).c_str ());
#else
    client_data_p->SetData (*iterator);
#endif // _WIN32_WINNT_VISTA
    index_i =
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
      choice_interface->Append (Net_Common_Tools::interfaceToString (*iterator).c_str (),
#else
      choice_interface->Append ((*iterator).c_str (),
#endif // _WIN32_WINNT_VISTA
                                client_data_p);
  } // end FOR
  if (unlikely (interfaces_a.empty ()))
    activate_interface_b = false;
  else
    choice_interface->Enable (true);
  textcontrol_SSID->Enable (!configuration_r.configuration->WLANMonitorConfiguration.SSID.empty ());
  textcontrol_SSID->SetValue (configuration_r.configuration->WLANMonitorConfiguration.SSID.c_str ());
  const Net_WLAN_AccessPointCacheValue_t& access_point_s =
    ARDRONE_WLANMONITOR_SINGLETON::instance ()->get1RR (configuration_r.configuration->WLANMonitorConfiguration.SSID);
  textcontrol_address->Enable (!access_point_s.second.IPAddress.is_any ());
  textcontrol_address->SetValue (Net_Common_Tools::IPAddressToString (access_point_s.second.IPAddress, true).c_str ());
  spincontrol_port_video->Enable (true);
  spincontrol_port_video->SetValue ((*iterator_2).second.socketHandlerConfiguration.socketConfiguration_2.address.get_port_number ());
  spincontrol_buffer->Enable (true);
  spincontrol_buffer->SetValue ((*streams_iterator).second.allocatorConfiguration_.defaultBufferSize);

  Common_UI_DisplayAdapters_t display_adapters_a =
    Common_UI_Tools::getAdapters ();
  for (Common_UI_DisplayAdaptersIterator_t iterator = display_adapters_a.begin ();
       iterator != display_adapters_a.end ();
       ++iterator)
  {
    client_data_p = NULL;
    ACE_NEW_NORETURN (client_data_p,
                      wxStringClientData ());
    ACE_ASSERT (client_data_p);
    client_data_p->SetData ((*iterator).id);

    index_i =
      choice_adapter->Append ((*iterator).description.c_str (),
                              client_data_p);
  } // end FOR
  Common_UI_DisplayDevices_t display_interfaces_a =
    Common_UI_Tools::getDisplays ();
  for (Common_UI_DisplayDevicesIterator_t iterator = display_interfaces_a.begin ();
       iterator != display_interfaces_a.end ();
       ++iterator)
  {
    client_data_p = NULL;
    ACE_NEW_NORETURN (client_data_p,
                      wxStringClientData ());
    ACE_ASSERT (client_data_p);
    client_data_p->SetData ((*iterator).device);

    index_i =
      choice_display->Append ((*iterator).description.c_str (),
                              client_data_p);
  } // end FOR
  std::stringstream converter;
  client_data_p = NULL;
  ACE_NEW_NORETURN (client_data_p,
                    wxStringClientData ());
  ACE_ASSERT (client_data_p);
  converter << ARDRONE_VIDEOMODE_360P;
  client_data_p->SetData (converter.str ());
  index_i =
    choice_display_video_format->Append (ARDroneVideoModeToString (ARDRONE_VIDEOMODE_360P).c_str (),
                                         client_data_p);
  client_data_p = NULL;
  ACE_NEW_NORETURN (client_data_p,
                    wxStringClientData ());
  ACE_ASSERT (client_data_p);
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << ARDRONE_VIDEOMODE_720P;
  client_data_p->SetData (converter.str ());
  index_i =
    choice_display_video_format->Append (ARDroneVideoModeToString (ARDRONE_VIDEOMODE_720P).c_str (),
                                         client_data_p);
  if (likely (!display_interfaces_a.empty ()))
  {
    togglebutton_display_video->Enable (!(*stream_iterator_2).second.second.interfaceIdentifier.empty ());
    togglebutton_display_video->SetValue (!(*stream_iterator_2).second.second.interfaceIdentifier.empty ());
    togglebutton_fullscreen->Enable (togglebutton_display_video->GetValue ());
    ACE_ASSERT ((*stream_iterator_2).second.second.direct3DConfiguration);
    togglebutton_fullscreen->SetValue (!(*stream_iterator_2).second.second.direct3DConfiguration->presentationParameters.Windowed);
    panel_video->Show (togglebutton_display_video->GetValue () &&
                       !togglebutton_fullscreen->GetValue ());
  } // end IF
  if (unlikely (interfaces_a.empty ()))
    activate_display_b = false;
  else
    choice_display->Enable (true);

  togglebutton_save_video->Enable (!(*stream_iterator).second.second.targetFileName.empty ());
  togglebutton_save_video->SetValue (!(*stream_iterator).second.second.targetFileName.empty ());
  textcontrol_save_video_filename->Enable (togglebutton_save_video->GetValue ());
  textcontrol_save_video_filename->SetValue (ACE_TEXT_ALWAYS_CHAR (ACE::basename ((*stream_iterator).second.second.targetFileName.c_str (),
                                                                                  ACE_DIRECTORY_SEPARATOR_CHAR)));
  directorypicker_save_video->Enable (togglebutton_save_video->GetValue ());
  directorypicker_save_video->SetPath (ACE_TEXT_ALWAYS_CHAR (ACE::dirname ((*stream_iterator).second.second.targetFileName.c_str (),
                                                                           ACE_DIRECTORY_SEPARATOR_CHAR)));

  if (likely (activate_interface_b))
  {
    index_i =
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
      (initializing_ ? Common_UI_WxWidgets_Tools::clientDataToIndex (choice_interface,
                                                                     (*stream_iterator).second.second.interfaceIdentifier)
#else
      (initializing_ ? choice_interface->FindString ((*stream_iterator).second.second.interfaceIdentifier)
#endif // _WIN32_WINNT_VISTA
                     : 0);
    ACE_ASSERT (index_i != wxNOT_FOUND);
    choice_interface->Select (index_i);
    wxCommandEvent event_s (wxEVT_COMMAND_CHOICE_SELECTED,
                            XRCID ("choice_interface"));
    event_s.SetInt (index_i);
    //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
    this->AddPendingEvent (event_s);
  } // end IF
  application_->wait ();

  if (likely (activate_display_b))
  {
    index_i =
//#if defined (UNICODE)
//      (initializing_ ? Common_UI_WxWidgets_Tools::clientDataToIndex (choice_display,
//                                                                     ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR ((*stream_iterator_2).second.second.interfaceIdentifier.c_str ())))
//#else
      (initializing_ ? Common_UI_WxWidgets_Tools::clientDataToIndex (choice_display,
                                                                     (*stream_iterator_2).second.second.interfaceIdentifier)
//#endif // UNICODE
                     : 0);
    ACE_ASSERT (index_i != wxNOT_FOUND);
    choice_display->Select (index_i);
    wxCommandEvent event_s (wxEVT_COMMAND_CHOICE_SELECTED,
                            XRCID ("choice_display"));
    event_s.SetInt (index_i);
    //choice_interface->GetEventHandler ()->ProcessEvent (event_s);
    this->AddPendingEvent (event_s);
    wxCommandEvent event_2 (wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
                            XRCID ("togglebutton_display"));
    event_2.SetInt (!(*stream_iterator_2).second.second.interfaceIdentifier.empty () ? 1
                                                                                     : 0);
    //togglebutton_display_video->GetEventHandler ()->ProcessEvent (event_2);
    this->AddPendingEvent (event_2);
    wxCommandEvent event_3 (wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
                            XRCID ("togglebutton_fullscreen"));
    ACE_ASSERT ((*stream_iterator_2).second.second.direct3DConfiguration);
    event_3.SetInt ((*stream_iterator_2).second.second.direct3DConfiguration->presentationParameters.Windowed ? 0 : 1);
    //togglebutton_fullscreen->GetEventHandler ()->ProcessEvent (event_3);
    this->AddPendingEvent (event_3);
  } // end IF

  return true;
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::OnExit_2 ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::OnExit_2"));

  // sanity check(s)
  ACE_ASSERT (application_);
}

//////////////////////////////////////////

void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::dialog_main_idle_cb (wxIdleEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::dialog_main_idle_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ARDrone_StreamsIterator_t iterator = configuration_r.streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.streams.end ());
  Stream_IStreamControlBase* istream_p =
    dynamic_cast<Stream_IStreamControlBase*> ((*iterator).second);
  bool finished_b = false;

  // sanity check(s)
  ACE_ASSERT (istream_p);

  process_stream_events (&configuration_r,
                         finished_b);
  if (!finished_b &&
      istream_p->isRunning ())
    gauge_progress->Pulse ();
}

void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::dialog_main_keydown_cb (wxKeyEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::dialog_main_keydown_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
}

//////////////////////////////////////////

void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::togglebutton_connect_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_connect_toggled_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_calibrate_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_calibrate_clicked_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_animate_LEDs_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_animate_LEDs_clicked_cb"));

}
#if defined (_DEBUG)
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_report_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_report_clicked_cb"));

}
#endif // _DEBUG
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::togglebutton_associate_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_associate_toggled_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::choice_interface_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_interface_selected_cb"));

  // sanity check(s)
  ACE_ASSERT (application_);

  ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T& configuration_r =
    const_cast<ARDrone_MediaFoundation_WxWidgetsIApplication_t::CONFIGURATION_T&> (application_->getR_2 ());
  ARDrone_StreamsIterator_t iterator = configuration_r.streams.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != configuration_r.streams.end ());
  Stream_IStream_t* istream_p =
    dynamic_cast<Stream_IStream_t*> ((*iterator).second);
  ACE_ASSERT (istream_p);
  ACE_ASSERT (configuration_r.configuration);
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t streams_iterator =
    configuration_r.configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (streams_iterator != configuration_r.configuration->streamConfigurations.end ());
  ARDrone_MediaFoundation_AsynchVideoStream_t::CONFIGURATION_T::ITERATOR_T stream_iterator =
    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_ALWAYS_CHAR ("")));
  ACE_ASSERT (stream_iterator != (*streams_iterator).second.end ());

  std::string interface_identifier;
  int index_i = -1;

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
  ACE_ASSERT (!interface_identifier.empty ());

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0602) // _WIN32_WINNT_WIN8
  IMFMediaSourceEx* media_source_p = NULL;
#else
  IMFMediaSource* media_source_p = NULL;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0602)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0601) // _WIN32_WINNT_WIN7
  if (!Stream_Device_MediaFoundation_Tools::getMediaSource (interface_identifier,
                                                            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID,
                                                            media_source_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Device_MediaFoundation_Tools::getMediaSource(\"%s\"), returning\n"),
                ACE_TEXT (interface_identifier.c_str ())));
    return;
  } // end IF
  ACE_ASSERT (media_source_p);
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0601)

  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (istream_p->find (ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_NULL_DEFAULT_NAME_STRING)));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Base_T::find(\"%s\"), returning\n"),
                ACE_TEXT (STREAM_VIS_NULL_DEFAULT_NAME_STRING)));
    media_source_p->Release (); media_source_p = NULL;
    return;
  } // end IF
  //Stream_CamSave_MediaFoundation_MediaFoundationDisplayNull* display_impl_p =
  //  dynamic_cast<ARDrone_MediaFoundation_MediaFoundationDisplayNull*> (module_p->writer ());
  //ACE_ASSERT (display_impl_p);

  IMFTopology* topology_p = NULL;
  struct _MFRatio pixel_aspect_ratio = { 1, 1 };
  IMFSampleGrabberSinkCallback* isample_grabber_p = NULL;
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0601) // _WIN32_WINNT_WIN7
  if (!Stream_Device_MediaFoundation_Tools::loadDeviceTopology (interface_identifier,
                                                                MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID,
                                                                media_source_p,
                                                                isample_grabber_p,
                                                                topology_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Device_MediaFoundation_Tools::loadDeviceTopology(), returning\n")));
    media_source_p->Release (); media_source_p = NULL;
    return;
  } // end IF
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0601)
  ACE_ASSERT (topology_p);
  media_source_p->Release (); media_source_p = NULL;

  // sanity check(s)
  ACE_ASSERT ((*stream_iterator).second.second.session);

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  if (!Stream_MediaFramework_MediaFoundation_Tools::setTopology (topology_p,
                                                                 (*stream_iterator).second.second.session,
                                                                 true,
                                                                 true))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_MediaFramework_MediaFoundation_Tools::setTopology(), returning\n")));
    topology_p->Release (); topology_p = NULL;
    return;
  } // end IF
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  topology_p->Release (); topology_p = NULL;

  if ((*stream_iterator).second.second.outputFormat)
  {
    (*stream_iterator).second.second.outputFormat->Release (); (*stream_iterator).second.second.outputFormat = NULL;
  } // end IF
  HRESULT result_2 =
    MFCreateMediaType (&(*stream_iterator).second.second.outputFormat);
  if (FAILED (result_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to MFCreateMediaType(): \"%s\", returning\n"),
                ACE_TEXT (Common_Error_Tools::errorToString (result_2).c_str ())));
    return;
  } // end IF
  ACE_ASSERT ((*stream_iterator).second.second.outputFormat);
  result_2 =
    (*stream_iterator).second.second.outputFormat->SetGUID (MF_MT_MAJOR_TYPE,
                                                            MFMediaType_Video);
  ACE_ASSERT (SUCCEEDED (result_2));
  result_2 =
    (*stream_iterator).second.second.outputFormat->SetUINT32 (MF_MT_INTERLACE_MODE,
                                                              MFVideoInterlace_Unknown);
  ACE_ASSERT (SUCCEEDED (result_2));
  result_2 =
    MFSetAttributeRatio ((*stream_iterator).second.second.outputFormat,
                          MF_MT_PIXEL_ASPECT_RATIO,
                          pixel_aspect_ratio.Numerator,
                          pixel_aspect_ratio.Denominator);
  ACE_ASSERT (SUCCEEDED (result_2));

  //if (_DEBUG)
  //{
  //  std::string log_file_name =
  //    Common_File_Tools::getLogDirectory (std::string (),
  //                                        0);
  //  log_file_name += ACE_DIRECTORY_SEPARATOR_STR;
  //  log_file_name += MODULE_DEV_DIRECTSHOW_LOGFILE_NAME;
  //  Stream_Device_Tools::debug (data_p->configuration->moduleHandlerConfiguration.builder,
  //                                     log_file_name);
  //} // end IF
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::spincontrol_port_changed_cb (wxSpinEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::spincontrol_port_changed_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::togglebutton_display_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_display_toggled_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::togglebutton_fullscreen_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_fullscreen_toggled_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_switch_camera_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_switch_camera_clicked_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::choice_display_video_format_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_video_format_selected_cb"));

}

void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::choice_adapter_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_adapter_selected_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::choice_display_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_selected_cb"));

}

void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_display_settings_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_display_settings_clicked_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::choice_display_resolution_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_display_resolution_selected_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::togglebutton_save_video_toggled_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::togglebutton_save_video_toggled_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::choice_save_video_format_selected_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::choice_save_video_format_selected_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_snapshot_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_snapshot_clicked_cb"));

}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::directorypicker_save_video_changed_cb (wxFileDirPickerEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::directorypicker_save_video_changed_cb"));

}

void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_about_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_about_clicked_cb"));

  wxAboutDialogInfo about_dialog_info;
  about_dialog_info.SetName (_ ("My Program"));
  about_dialog_info.SetVersion (_ ("1.2.3 Beta"));
  about_dialog_info.SetDescription (_ ("This program does something great."));
  about_dialog_info.SetCopyright (wxT ("(C) 2007 Me <my@email.addre.ss>"));
  wxAboutBox (about_dialog_info);
}
void
ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>::button_quit_clicked_cb (wxCommandEvent& event_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_WxWidgetsDialog_T::button_quit_clicked_cb"));

  // step2: close main window
  this->Close (true); // force ?

  // step3: initiate shutdown sequence
  int result = ACE_OS::raise (SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::raise(%S): \"%m\", continuing\n"),
                SIGINT));
}
#endif // ACE_WIN32 || ACE_WIN64
