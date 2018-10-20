///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug  8 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ARDRONE_UI_BASE_H__
#define __ARDRONE_UI_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/tglbtn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bmpbuttn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
#include <wx/filepicker.h>
#include <wx/stattext.h>
#include <wx/gauge.h>
#include <wx/statline.h>
#include <wx/animate.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class dialog_main
///////////////////////////////////////////////////////////////////////////////
class dialog_main : public wxDialog 
{
	private:
	
	protected:
		wxToggleButton* togglebutton_connect;
		wxBitmapButton* button_calibrate;
		wxBitmapButton* button_animate_leds;
		wxBitmapButton* button_report;
		wxPanel* m_panel5;
		wxToggleButton* togglebutton_auto_associate;
		wxChoice* choice_interface;
		wxTextCtrl* textcontrol_SSID;
		wxTextCtrl* textcontrol_address;
		wxSpinCtrl* spincontrol_port_video;
		wxSpinCtrl* spincontrol_buffer;
		wxPanel* m_panel4;
		wxToggleButton* togglebutton_display_video;
		wxToggleButton* togglebutton_fullscreen;
		wxBitmapButton* button_switch_camera;
		wxChoice* choice_display_video_format;
		wxChoice* choice_adapter;
		wxChoice* choice_display;
		wxBitmapButton* button_display_settings;
		wxChoice* choice_display_resolution;
		wxPanel* m_panel3;
		wxToggleButton* togglebutton_save_video;
		wxBitmapButton* button_snapshot;
		wxChoice* choice_save_video_format;
		wxTextCtrl* textcontrol_save_video_filename;
		wxDirPickerCtrl* directorypicker_save_video;
		wxPanel* panel_video;
		wxPanel* m_panel6;
		wxStaticText* label_control;
		wxSpinCtrl* spincontrol_control_session_messages;
		wxSpinCtrl* spincontrol_control_data_messages;
		wxGauge* control_gauge;
		wxStaticText* label_navdata;
		wxSpinCtrl* spincontrol_navdata_session_messages;
		wxSpinCtrl* spincontrol_navdata_data_messages;
		wxGauge* navdata_gauge;
		wxStaticText* label_mavlink;
		wxSpinCtrl* spincontrol_mavlink_session_messages;
		wxSpinCtrl* spincontrol_mavlink_data_messages;
		wxGauge* mavlink_gauge;
		wxStaticText* label_video;
		wxSpinCtrl* spincontrol_video_session_messages;
		wxSpinCtrl* spincontrol_video_data_messages;
		wxGauge* video_gauge;
		wxStaticText* label_;
		wxSpinCtrl* spincontrol_connections;
		wxSpinCtrl* spincontrol_data;
		wxGauge* cache_gauge;
		wxPanel* m_panel7;
		wxStaticText* label_roll;
		wxStaticText* label_roll_value;
		wxStaticText* label_pitch;
		wxStaticText* label_pitch_value;
		wxStaticText* label_yaw;
		wxStaticText* label_yaw_value;
		wxPanel* panel_orientation;
		wxStaticLine* m_staticline1;
		wxBitmapButton* button_about;
		wxBitmapButton* button_quit;
		wxAnimationCtrl* animation_associating;
		wxGauge* gauge_progress;
		
		// Virtual event handlers, overide them in your derived class
		virtual void togglebutton_connect_toggled_cb( wxCommandEvent& event ) = 0;
		virtual void button_calibrate_clicked_cb( wxCommandEvent& event ) = 0;
		virtual void button_animate_LEDs_clicked_cb( wxCommandEvent& event ) = 0;
		virtual void button_report_clicked_cb( wxCommandEvent& event ) = 0;
		virtual void togglebutton_associate_toggled_cb( wxCommandEvent& event ) = 0;
		virtual void choice_interface_selected_cb( wxCommandEvent& event ) = 0;
		virtual void spincontrol_port_changed_cb( wxSpinEvent& event ) = 0;
		virtual void togglebutton_display_toggled_cb( wxCommandEvent& event ) = 0;
		virtual void togglebutton_fullscreen_toggled_cb( wxCommandEvent& event ) = 0;
		virtual void button_switch_camera_clicked_cb( wxCommandEvent& event ) = 0;
		virtual void choice_display_video_format_selected_cb( wxCommandEvent& event ) = 0;
		virtual void choice_adapter_selected_cb( wxCommandEvent& event ) = 0;
		virtual void choice_display_selected_cb( wxCommandEvent& event ) = 0;
		virtual void button_display_settings_clicked_cb( wxCommandEvent& event ) = 0;
		virtual void choice_display_resolution_selected_cb( wxCommandEvent& event ) = 0;
		virtual void togglebutton_save_video_toggled_cb( wxCommandEvent& event ) = 0;
		virtual void button_snapshot_clicked_cb( wxCommandEvent& event ) = 0;
		virtual void choice_save_video_format_selected_cb( wxCommandEvent& event ) = 0;
		virtual void directorypicker_save_video_changed_cb( wxFileDirPickerEvent& event ) = 0;
		virtual void button_about_clicked_cb( wxCommandEvent& event ) = 0;
		virtual void button_quit_clicked_cb( wxCommandEvent& event ) = 0;
		
	
	public:
		
		dialog_main( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("ardrone"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 518,531 ), long style = wxCAPTION|wxDIALOG_NO_PARENT|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER ); 
		~dialog_main();
	
};

#endif //__ARDRONE_UI_BASE_H__
