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

#include "test_u_video_ui_session_message.h"

#include "test_u_video_ui_stream.h"

#include "ace/Log_Msg.h"

#include "stream_macros.h"

#include "stream_dec_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dec_tools.h"

#include "stream_dev_defines.h"
#include "stream_dev_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_stat_defines.h"

#include "stream_vis_defines.h"
#include "stream_vis_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directshow_tools.h"
#include "stream_lib_mediafoundation_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
Test_U_Stream::Test_U_Stream ()
 : inherited ()
 , source_ (this,
            ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING))
 , decode_ (this,
            ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MODULE_PAVE_DECODER_NAME_STRING))
 , decode_2 (this,
             ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING))
 //, report_ (this,
 //           ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING))
 , convert_ (this,
             ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_CONVERTER_DEFAULT_NAME_STRING))
 , detect_ (this,
            ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_OPENCV_CLASSIFIER_DEFAULT_NAME_STRING))
 , resize_ (this,
            ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_LIBAV_RESIZE_DEFAULT_NAME_STRING))
 , direct3DDisplay_ (this,
                     ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_DIRECT3D_DEFAULT_NAME_STRING))
 , directShowDisplay_ (this,
                       ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_DIRECTSHOW_DEFAULT_NAME_STRING))
//#if defined (GTK_USE)
// , GTKCairoDisplay_ (this,
//                     ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_GTK_CAIRO_DEFAULT_NAME_STRING))
//#endif // GTK_USE
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::Test_U_Stream"));

}

Test_U_Stream::~Test_U_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::~Test_U_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
Test_U_Stream::load (Stream_ILayout* layout_inout,
                     bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::load"));

  // initialize return value(s)
  delete_out = false;

  // sanity check(s)
  ACE_ASSERT (configuration_);
  //ACE_ASSERT (configuration_->configuration_.renderer != STREAM_VISUALIZATION_VIDEORENDERER_INVALID);
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator =
    configuration_->find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_->end ());
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator_2 =
    configuration_->find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_2 != configuration_->end ());

  layout_inout->append (&source_, NULL, 0);
  layout_inout->append (&decode_, NULL, 0);
  layout_inout->append (&decode_2, NULL, 0);
  //  layout_inout.append (&report_, NULL, 0);
  //if (configuration_->configuration_.renderer != STREAM_VISUALIZATION_VIDEORENDERER_GTK_WINDOW)
  layout_inout->append (&convert_, NULL, 0);
  layout_inout->append (&resize_, NULL, 0); // output is window size/fullscreen

  // *NOTE*: one problem is that any module that was NOT enqueued onto the
  //         stream (e.g. because initialize() failed) needs to be explicitly
  //         close()d
#if defined (GUI_SUPPORT)
  //switch (inherited::configuration_->configuration_.renderer)
  //{
  //  case STREAM_VISUALIZATION_VIDEORENDERER_NULL:
  //    break;
  //  //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_2D:
  //  //  modules_out.push_back (&direct2DDisplay_);
  //  //  break;
  //  case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTDRAW_3D:
      layout_inout->append (&direct3DDisplay_, NULL, 0);
    //  break;
    //case STREAM_VISUALIZATION_VIDEORENDERER_DIRECTSHOW:
    //  layout_inout->append (&directShowDisplay_, NULL, 0);
    //  break;
    //case STREAM_VISUALIZATION_VIDEORENDERER_MEDIAFOUNDATION:
    //  modules_out.push_back (&mediaFoundationDisplay_);
    //  break;
//#if defined (GTK_USE)
//    case STREAM_VISUALIZATION_VIDEORENDERER_GTK_CAIRO:
//      modules_out.push_back (&GTKCairoDisplay_);
//      break;
//#endif // GTK_USE
  //  default:
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("%s: invalid/unknown video renderer (was: %d), aborting\n"),
  //                ACE_TEXT (stream_name_string_),
  //                inherited::configuration_->configuration_.renderer));
  //    return false;
  //  }
  //} // end SWITCH
#endif // GUI_SUPPORT

  return true;
}

bool
Test_U_Stream::initialize (const inherited::CONFIGURATION_T& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  bool result = false;
  bool setup_pipeline = configuration_in.configuration_->setupPipeline;
  bool reset_setup_pipeline = false;
  Test_U_SessionData* session_data_p = NULL;
  inherited::CONFIGURATION_T::ITERATOR_T iterator, iterator_2;
  Test_U_AsynchTCPSource* source_impl_p = NULL;
  //struct _AMMediaType media_type_s;
  struct _AMMediaType* media_type_p = NULL, *media_type_2 = NULL;
  Test_U_SessionManager_t* session_manager_p = NULL;

  iterator =
    const_cast<inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  //iterator_2 =
  //  const_cast<inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (Stream_Visualization_Tools::rendererToModuleName (configuration_in.configuration_.renderer).c_str ()));
  // sanity check(s)
  ACE_ASSERT (iterator != const_cast<inherited::CONFIGURATION_T&> (configuration_in).end ());
  //ACE_ASSERT (iterator_2 != const_cast<inherited::CONFIGURATION_T&> (configuration_in).end ());

  // ---------------------------------------------------------------------------
  // step3: allocate a new session state, reset stream
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
  const_cast<inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  //ACE_ASSERT (inherited::sessionData_);
  //ACE_ASSERT ((*iterator).second.second.direct3DConfiguration);

  session_manager_p = Test_U_SessionManager_t::SINGLETON_T::instance ();
  ACE_ASSERT (session_manager_p);
  session_data_p =
    &const_cast<Test_U_SessionData&> (session_manager_p->getR (inherited::id_));
  // *TODO*: remove type inferences
  //if ((*iterator).second.second.direct3DConfiguration->handle)
  //{
  //  (*iterator).second.second.direct3DConfiguration->handle->AddRef ();
  //  session_data_p->direct3DDevice =
  //    (*iterator).second.second.direct3DConfiguration->handle;
  //} // end IF

  // ---------------------------------------------------------------------------
  // step4: initialize module(s)

  // ******************* Queue Source ************************
  //source_impl_p = dynamic_cast<Test_U_AsynchTCPSource*> (source_.writer ());
  //if (!source_impl_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("%s: dynamic_cast<Test_U_AsynchTCPSource> failed, aborting\n"),
  //              ACE_TEXT (stream_name_string_)));
  //  goto error;
  //} // end IF

  // ---------------------------------------------------------------------------
  // step5: update session data
  session_data_p->formats.push_back (configuration_in.configuration_->format);

  // ---------------------------------------------------------------------------
  // step6: initialize head module
  //source_impl_p->setP (&(inherited::state_));
  ////fileReader_impl_p->reset ();
  //// *NOTE*: push()ing the module will open() it
  ////         --> set the argument that is passed along (head module expects a
  ////             handle to the session data)
  //source_.arg (inherited::sessionData_);

  // step7: assemble stream
  if (configuration_in.configuration_->setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF

  // ---------------------------------------------------------------------------

  //// *TODO*: remove type inferences
  //session_data_r.fileName =
  //  configuration_in.moduleHandlerConfiguration->fileName;
  //session_data_r.size =
  //  Common_File_Tools::size (configuration_in.moduleHandlerConfiguration->fileName);

  // OK: all went well
  inherited::isInitialized_ = true;

  return true;

error:
  if ((*iterator).second.second->builder)
  {
    (*iterator).second.second->builder->Release (); (*iterator).second.second->builder = NULL;
  } // end IF
  if (session_data_p)
  {
    //if (session_data_p->direct3DDevice)
    //{
    //  session_data_p->direct3DDevice->Release (); session_data_p->direct3DDevice = NULL;
    //} // end IF
    //Stream_MediaFramework_DirectShow_Tools::free (session_data_p->formats);
    //session_data_p->resetToken = 0;
  } // end IF

  return false;
}
#else
Test_U_Stream::Test_U_Stream ()
 : inherited ()
 , source_ (this,
            ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING))
 , decode_ (this,
            ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MODULE_PAVE_DECODER_NAME_STRING))
 , decode_2 (this,
             ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING))
// , report_ (this,
//            ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING))
 , convert_ (this,
             ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_CONVERTER_DEFAULT_NAME_STRING))
 , detect_ (this,
            ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_OPENCV_CLASSIFIER_DEFAULT_NAME_STRING))
 , resize_ (this,
            ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_LIBAV_RESIZE_DEFAULT_NAME_STRING))
#if defined (GTK_SUPPORT)
 , GTKDisplay_ (this,
                ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_GTK_WINDOW_DEFAULT_NAME_STRING))
#endif // GTK_SUPPORT
 , WaylandDisplay_ (this,
                    ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_WAYLAND_WINDOW_DEFAULT_NAME_STRING))
 , X11Display_ (this,
                ACE_TEXT_ALWAYS_CHAR (STREAM_VIS_X11_WINDOW_DEFAULT_NAME_STRING))
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::Test_U_Stream"));

}

Test_U_Stream::~Test_U_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::~Test_U_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
Test_U_Stream::load (Stream_ILayout* layout_inout,
                     bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::load"));

  // initialize return value(s)
  delete_out = false;

  // sanity check(s)
  ACE_ASSERT (configuration_);
//  ACE_ASSERT (configuration_->configuration_.renderer != STREAM_VISUALIZATION_VIDEORENDERER_INVALID);

  layout_inout->append (&source_, NULL, 0);
  layout_inout->append (&decode_, NULL, 0);
  layout_inout->append (&decode_2, NULL, 0);
  //  layout_inout.append (&report_, NULL, 0);
//  if (configuration_->configuration_.renderer != STREAM_VISUALIZATION_VIDEORENDERER_GTK_WINDOW)

  // layout_inout->append (&convert_, NULL, 0);
  // layout_inout->append (&detect_, NULL, 0);

//  layout_inout.append (&resize_, NULL, 0); // output is window size/fullscreen
//  if (configuration_->configuration_.renderer != STREAM_VISUALIZATION_VIDEORENDERER_GTK_WINDOW)
//   layout_inout->append (&X11Display_, NULL, 0);
  layout_inout->append (&WaylandDisplay_, NULL, 0);
//  else
//    layout_inout->append (&display_2_, NULL, 0);

  return true;
}

bool
Test_U_Stream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  bool setup_pipeline = configuration_in.configuration_->setupPipeline;
  bool reset_setup_pipeline = false;
  Test_U_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct Test_U_ModuleHandlerConfiguration* configuration_p = NULL;
  Test_U_AsynchTCPSource* source_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  session_data_p =
    &const_cast<Test_U_SessionData&> (inherited::sessionData_->getR ());
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));

  // sanity check(s)
  ACE_ASSERT (iterator != configuration_in.end ());

  configuration_p = (*iterator).second.second;
  // sanity check(s)
  ACE_ASSERT (configuration_p);

  // *TODO*: remove type inferences
  ACE_ASSERT (session_data_p->formats.empty ());
  session_data_p->formats.push_back (configuration_in.configuration_->format);

  // ---------------------------------------------------------------------------

  // ******************* Camera Source ************************
  source_impl_p = dynamic_cast<Test_U_AsynchTCPSource*> (source_.writer ());
  ACE_ASSERT (source_impl_p);
  // source_impl_p->setP (&(inherited::state_));

  // // *NOTE*: push()ing the module will open() it
  // //         --> set the argument that is passed along (head module expects a
  // //             handle to the session data)
  // source_.arg (inherited::sessionData_);

  if (configuration_in.configuration_->setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
      setup_pipeline;

  return false;
}
#endif
