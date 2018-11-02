#ifndef ARDRONE_UI_H
#define ARDRONE_UI_H

#include "wx/wx.h"
#include "wx/apptrait.h"

#include "ace/config-macros.h"

#include "common_ui_wxwidgets_application.h"
#include "common_ui_wxwidgets_common.h"
#include "common_ui_wxwidgets_itoplevel.h"
#include "common_ui_wxwidgets_manager.h"
#include "common_ui_wxwidgets_xrc_definition.h"

#include "ardrone_configuration.h"
#include "ardrone_ui_base.h"

// helper functions
void process_stream_events (struct ARDrone_UI_CBData_Base*, bool&);

// thread functions
//ACE_THR_FUNC_RETURN event_processing_thread (void*);
ACE_THR_FUNC_RETURN stream_processing_thread (void*);

template <typename InterfaceType> // implements Common_UI_wxWidgets_IApplication_T
class ARDrone_WxWidgetsDialog_T
 : public dialog_main
 , public Common_UI_wxWidgets_ITopLevel_T<typename InterfaceType::STATE_T,
                                          typename InterfaceType::CONFIGURATION_T>
{
  typedef dialog_main inherited;
  //typedef Common_UI_wxWidgets_ITopLevel_T<typename InterfaceType::STATE_T,
  //                                        typename InterfaceType::CONFIGURATION_T> inherited2;

 public:
  // convenient types
  typedef Common_UI_wxWidgets_IApplication_T<typename InterfaceType::STATE_T,
                                             typename InterfaceType::CONFIGURATION_T> IAPPLICATION_T;

  ARDrone_WxWidgetsDialog_T (wxWindow* = NULL); // parent window (if any)
  inline virtual ~ARDrone_WxWidgetsDialog_T () {}

  // implement Common_UI_wxWidgets_ITopLevel
  inline virtual const IAPPLICATION_T* const getP () const { ACE_ASSERT (application_); return application_; }

 private:
  // convenient types
  typedef ARDrone_WxWidgetsDialog_T<InterfaceType> OWN_TYPE_T;

  // implement Common_UI_wxWidgets_ITopLevel
  virtual bool OnInit_2 (IAPPLICATION_T*);
  virtual void OnExit_2 ();

  // event handlers
  virtual void dialog_main_idle_cb (wxIdleEvent&);
  virtual void dialog_main_keydown_cb (wxKeyEvent&);

  // control handlers
  virtual void togglebutton_connect_toggled_cb (wxCommandEvent&);
  virtual void button_calibrate_clicked_cb (wxCommandEvent&);
  virtual void button_animate_LEDs_clicked_cb (wxCommandEvent&);
#if defined (_DEBUG)
  virtual void button_report_clicked_cb (wxCommandEvent&);
#endif // _DEBUG
  virtual void togglebutton_associate_toggled_cb (wxCommandEvent&);
  virtual void choice_interface_selected_cb (wxCommandEvent&);
  virtual void spincontrol_port_changed_cb (wxSpinEvent&);
  virtual void togglebutton_display_toggled_cb (wxCommandEvent&);
  virtual void togglebutton_fullscreen_toggled_cb (wxCommandEvent&);
  virtual void button_switch_camera_clicked_cb (wxCommandEvent&);
  virtual void choice_display_video_format_selected_cb (wxCommandEvent&);
  virtual void choice_adapter_selected_cb (wxCommandEvent&);
  virtual void choice_display_selected_cb (wxCommandEvent&);
  virtual void button_display_settings_clicked_cb (wxCommandEvent&);
  virtual void choice_display_resolution_selected_cb (wxCommandEvent&);
  virtual void togglebutton_save_video_toggled_cb (wxCommandEvent&);
  virtual void choice_save_video_format_selected_cb (wxCommandEvent&);
  virtual void button_snapshot_clicked_cb (wxCommandEvent&);
  virtual void directorypicker_save_video_changed_cb (wxFileDirPickerEvent&);
  virtual void button_about_clicked_cb (wxCommandEvent&);
  virtual void button_quit_clicked_cb (wxCommandEvent&);

  wxDECLARE_DYNAMIC_CLASS (OWN_TYPE_T);
  wxDECLARE_EVENT_TABLE ();

  InterfaceType* application_;
  bool           initializing_;
  bool           untoggling_;
};

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
// specializations (for Win32)
template <>
class ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t>
 : public dialog_main
 , public Common_UI_wxWidgets_ITopLevel_T<struct ARDrone_UI_wxWidgets_State,
                                          struct ARDrone_DirectShow_UI_CBData>
{
  typedef dialog_main inherited;
  //typedef Common_UI_wxWidgets_ITopLevel_T<struct Common_UI_wxWidgets_State,
  //                                        struct ARDrone_DirectShow_UI_CBData> inherited2;

 public:
  // convenient types
  typedef Common_UI_wxWidgets_IApplication_T<struct ARDrone_UI_wxWidgets_State,
                                             struct ARDrone_DirectShow_UI_CBData> IAPPLICATION_T;

  ARDrone_WxWidgetsDialog_T (wxWindow* = NULL); // parent window (if any)
  inline virtual ~ARDrone_WxWidgetsDialog_T () {}

  // implement Common_UI_wxWidgets_ITopLevel
  inline virtual const IAPPLICATION_T* const getP () const { ACE_ASSERT (application_); return application_; }

 private:
  // convenient types
  typedef ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t> OWN_TYPE_T;

  // implement Common_UI_wxWidgets_ITopLevel
  virtual bool OnInit_2 (IAPPLICATION_T*);
  virtual void OnExit_2 ();

  // event handlers
  virtual void dialog_main_idle_cb (wxIdleEvent&);
  virtual void dialog_main_keydown_cb (wxKeyEvent&);

  // control handlers
  virtual void togglebutton_connect_toggled_cb (wxCommandEvent&);
  virtual void button_calibrate_clicked_cb (wxCommandEvent&);
  virtual void button_animate_LEDs_clicked_cb (wxCommandEvent&);
#if defined (_DEBUG)
  virtual void button_report_clicked_cb (wxCommandEvent&);
#endif // _DEBUG
  virtual void togglebutton_associate_toggled_cb (wxCommandEvent&);
  virtual void choice_interface_selected_cb (wxCommandEvent&);
  virtual void spincontrol_port_changed_cb (wxSpinEvent&);
  virtual void togglebutton_display_toggled_cb (wxCommandEvent&);
  virtual void togglebutton_fullscreen_toggled_cb (wxCommandEvent&);
  virtual void button_switch_camera_clicked_cb (wxCommandEvent&);
  virtual void choice_display_video_format_selected_cb (wxCommandEvent&);
  virtual void choice_adapter_selected_cb (wxCommandEvent&);
  virtual void choice_display_selected_cb (wxCommandEvent&);
  virtual void button_display_settings_clicked_cb (wxCommandEvent&);
  virtual void choice_display_resolution_selected_cb (wxCommandEvent&);
  virtual void togglebutton_save_video_toggled_cb (wxCommandEvent&);
  virtual void choice_save_video_format_selected_cb (wxCommandEvent&);
  virtual void button_snapshot_clicked_cb (wxCommandEvent&);
  virtual void directorypicker_save_video_changed_cb (wxFileDirPickerEvent&);
  virtual void button_about_clicked_cb (wxCommandEvent&);
  virtual void button_quit_clicked_cb (wxCommandEvent&);

  wxDECLARE_DYNAMIC_CLASS (OWN_TYPE_T);
  wxDECLARE_EVENT_TABLE ();

  ARDrone_DirectShow_WxWidgetsIApplication_t* application_;
  bool                                        initializing_;
  bool                                        reset_; // direct3d device-
  bool                                        untoggling_;
};

template <>
class ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t>
 : public dialog_main
 , public Common_UI_wxWidgets_ITopLevel_T<struct ARDrone_UI_wxWidgets_State,
                                          struct ARDrone_MediaFoundation_UI_CBData>
{
  typedef dialog_main inherited;
  //typedef Common_UI_wxWidgets_ITopLevel_T<struct ARDrone_UI_wxWidgets_State,
  //                                        struct ARDrone_MediaFoundation_UI_CBData> inherited2;

 public:
  // convenient types
  typedef Common_UI_wxWidgets_IApplication_T<struct ARDrone_UI_wxWidgets_State,
                                             struct ARDrone_MediaFoundation_UI_CBData> IAPPLICATION_T;

  ARDrone_WxWidgetsDialog_T (wxWindow* = NULL); // parent window (if any)
  inline virtual ~ARDrone_WxWidgetsDialog_T () {}

   // implement Common_UI_wxWidgets_ITopLevel
  inline virtual const IAPPLICATION_T* const getP () const { ACE_ASSERT (application_); return application_; }

 private:
  // convenient types
  typedef ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t> OWN_TYPE_T;

  // implement Common_UI_wxWidgets_ITopLevel
  virtual bool OnInit_2 (IAPPLICATION_T*);
  virtual void OnExit_2 ();

  // event handlers
  virtual void dialog_main_idle_cb (wxIdleEvent&);
  virtual void dialog_main_keydown_cb (wxKeyEvent&);

  // control handlers
  virtual void togglebutton_connect_toggled_cb (wxCommandEvent&);
  virtual void button_calibrate_clicked_cb (wxCommandEvent&);
  virtual void button_animate_LEDs_clicked_cb (wxCommandEvent&);
#if defined (_DEBUG)
  virtual void button_report_clicked_cb (wxCommandEvent&);
#endif // _DEBUG
  virtual void togglebutton_associate_toggled_cb (wxCommandEvent&);
  virtual void choice_interface_selected_cb (wxCommandEvent&);
  virtual void spincontrol_port_changed_cb (wxSpinEvent&);
  virtual void togglebutton_display_toggled_cb (wxCommandEvent&);
  virtual void togglebutton_fullscreen_toggled_cb (wxCommandEvent&);
  virtual void button_switch_camera_clicked_cb (wxCommandEvent&);
  virtual void choice_display_video_format_selected_cb (wxCommandEvent&);
  virtual void choice_adapter_selected_cb (wxCommandEvent&);
  virtual void choice_display_selected_cb (wxCommandEvent&);
  virtual void button_display_settings_clicked_cb (wxCommandEvent&);
  virtual void choice_display_resolution_selected_cb (wxCommandEvent&);
  virtual void togglebutton_save_video_toggled_cb (wxCommandEvent&);
  virtual void choice_save_video_format_selected_cb (wxCommandEvent&);
  virtual void button_snapshot_clicked_cb (wxCommandEvent&);
  virtual void directorypicker_save_video_changed_cb (wxFileDirPickerEvent&);
  virtual void button_about_clicked_cb (wxCommandEvent&);
  virtual void button_quit_clicked_cb (wxCommandEvent&);

  wxDECLARE_DYNAMIC_CLASS (OWN_TYPE_T);
  wxDECLARE_EVENT_TABLE ();

  ARDrone_MediaFoundation_WxWidgetsIApplication_t* application_;
  bool                                             initializing_;
  bool                                             reset_; // direct3d device-
  bool                                             untoggling_;
};
#endif // ACE_WIN32 || ACE_WIN64

// include template definition
#include "ardrone_ui.inl"

//////////////////////////////////////////

extern const char toplevel_widget_classname_string_[];
typedef Common_UI_WxWidgetsXRCDefinition_T<struct ARDrone_UI_wxWidgets_State,
                                           toplevel_widget_classname_string_> ARDrone_WxWidgetsXRCDefinition_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ARDrone_WxWidgetsDialog_T<ARDrone_DirectShow_WxWidgetsIApplication_t> ARDrone_DirectShow_WxWidgetsDialog_t;
typedef Comon_UI_WxWidgets_Application_T<ARDrone_WxWidgetsXRCDefinition_t,
                                         struct ARDrone_UI_wxWidgets_State,
                                         struct ARDrone_DirectShow_UI_CBData,
                                         ARDrone_DirectShow_WxWidgetsDialog_t,
                                         wxGUIAppTraits> ARDrone_DirectShow_WxWidgetsApplication_t;
typedef Common_UI_WxWidgets_Manager_T<ARDrone_DirectShow_WxWidgetsApplication_t> ARDrone_DirectShow_WxWidgetsManager_t;

typedef ARDrone_WxWidgetsDialog_T<ARDrone_MediaFoundation_WxWidgetsIApplication_t> ARDrone_MediaFoundation_WxWidgetsDialog_t;
typedef Comon_UI_WxWidgets_Application_T<ARDrone_WxWidgetsXRCDefinition_t,
                                         struct ARDrone_UI_wxWidgets_State,
                                         struct ARDrone_MediaFoundation_UI_CBData,
                                         ARDrone_MediaFoundation_WxWidgetsDialog_t,
                                         wxGUIAppTraits> ARDrone_MediaFoundation_WxWidgetsApplication_t;
typedef Common_UI_WxWidgets_Manager_T<ARDrone_MediaFoundation_WxWidgetsApplication_t> ARDrone_MediaFoundation_WxWidgetsManager_t;
#else
typedef ARDrone_WxWidgetsDialog_T<ARDrone_WxWidgetsIApplication_t> ARDrone_WxWidgetsDialog_t;
typedef Comon_UI_WxWidgets_Application_T<ARDrone_WxWidgetsXRCDefinition_t,
                                         struct ARDrone_UI_wxWidgets_State,
                                         struct ARDrone_UI_CBData,
                                         ARDrone_WxWidgetsDialog_t,
                                         wxGUIAppTraits> ARDrone_WxWidgetsApplication_t;
typedef Common_UI_WxWidgets_Manager_T<ARDrone_WxWidgetsApplication_t> ARDrone_WxWidgetsManager_t;
#endif // ACE_WIN32 || ACE_WIN64

#endif // ardrone_ui_h
