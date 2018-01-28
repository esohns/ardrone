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
#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <Mferror.h>
#endif

#include "ace/Log_Msg.h"

#include "stream_dec_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#endif
#include "stream_file_defines.h"
#include "stream_net_defines.h"
#include "stream_stat_defines.h"
#include "stream_vis_defines.h"

#include "ardrone_defines.h"
#include "ardrone_macros.h"

template <typename ModuleConfigurationType,
          typename SourceModuleType>
ARDrone_VideoStream_T<ModuleConfigurationType, 
                      SourceModuleType>::ARDrone_VideoStream_T ()
 : inherited ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
 , graphBuilder_ (NULL)
 , mediaSession_ (NULL)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::ARDrone_VideoStream_T"));

  inherited::state_.type = ARDRONE_STREAM_VIDEO;
}

template <typename ModuleConfigurationType,
          typename SourceModuleType>
ARDrone_VideoStream_T<ModuleConfigurationType, 
                      SourceModuleType>::~ARDrone_VideoStream_T ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::~ARDrone_VideoStream_T"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HRESULT result = E_FAIL;

  if (graphBuilder_)
    graphBuilder_->Release ();

  if (mediaSession_)
  {
    result = mediaSession_->Shutdown ();
    if (FAILED (result) &&
        (result != MF_E_SHUTDOWN)) // already shut down
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::errorToString (result).c_str ())));
    mediaSession_->Release ();
  } // end IF
#endif

  // *NOTE*: this implements an ordered shutdown on destruction
  inherited::shutdown ();
}

template <typename ModuleConfigurationType,
          typename SourceModuleType>
bool
ARDrone_VideoStream_T<ModuleConfigurationType, 
                      SourceModuleType>::load (Stream_ModuleList_t& modules_out,
                                               bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::load"));

  Stream_Module_t* module_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (inherited::configuration_->configuration_.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_FileWriter_Module (this,
                                                                   ACE_TEXT_ALWAYS_CHAR (MODULE_FILE_SINK_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_AVIEncoder_Module (this,
                                                                   ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_ENCODER_AVI_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_Display_Module (this,
                                                                ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_DIRECTSHOW_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_H264Decoder_Module (this,
                                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_StatisticReport_Module (this,
                                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_PaVEDecoder_Module (this,
                                                                    ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_PAVE_DECODER_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_FileWriter_Module (this,
                                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_FILE_SINK_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_AVIEncoder_Module (this,
                                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_ENCODER_AVI_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_Display_Module (this,
                                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_MEDIAFOUNDATION_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_H264Decoder_Module (this,
                                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_StatisticReport_Module (this,
                                                                             ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_PaVEDecoder_Module (this,
                                                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_PAVE_DECODER_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown media framework (was: %d), aborting\n"),
                  ACE_TEXT (video_stream_name_string_),
                  inherited::configuration_->configuration_.mediaFramework));
      break;
    }
  } // end SWITCH
#else
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_FileWriter_Module (this,
                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_FILE_SINK_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_AVIEncoder_Module (this,
                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_ENCODER_AVI_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_Display_Module (this,
                                                 ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_GTK_PIXBUF_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_H264Decoder_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_PaVEDecoder_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_PAVE_DECODER_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
#endif
  ACE_NEW_RETURN (module_p,
                  SourceModuleType (this,
                                    ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

template <typename ModuleConfigurationType,
          typename SourceModuleType>
bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_VideoStream_T<ModuleConfigurationType,
                      SourceModuleType>::initialize (const CONFIGURATION_T& configuration_in)
#else
ARDrone_VideoStream_T<ModuleConfigurationType,
                      SourceModuleType>::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::initialize"));

  if (inherited::isInitialized_)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (graphBuilder_)
    {
      graphBuilder_->Release ();
      graphBuilder_ = NULL;
    } // end IF

    if (mediaSession_)
    {
      mediaSession_->Release ();
      mediaSession_ = NULL;
    } // end IF
#endif
  } // end IF

  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_SessionData* session_data_p = NULL;
#endif
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ModuleConfigurationType* configuration_p = NULL;
#endif
  typename inherited::ISTREAM_T::MODULE_T* module_p = NULL;
  typename SourceModuleType::WRITER_T* sourceWriter_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (video_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  configuration_p =
    dynamic_cast<ModuleConfigurationType*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (inherited::sessionData_);

  session_data_p =
    &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  switch (configuration_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      // sanity check(s)
      ACE_ASSERT (configuration_p->inputFormat);
      //ACE_ASSERT (configuration_p->filterConfiguration);
      //ACE_ASSERT (configuration_p->filterConfiguration->pinConfiguration);
      //ACE_ASSERT (configuration_p->filterConfiguration->pinConfiguration->format);

      if (session_data_p->inputFormat)
        Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_p->inputFormat);

      ACE_ASSERT (!session_data_p->inputFormat);
      //if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*(configuration_p->filterConfiguration->pinConfiguration->format),
      if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*(configuration_p->inputFormat),
                                                                 session_data_p->inputFormat))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), aborting\n")));
        goto error;
      } // end IF
      ACE_ASSERT (session_data_p->inputFormat);

      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_ASSERT (false); // *TODO*
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  configuration_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#endif

  // ---------------------------------------------------------------------------

  // ******************************** Source ***********************************
  module_p =
    const_cast<typename inherited::ISTREAM_T::MODULE_T*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                                           true,
                                                                           false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (video_stream_name_string_),
                ACE_TEXT (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

  sourceWriter_impl_p =
    dynamic_cast<typename SourceModuleType::WRITER_T*> (module_p->writer ());
  if (!sourceWriter_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<SourceModuleType::WRITER_T> failed, aborting\n"),
                ACE_TEXT (video_stream_name_string_)));
    goto error;
  } // end IF
  sourceWriter_impl_p->setP (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (video_stream_name_string_)));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //if (media_type_p)
  //  media_type_p->Release ();
  //if (topology_p)
  //  topology_p->Release ();
  //if (session_data_p->direct3DDevice)
  //{
  //  session_data_p->direct3DDevice->Release ();
  //  session_data_p->direct3DDevice = NULL;
  //} // end IF
  if (session_data_p->inputFormat)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_p->inputFormat);
  //session_data_p->resetToken = 0;
  //if (session_data_p->session)
  //{
  //  session_data_p->session->Release ();
  //  session_data_p->session = NULL;
  //} // end IF
  //if (mediaSession_)
  //{
  //  mediaSession_->Release ();
  //  mediaSession_ = NULL;
  //} // end IF

  //if (COM_initialized)
  //  CoUninitialize ();
#endif
  return false;
}

//////////////////////////////////////////

template <typename ModuleConfigurationType>
ARDrone_ControlStream_T<ModuleConfigurationType>::ARDrone_ControlStream_T ()
 : inherited ()
 , inherited2 (&(inherited::sessionDataLock_))
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream_T::ARDrone_ControlStream_T"));

  inherited::state_.type = ARDRONE_STREAM_CONTROL;
}

template <typename ModuleConfigurationType>
bool
ARDrone_ControlStream_T<ModuleConfigurationType>::load (Stream_ModuleList_t& modules_out,
                                                        bool& delete_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream_T::load"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  Stream_Module_t* module_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (inherited::configuration_->configuration_.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_ControlDecoder_Module (this,
                                                                       ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_StatisticReport_Module (this,
                                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_TCPSource_Module (this,
                                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_AsynchTCPSource_Module (this,
                                                                          ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_ControlDecoder_Module (this,
                                                                            ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_StatisticReport_Module (this,
                                                                             ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_TCPSource_Module (this,
                                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_AsynchTCPSource_Module (this,
                                                                               ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown media framework (was: %d), aborting\n"),
                  ACE_TEXT (video_stream_name_string_),
                  inherited::configuration_->configuration_.mediaFramework));
      break;
    }
  } // end SWITCH
#else
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (this,
//                                                    ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#if defined (_DEBUG)
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_Dump_Module (this,
//                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_ControlDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_TCPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchTCPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
#endif
  delete_out = true;

  return true;
}

template <typename ModuleConfigurationType>
bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_ControlStream_T<ModuleConfigurationType>::initialize (const CONFIGURATION_T& configuration_in)
#else
ARDrone_ControlStream_T<ModuleConfigurationType>::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream_T::initialize"));

  if (inherited::isInitialized_)
  {
    configuration_ = NULL;
  } // end IF

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
  //  struct ARDrone_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  ModuleConfigurationType* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;
  Common_ISetP_T<struct ARDrone_StreamState>* iset_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  ACE_ASSERT (configuration_in.configuration_.CBData);

  configuration_ = configuration_in.configuration_.CBData->controller;

  // sanity check(s)
  ACE_ASSERT (configuration_);

  iterator =
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));

  // sanity check(s)
  ACE_ASSERT (iterator != configuration_in.end ());

  configuration_p =
    dynamic_cast<ModuleConfigurationType*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);

  configuration_p->subscribers->push_back (this);
  configuration_p->subscribers->sort ();
  configuration_p->subscribers->unique (SUBSCRIBERS_IS_EQUAL_P ());

  // ---------------------------------------------------------------------------

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (control_stream_name_string_),
                ACE_TEXT (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

  iset_p =
    dynamic_cast<Common_ISetP_T<struct ARDrone_StreamState>*> (module_p->writer ());
  ACE_ASSERT (iset_p);
  iset_p->setP (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  ACE_ASSERT (configuration_in.configuration_.initializeControl);
  if (!configuration_in.configuration_.initializeControl->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (control_stream_name_string_)));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

template <typename ModuleConfigurationType>
void
ARDrone_ControlStream_T<ModuleConfigurationType>::start (Stream_SessionId_t sessionId_in,
                                                         const struct ARDrone_SessionData& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_ControlStream_T::start"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionData_in);
}

template <typename ModuleConfigurationType>
void
ARDrone_ControlStream_T<ModuleConfigurationType>::notify (Stream_SessionId_t sessionId_in,
                                                          const enum Stream_SessionMessageType& event_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_ControlStream_T::notify"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  ACE_UNUSED_ARG (event_in);
}

template <typename ModuleConfigurationType>
void
ARDrone_ControlStream_T<ModuleConfigurationType>::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_ControlStream_T::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;
}

template <typename ModuleConfigurationType>
void
ARDrone_ControlStream_T<ModuleConfigurationType>::messageCB (const ARDrone_DeviceConfiguration_t& deviceConfiguration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream_T::messageCB"));

  // sanity check(s)
  ACE_ASSERT (configuration_);

  try {
    configuration_->setP (&const_cast<ARDrone_DeviceConfiguration_t&> (deviceConfiguration_in));
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in ARDrone_IDeviceConfiguration::setP(), continuing\n"),
                ACE_TEXT (control_stream_name_string_)));
  }

//#if defined (_DEBUG)
//  // debug info
//  unsigned int number_of_settings = 0;
//  for (ARDrone_DeviceConfigurationConstIterator_t iterator = deviceConfiguration_in.begin ();
//       iterator != deviceConfiguration_in.end ();
//       ++iterator)
//    number_of_settings += (*iterator).second.size ();
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("received device configuration (%d setting(s) in %d categories):\n"),
//              number_of_settings, deviceConfiguration_in.size ()));
//  for (ARDrone_DeviceConfigurationConstIterator_t iterator = deviceConfiguration_in.begin ();
//       iterator != deviceConfiguration_in.end ();
//       ++iterator)
//  {
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("--- \"%s\" (%d setting(s) ---):\n"),
//                ACE_TEXT ((*iterator).first.c_str ()), (*iterator).second.size ()));
//    for (ARDrone_DeviceConfigurationCategoryIterator_t iterator_2 = (*iterator).second.begin ();
//         iterator_2 != (*iterator).second.end ();
//         ++iterator_2)
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("\t%s:\t%s\n"),
//                  ACE_TEXT ((*iterator_2).first.c_str ()),
//                  ACE_TEXT ((*iterator_2).second.c_str ())));
//  } // end FOR
//#endif
}

//////////////////////////////////////////

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::ARDrone_NavDataStream_T ()
 : inherited ()
 , inherited2 (&(inherited::sessionDataLock_))
 , isFirst_ (true)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::ARDrone_NavDataStream_T"));

  inherited::state_.type = ARDRONE_STREAM_NAVDATA;

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
  WLAN_monitor_p->subscribe (this);
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::~ARDrone_NavDataStream_T ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::~ARDrone_NavDataStream_T"));

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
  WLAN_monitor_p->unsubscribe (this);

  inherited::shutdown ();
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
bool
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::load (Stream_ModuleList_t& modules_out,
                                                                    bool& delete_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::load"));

  Stream_Module_t* module_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (inherited::configuration_->configuration_.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_Controller_Module (this,
                                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_AsynchController_Module (this,
                                                                           ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#if defined (_DEBUG)
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_Dump_Module (this,
      //                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#endif
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_StatisticReport_Module (this,
                                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_NavDataDecoder_Module (this,
                                                                       ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_UDPSource_Module (this,
                                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_AsynchUDPSource_Module (this,
                                                                          ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_Controller_Module (this,
                                                                          ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_AsynchController_Module (this,
                                                                                ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#if defined (_DEBUG)
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_Dump_Module (this,
      //                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#endif
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_StatisticReport_Module (this,
                                                                             ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_NavDataDecoder_Module (this,
                                                                            ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_UDPSource_Module (this,
                                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_AsynchUDPSource_Module (this,
                                                                               ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown media framework (was: %d), aborting\n"),
                  ACE_TEXT (video_stream_name_string_),
                  inherited::configuration_->configuration_.mediaFramework));
      break;
    }
  } // end SWITCH
#else
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_Controller_Module (this,
                                                      ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchController_Module (this,
                                                            ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                    false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#if defined (_DEBUG)
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_Dump_Module (this,
//                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_NavDataDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_UDPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchUDPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
#endif
  delete_out = true;

  return true;
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::initialize (const CONFIGURATION_T& configuration_in)
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::initialize"));

  if (inherited::isInitialized_)
  {
    isFirst_ = true;
  } // end IF

  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
//  struct ARDrone_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  ModuleConfigurationType* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;
  Common_ISetP_T<struct ARDrone_StreamState>* iset_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (navdata_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  //// sanity check(s)
  //ACE_ASSERT (inherited::sessionData_);
  { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, inherited::lock_, false);
    inherited::state_.CBData = configuration_in.configuration_.CBData;
  } // end lock scope

//  session_data_p =
//    &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));

  // sanity check(s)
  ACE_ASSERT (iterator != configuration_in.end ());

  configuration_p =
      dynamic_cast<ModuleConfigurationType*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);

  configuration_p->subscribers->push_back (this);
  configuration_p->subscribers->sort ();
  configuration_p->subscribers->unique (SUBSCRIBERS_IS_EQUAL_P ());

  // ---------------------------------------------------------------------------

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_),
                ACE_TEXT (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

  iset_p =
    dynamic_cast<Common_ISetP_T<struct ARDrone_StreamState>*> (module_p->writer ());
  ACE_ASSERT (iset_p);
  iset_p->setP (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  ACE_ASSERT (configuration_in.configuration_.initializeNavData);
  if (!configuration_in.configuration_.initializeNavData->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_)));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (navdata_stream_name_string_)));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::start (Stream_SessionId_t sessionId_in,
                                                                     const struct ARDrone_SessionData& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::start"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (sessionData_in.state);
  if (sessionData_in.state->type != ARDRONE_STREAM_NAVDATA)
    return;

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = true;
  } // end lock scope

  try {
    inherited2::startCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::startCB(), continuing\n"),
                ACE_TEXT (navdata_stream_name_string_)));
  }
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::notify (Stream_SessionId_t sessionId_in,
                                                                      const enum Stream_SessionMessageType& event_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::notify"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  switch (event_in)
  {
    case STREAM_SESSION_MESSAGE_LINK:
    {
      // *NOTE*: there will be two 'link' messages; wait for the second one
      if (isFirst_)
      {
        isFirst_ = false;
        break;
      } // end IF

      // reset event dispatch notification for outbound data
      struct Net_UDPSocketConfiguration* socket_configuration_p = NULL;
      typename inherited::CONFIGURATION_T::ITERATOR_T stream_configuration_iterator;
      ConnectionConfigurationIteratorType connection_iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ARDrone_DirectShow_IConnectionManager_t* directshow_iconnection_manager_p =
        NULL;
      typename ARDrone_DirectShow_IConnectionManager_t::CONNECTION_T* directshow_connection_p =
        NULL;
      ARDrone_MediaFoundation_IConnectionManager_t* mediafoundation_iconnection_manager_p =
        NULL;
      typename ARDrone_MediaFoundation_IConnectionManager_t::CONNECTION_T* mediafoundation_connection_p =
        NULL;
      switch (inherited::configuration_->configuration_.mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          directshow_iconnection_manager_p =
            ARDRONE_DIRECTSHOW_CONNECTIONMANAGER_SINGLETON::instance ();
          ACE_ASSERT (directshow_iconnection_manager_p);
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          mediafoundation_iconnection_manager_p =
            ARDRONE_MEDIAFOUNDATION_CONNECTIONMANAGER_SINGLETON::instance ();
          ACE_ASSERT (mediafoundation_iconnection_manager_p);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      inherited::configuration_->configuration_.mediaFramework));
          return;
        }
      } // end SWITCH
#else
      ARDrone_IConnectionManager_t* iconnection_manager_p =
          ARDRONE_CONNECTIONMANAGER_SINGLETON::instance ();
      ACE_ASSERT (iconnection_manager_p);
      typename ARDrone_IConnectionManager_t::CONNECTION_T* connection_p = NULL;
#endif
      Stream_IOutboundDataNotify* ioutbound_data_notify = NULL;
      std::string module_name_string;

      // sanity check(s)
      ACE_ASSERT (inherited::configuration_);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (inherited::configuration_->configuration_.mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          stream_configuration_iterator =
            inherited::configuration_->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
          ACE_ASSERT (stream_configuration_iterator != inherited::configuration_->end ());
          ACE_ASSERT ((*stream_configuration_iterator).second.second.connectionConfigurations);
          connection_iterator =
            (*stream_configuration_iterator).second.second.connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
          ACE_ASSERT (connection_iterator != (*stream_configuration_iterator).second.second.connectionConfigurations->end ());
          ACE_ASSERT ((*connection_iterator).second.socketHandlerConfiguration.socketConfiguration);
          socket_configuration_p =
            dynamic_cast<struct Net_UDPSocketConfiguration*> ((*connection_iterator).second.socketHandlerConfiguration.socketConfiguration);
          ACE_ASSERT (socket_configuration_p);
          directshow_connection_p =
            directshow_iconnection_manager_p->get (socket_configuration_p->peerAddress,
                                                   true); // is peer address ?

          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          stream_configuration_iterator =
            inherited::configuration_->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
          ACE_ASSERT (stream_configuration_iterator != inherited::configuration_->end ());
          ACE_ASSERT ((*stream_configuration_iterator).second.second.connectionConfigurations);
          connection_iterator =
            (*stream_configuration_iterator).second.second.connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
          ACE_ASSERT (connection_iterator != (*stream_configuration_iterator).second.second.connectionConfigurations->end ());
          ACE_ASSERT ((*connection_iterator).second.socketHandlerConfiguration.socketConfiguration);
          socket_configuration_p =
            dynamic_cast<struct Net_UDPSocketConfiguration*> ((*connection_iterator).second.socketHandlerConfiguration.socketConfiguration);
          ACE_ASSERT (socket_configuration_p);
          mediafoundation_connection_p =
            mediafoundation_iconnection_manager_p->get (socket_configuration_p->peerAddress,
                                                        true); // is peer address ?

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      inherited::configuration_->configuration_.mediaFramework));
          return;
        }
      } // end SWITCH
#else
      stream_configuration_iterator =
          inherited::configuration_->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
      ACE_ASSERT (stream_configuration_iterator != inherited::configuration_->end ());
      ACE_ASSERT ((*stream_configuration_iterator).second.second.connectionConfigurations);
      connection_iterator =
          (*stream_configuration_iterator).second.second.connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
      ACE_ASSERT (connection_iterator != (*stream_configuration_iterator).second.second.connectionConfigurations->end ());
      ACE_ASSERT ((*connection_iterator).second.socketHandlerConfiguration.socketConfiguration);
      socket_configuration_p =
          dynamic_cast<struct Net_UDPSocketConfiguration*> ((*connection_iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      connection_p =
          iconnection_manager_p->get (socket_configuration_p->peerAddress,
                                      true); // is peer address ?
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (inherited::configuration_->configuration_.mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        { 
          if (!directshow_connection_p)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to Net_IConnectionManager::get(0x%@), returning"),
                        ACE_TEXT (navdata_stream_name_string_),
                        ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
            return;
          } // end IF

          ARDrone_DirectShow_IStreamConnection_t* istream_connection_p =
            dynamic_cast<ARDrone_DirectShow_IStreamConnection_t*> (directshow_connection_p);
          ACE_ASSERT (istream_connection_p);
          ioutbound_data_notify =
            &const_cast<typename ARDrone_DirectShow_IStreamConnection_t::STREAM_T&> (istream_connection_p->stream ());
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        { 
          if (!mediafoundation_connection_p)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: failed to Net_IConnectionManager::get(0x%@), returning"),
                        ACE_TEXT (navdata_stream_name_string_),
                        ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
            return;
          } // end IF

          ARDrone_MediaFoundation_IStreamConnection_t* istream_connection_p =
            dynamic_cast<ARDrone_MediaFoundation_IStreamConnection_t*> (mediafoundation_connection_p);
          ACE_ASSERT (istream_connection_p);
          ioutbound_data_notify =
            &const_cast<typename ARDrone_MediaFoundation_IStreamConnection_t::STREAM_T&> (istream_connection_p->stream ());
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      inherited::configuration_->configuration_.mediaFramework));
          return;
        }
      } // end SWITCH
#else
      if (!connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Net_IConnectionManager::get(0x%@), returning"),
                    ACE_TEXT (navdata_stream_name_string_),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
        return;
      } // end IF

      ARDrone_IStreamConnection_t* istream_connection_p =
        dynamic_cast<ARDrone_IStreamConnection_t*> (connection_p);
      ACE_ASSERT (istream_connection_p);
      ioutbound_data_notify =
        &const_cast<typename ARDrone_IStreamConnection_t::STREAM_T&> (istream_connection_p->stream ());
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (inherited::configuration_->configuration_.mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          try {
            if (!ioutbound_data_notify->initialize (directshow_connection_p->notification (),
                                                    module_name_string))
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("%s: failed to Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                          ACE_TEXT (navdata_stream_name_string_),
                          directshow_connection_p->notification (),
                          ACE_TEXT (module_name_string.c_str ())));
            return;
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                        ACE_TEXT (navdata_stream_name_string_),
                        directshow_connection_p->notification (),
                        ACE_TEXT (module_name_string.c_str ())));
            return;
          }
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          try {
            if (!ioutbound_data_notify->initialize (mediafoundation_connection_p->notification (),
                                                    module_name_string))
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("%s: failed to Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                          ACE_TEXT (navdata_stream_name_string_),
                          mediafoundation_connection_p->notification (),
                          ACE_TEXT (module_name_string.c_str ())));
            return;
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: caught exception in Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                        ACE_TEXT (navdata_stream_name_string_),
                        mediafoundation_connection_p->notification (),
                        ACE_TEXT (module_name_string.c_str ())));
            return;
          }
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      inherited::configuration_->configuration_.mediaFramework));
          return;
        }
      } // end SWITCH
#else
      try {
        if (!ioutbound_data_notify->initialize (connection_p->notification (),
                                                module_name_string))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                      ACE_TEXT (navdata_stream_name_string_),
                      connection_p->notification (),
                      ACE_TEXT (module_name_string.c_str ())));
        return;
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                    ACE_TEXT (navdata_stream_name_string_),
                    connection_p->notification (),
                    ACE_TEXT (module_name_string.c_str ())));
        return;
      }
#endif
      break;
    }
    default:
      break;
  } // end SWITCH
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  try {
    inherited2::endCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::endCB(), continuing\n"),
                ACE_TEXT (navdata_stream_name_string_)));
  }

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = false;
  } // end lock scope
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::messageCB (const struct _navdata_t& record_in,
                                                                         const ARDrone_NavDataOptionOffsets_t& offsets_in,
                                                                         void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::messageCB"));

//#if defined (_DEBUG)
//  // dump state
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("state:\n\tflying: %s\n\tvideo: %s\n\tvision: %s\n\tcontrol algorithm: %s\n\taltitude control active: %s\n\tstart button state: %s\n\tcontrol command: %s\n\tcamera ready: %s\n\ttravelling: %s\n\tUSB key ready: %s\n\tNavData demo only: %s\n\tbootstrap mode: %s\n\tmotor status: %s\n\tCOM lost: %s\n\tsoftware fault: %s\n\tbattery low: %s\n\temergency landing (user): %s\n\ttimer elapsed: %s\n\tmagnetometer needs calibration: %s\n\tangles out of range: %s\n\twind mask: %s\n\tultrasound mask: %s\n\tcutout system: %s\n\tPIC version number: %s\n\tATcodec thread: %s\n\tNavData thread: %s\n\tvideo thread: %s\n\tacquisition thread: %s\n\tcontrol watchdog: %s\n\tADC watchdog: %s\n\tCOM watchdog: %s\n\temergency landing: %s\n"),
//              ((record_in.ardrone_state & ARDRONE_FLY_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_VIDEO_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_VISION_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_CONTROL_MASK) ? ACE_TEXT ("euler angles") : ACE_TEXT ("angular speed")),
//              ((record_in.ardrone_state & ARDRONE_ALTITUDE_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_USER_FEEDBACK_START) ? ACE_TEXT ("on") : ACE_TEXT ("off")),
//              ((record_in.ardrone_state & ARDRONE_COMMAND_MASK) ? ACE_TEXT ("ACK") : ACE_TEXT ("not set")),
//              ((record_in.ardrone_state & ARDRONE_CAMERA_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_TRAVELLING_MASK) ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled")),
//              ((record_in.ardrone_state & ARDRONE_USB_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_NAVDATA_DEMO_MASK) ? ACE_TEXT ("demo only") : ACE_TEXT ("all")),
//              ((record_in.ardrone_state & ARDRONE_NAVDATA_BOOTSTRAP) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_MOTORS_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
//              ((record_in.ardrone_state & ARDRONE_COM_LOST_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_SOFTWARE_FAULT) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_VBAT_LOW) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_USER_EL) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_TIMER_ELAPSED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_MAGNETO_NEEDS_CALIB) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_ANGLES_OUT_OF_RANGE) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_WIND_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
//              ((record_in.ardrone_state & ARDRONE_ULTRASOUND_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
//              ((record_in.ardrone_state & ARDRONE_CUTOUT_MASK) ? ACE_TEXT ("detected") : ACE_TEXT ("not detected")),
//              ((record_in.ardrone_state & ARDRONE_PIC_VERSION_MASK) ? ACE_TEXT ("OK") : ACE_TEXT ("error")),
//              ((record_in.ardrone_state & ARDRONE_ATCODEC_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_NAVDATA_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_VIDEO_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_ACQ_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
//              ((record_in.ardrone_state & ARDRONE_CTRL_WATCHDOG_MASK) ? ACE_TEXT ("delayed >5ms") : ACE_TEXT ("OK")),
//              ((record_in.ardrone_state & ARDRONE_ADC_WATCHDOG_MASK) ? ACE_TEXT ("delayed >5ms") : ACE_TEXT ("OK")),
//              ((record_in.ardrone_state & ARDRONE_COM_WATCHDOG_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
//              ((record_in.ardrone_state & ARDRONE_EMERGENCY_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));

//  // *TODO*: dump options
//#endif

  struct _navdata_option_t* option_p = NULL;
  for (ARDrone_NavDataOptionOffsetsIterator_t iterator = offsets_in.begin ();
       iterator != offsets_in.end ();
       ++iterator)
  {
    option_p =
      reinterpret_cast<struct _navdata_option_t*> (static_cast<char*> (payload_in) + *iterator);
    switch (option_p->tag)
    {
      case NAVDATA_DEMO_TAG: // 0
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_demo_t));
        struct _navdata_demo_t* option_2 =
          reinterpret_cast<struct _navdata_demo_t*> (option_p);
        ACE_ASSERT (option_2);

        ACE_ASSERT (inherited::state_.CBData);
        { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::state_.CBData->lock);
          inherited::state_.CBData->openGLScene.orientation.x =
              option_2->phi; // roll (--> rotation along x)
          inherited::state_.CBData->openGLScene.orientation.y =
              option_2->psi; // yaw (--> rotation along y)
          inherited::state_.CBData->openGLScene.orientation.z =
              option_2->theta; // pitch (--> rotation along z)
        } // end lock scope

        break;
      }
      case NAVDATA_TIME_TAG: // 1
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_time_t));
        struct _navdata_time_t* option_2 =
          reinterpret_cast<struct _navdata_time_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_RAW_MEASURES_TAG: // 2
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_raw_measures_t));
        struct _navdata_raw_measures_t* option_2 =
          reinterpret_cast<struct _navdata_raw_measures_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_PHYS_MEASURES_TAG: // 3
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_phys_measures_t));
        struct _navdata_phys_measures_t* option_2 =
          reinterpret_cast<struct _navdata_phys_measures_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_GYROS_OFFSETS_TAG: // 4
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_gyros_offsets_t));
        struct _navdata_gyros_offsets_t* option_2 =
          reinterpret_cast<struct _navdata_gyros_offsets_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_EULER_ANGLES_TAG: // 5
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_euler_angles_t));
        struct _navdata_euler_angles_t* option_2 =
          reinterpret_cast<struct _navdata_euler_angles_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_REFERENCES_TAG: // 6
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_references_t));
        struct _navdata_references_t* option_2 =
          reinterpret_cast<struct _navdata_references_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_TRIMS_TAG: // 7
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_trims_t));
        struct _navdata_trims_t* option_2 =
          reinterpret_cast<struct _navdata_trims_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_RC_REFERENCES_TAG: // 8
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_rc_references_t));
        struct _navdata_rc_references_t* option_2 =
          reinterpret_cast<struct _navdata_rc_references_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_PWM_TAG: // 9
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_pwm_t));
        struct _navdata_pwm_t* option_2 =
          reinterpret_cast<struct _navdata_pwm_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_ALTITUDE_TAG: // 10
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_altitude_t));
        struct _navdata_altitude_t* option_2 =
          reinterpret_cast<struct _navdata_altitude_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_RAW_TAG: // 11
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_raw_t));
        struct _navdata_vision_raw_t* option_2 =
          reinterpret_cast<struct _navdata_vision_raw_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_OF_TAG: // 12
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_of_t));
        struct _navdata_vision_of_t* option_2 =
          reinterpret_cast<struct _navdata_vision_of_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_TAG: // 13
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_t));
        struct _navdata_vision_t* option_2 =
          reinterpret_cast<struct _navdata_vision_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_PERF_TAG: // 14
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_perf_t));
        struct _navdata_vision_perf_t* option_2 =
          reinterpret_cast<struct _navdata_vision_perf_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_TRACKERS_SEND_TAG: // 15
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_trackers_send_t));
        struct _navdata_trackers_send_t* option_2 =
          reinterpret_cast<struct _navdata_trackers_send_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_DETECT_TAG: // 16
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_detect_t));
        struct _navdata_vision_detect_t* option_2 =
          reinterpret_cast<struct _navdata_vision_detect_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_WATCHDOG_TAG: // 17
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_watchdog_t));
        struct _navdata_watchdog_t* option_2 =
          reinterpret_cast<struct _navdata_watchdog_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_ADC_DATA_FRAME_TAG: // 18
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_adc_data_frame_t));
        struct _navdata_adc_data_frame_t* option_2 =
          reinterpret_cast<struct _navdata_adc_data_frame_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VIDEO_STREAM_TAG: // 19
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_video_stream_t));
        struct _navdata_video_stream_t* option_2 =
          reinterpret_cast<struct _navdata_video_stream_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_GAMES_TAG: // 20
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_games_t));
        struct _navdata_games_t* option_2 =
          reinterpret_cast<struct _navdata_games_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_PRESSURE_RAW_TAG: // 21
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_pressure_raw_t));
        struct _navdata_pressure_raw_t* option_2 =
          reinterpret_cast<struct _navdata_pressure_raw_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_MAGNETO_TAG: // 22
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_magneto_t));
        struct _navdata_magneto_t* option_2 =
          reinterpret_cast<struct _navdata_magneto_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_WIND_TAG: // 23
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_wind_speed_t));
        struct _navdata_wind_speed_t* option_2 =
          reinterpret_cast<struct _navdata_wind_speed_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_KALMAN_PRESSURE_TAG: // 24
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_kalman_pressure_t));
        struct _navdata_kalman_pressure_t* option_2 =
          reinterpret_cast<struct _navdata_kalman_pressure_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_HDVIDEO_STREAM_TAG: // 25
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_hdvideo_stream_t));
        struct _navdata_hdvideo_stream_t* option_2 =
          reinterpret_cast<struct _navdata_hdvideo_stream_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_WIFI_TAG: // 26
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_wifi_t));
        struct _navdata_wifi_t* option_2 =
          reinterpret_cast<struct _navdata_wifi_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_ZIMMU_3000_TAG: // 27
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_zimmu_3000_t));
        struct _navdata_zimmu_3000_t* option_2 =
          reinterpret_cast<struct _navdata_zimmu_3000_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_CKS_TAG: // 65535
      { ACE_ASSERT (option_p->size == sizeof (struct _navdata_cks_t));
        struct _navdata_cks_t* option_2 =
          reinterpret_cast<struct _navdata_cks_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: invalid/unknown NavData option (was: %d), continuing\n"),
                    ACE_TEXT (navdata_stream_name_string_),
                    option_p->tag));
        break;
      }
    } // end SWITCH
  } // end FOR
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
const ARDrone_IController* const
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::getP () const
{
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_),
                ACE_TEXT (MODULE_NET_TARGET_DEFAULT_NAME_STRING)));
    return NULL;
  } // end IF

  ARDrone_IController* controller_p =
    dynamic_cast<ARDrone_IController*> (module_p->writer ());
  if (!controller_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: dynamic_cast<ARDrone_IController> failed, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_),
                module_p->name ()));
    return NULL;
  } // end IF

  return controller_p;
}

//////////////////////////////////////////

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onSignalQualityChange (REFGUID interfaceIdentifier_in,
                                                                                     WLAN_SIGNAL_QUALITY signalQuality_in)
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onSignalQualityChange (const std::string& interfaceIdentifier_in,
                                                                                     unsigned int signalQuality_in)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onSignalQualityChange"));

  //  // sanity check(s)
  //  if (!isInitialized_ || !isActive_)
  //    return;
  //  ACE_ASSERT (configuration_);
  //  if (!configuration_->autoAssociate)
  //    return;
  //
  //  if (!associate (interfaceIdentifier_in,
  //                  configuration_->SSID))
  //#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
  //                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
  //                ACE_TEXT (configuration_->SSID.c_str ())));
  //#else
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
  //                ACE_TEXT (interfaceIdentifier_in.c_str ()),
  //                ACE_TEXT (configuration_->SSID.c_str ())));
  //#endif
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onAssociate (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onAssociate (const std::string& interfaceIdentifier_in,
#endif
                                                                           const std::string& SSID_in,
                                                                           bool success_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onAssociate"));

  ACE_UNUSED_ARG (interfaceIdentifier_in);
  ACE_UNUSED_ARG (SSID_in);

  // sanity check(s)
  if (!inherited::configuration_)
    return;
  if (!success_in)
    return;

  // update GUI ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *TODO*: move this to onConnect() (see below)
  if (inherited::configuration_->configuration_.CBData)
  {
    guint event_source_id =
      g_idle_add (idle_associated_SSID_cb,
                  inherited::configuration_->configuration_.CBData);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to g_idle_add(idle_associated_SSID_cb): \"%m\", returning\n"),
                  ACE_TEXT (navdata_stream_name_string_)));
      return;
    } // end IF
  } // end IF
#endif
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onDisassociate (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onDisassociate (const std::string& interfaceIdentifier_in,
#endif
                                                                              const std::string& SSID_in,
                                                                              bool success_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onDisassociate"));

  ACE_UNUSED_ARG (interfaceIdentifier_in);
  ACE_UNUSED_ARG (SSID_in);

  // sanity check(s)
  if (!inherited::configuration_)
    return;
  if (!success_in)
    return;

  // update GUI ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *TODO*: move this to onConnect() (see below)
  if (inherited::configuration_->configuration_.CBData)
  {
    guint event_source_id =
      g_idle_add (idle_associated_SSID_cb,
                  inherited::configuration_->configuration_.CBData);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to g_idle_add(idle_associated_SSID_cb): \"%m\", returning\n"),
                  ACE_TEXT (navdata_stream_name_string_)));
      return;
    } // end IF
  } // end IF
#endif
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onConnect (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onConnect (const std::string& interfaceIdentifier_in,
#endif
                                                                         const std::string& SSID_in,
                                                                         bool success_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onConnect"));

  ACE_UNUSED_ARG (interfaceIdentifier_in);
  ACE_UNUSED_ARG (SSID_in);

  // sanity check(s)
  if (!inherited::configuration_)
    return;
  if (!success_in)
    return;

  // debug info
  ACE_INET_Addr local_SAP, peer_SAP;
  ARDrone_WLANMonitor_t* wlan_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (wlan_monitor_p);
  if (!Net_Common_Tools::interfaceToIPAddress (interfaceIdentifier_in,
                                               local_SAP,
                                               peer_SAP))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ())));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
                ACE_TEXT (interfaceIdentifier_in.c_str ())));
#endif
    return;
  } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": connected to SSID %s: %s <---> %s\n"),
              ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
              ACE_TEXT (SSID_in.c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (local_SAP).c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (peer_SAP).c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": connected to SSID %s: %s <---> %s\n"),
              ACE_TEXT (interfaceIdentifier_in.c_str ()),
              ACE_TEXT (SSID_in.c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (local_SAP).c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (peer_SAP).c_str ())));
#endif

  // update GUI ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (inherited::configuration_->configuration_.CBData)
  {
    guint event_source_id =
      g_idle_add (idle_associated_SSID_cb,
                  inherited::configuration_->configuration_.CBData);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(idle_associated_SSID_cb): \"%m\", returning\n")));
      return;
    } // end IF
  } // end IF
#endif
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onDisconnect (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onDisconnect (const std::string& interfaceIdentifier_in,
#endif
                                                                            const std::string& SSID_in,
                                                                            bool success_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onDisconnect"));

  ACE_UNUSED_ARG (interfaceIdentifier_in);
  ACE_UNUSED_ARG (SSID_in);

  // sanity check(s)
  if (!inherited::configuration_)
    return;
  if (!success_in)
    return;

//  // debug info
//  ACE_INET_Addr local_SAP, peer_SAP;
//  ARDrone_WLANMonitor_t* wlan_monitor_p =
//    ARDRONE_WLANMONITOR_SINGLETON::instance ();
//  ACE_ASSERT (wlan_monitor_p);
//  if (!Net_Common_Tools::interfaceToIPAddress (interfaceIdentifier_in,
//                                               local_SAP,
//                                               peer_SAP))
//  {
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
//                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ())));
//#else
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
//                ACE_TEXT (interfaceIdentifier_in.c_str ())));
//#endif
//    return;
//  } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": disconnected from SSID %s\n"),
              ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
              ACE_TEXT (SSID_in.c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": disconnected from SSID %s\n"),
              ACE_TEXT (interfaceIdentifier_in.c_str ()),
              ACE_TEXT (SSID_in.c_str ())));
#endif

  // update GUI ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (inherited::configuration_->configuration_.CBData)
  {
    guint event_source_id =
      g_idle_add (idle_associated_SSID_cb,
                  inherited::configuration_->configuration_.CBData);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(idle_associated_SSID_cb): \"%m\", returning\n")));
      return;
    } // end IF
  } // end IF
#endif
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onHotPlug (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onHotPlug (const std::string& interfaceIdentifier_in,
#endif
                                                                         bool enabled_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onHotPlug"));

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("\"%s\": interface %s\n"),
//              ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
//              (enabled_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled/removed"))));
//#else
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("\"%s\": interface %s\n"),
//              ACE_TEXT (interfaceIdentifier_in.c_str ()),
//              (enabled_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled/removed"))));
//#endif
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onRemove (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onRemove (const std::string& interfaceIdentifier_in,
#endif
                                                                        bool enabled_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onRemove"));

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("\"%s\": interface %s\n"),
//              ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
//              (enabled_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled/removed"))));
//#else
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("\"%s\": interface %s\n"),
//              ACE_TEXT (interfaceIdentifier_in.c_str ()),
//              (enabled_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled/removed"))));
//#endif
}

template <typename ModuleConfigurationType,
          typename ConnectionConfigurationIteratorType>
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onScanComplete (REFGUID interfaceIdentifier_in)
#else
ARDrone_NavDataStream_T<ModuleConfigurationType,
                        ConnectionConfigurationIteratorType>::onScanComplete (const std::string& interfaceIdentifier_in)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream_T::onScanComplete"));

//  // sanity check(s)
//  if (!isInitialized_ || !isActive_)
//    return;
//  ACE_ASSERT (configuration_);
//  if (!configuration_->autoAssociate)
//    return;
//
//  if (!associate (interfaceIdentifier_in,
//                  configuration_->SSID))
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
//                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
//                ACE_TEXT (configuration_->SSID.c_str ())));
//#else
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
//                ACE_TEXT (interfaceIdentifier_in.c_str ()),
//                ACE_TEXT (configuration_->SSID.c_str ())));
//#endif
}

//////////////////////////////////////////

template <typename ModuleConfigurationType>
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::ARDrone_MAVLinkStream_T ()
 : inherited ()
 , inherited2 (&(inherited::sessionDataLock_))
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream_T::ARDrone_MAVLinkStream_T"));

  inherited::state_.type = ARDRONE_STREAM_MAVLINK;
}

template <typename ModuleConfigurationType>
bool
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::load (Stream_ModuleList_t& modules_out,
                                                        bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream_T::load"));

  Stream_Module_t* module_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (inherited::configuration_->configuration_.mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_FileWriter_Module (this,
      //                                                    ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#if defined (_DEBUG)
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_Dump_Module (this,
      //                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#endif
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_StatisticReport_Module (this,
                                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_DirectShow_MAVLinkDecoder_Module (this,
                                                                       ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_UDPSource_Module (this,
                                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_DirectShow_AsynchUDPSource_Module (this,
                                                                          ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_FileWriter_Module (this,
      //                                                    ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#if defined (_DEBUG)
      //  ACE_NEW_RETURN (module_p,
      //                  ARDrone_Module_Dump_Module (this,
      //                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
      //                  false);
      //  modules_out.push_back (module_p);
      //  module_p = NULL;
      //#endif
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_StatisticReport_Module (this,
                                                                             ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      ACE_NEW_RETURN (module_p,
                      ARDrone_Module_MediaFoundation_MAVLinkDecoder_Module (this,
                                                                            ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                      false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      module_p = NULL;
      if (inherited::configuration_->configuration_.useReactor)
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_UDPSource_Module (this,
                                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      else
        ACE_NEW_RETURN (module_p,
                        ARDrone_Module_MediaFoundation_AsynchUDPSource_Module (this,
                                                                               ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                        false);
      ACE_ASSERT (module_p);
      modules_out.push_back (module_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown media framework (was: %d), aborting\n"),
                  ACE_TEXT (video_stream_name_string_),
                  inherited::configuration_->configuration_.mediaFramework));
      break;
    }
  } // end SWITCH
#else
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (this,
//                                                    ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#if defined (_DEBUG)
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_Dump_Module (this,
//                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_MAVLinkDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                  false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_UDPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchUDPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  ACE_ASSERT (module_p);
  modules_out.push_back (module_p);
#endif
  delete_out = true;

  return true;
}

template <typename ModuleConfigurationType>
bool
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::initialize (const CONFIGURATION_T& configuration_in)
#else
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream_T::initialize"));

  if (inherited::isInitialized_)
  {
  } // end IF

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
//  struct ARDrone_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  ModuleConfigurationType* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;
  Common_ISetP_T<struct ARDrone_StreamState>* iset_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  //session_data_p =
  //  &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));

  // sanity check(s)
  ACE_ASSERT (iterator != configuration_in.end ());

  configuration_p =
      dynamic_cast<ModuleConfigurationType*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);

  configuration_p->subscribers->push_back (this);
  configuration_p->subscribers->sort ();
  configuration_p->subscribers->unique (SUBSCRIBERS_IS_EQUAL_P ());

  // ---------------------------------------------------------------------------

  // ***************************** Statistic **********************************
//  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
//    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (statistic_.writer ());
//  if (!statistic_impl_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<ARDrone_Module_RuntimeStatistic> failed, aborting\n")));
//    return false;
//  } // end IF
//  if (!statistic_impl_p->initialize (configuration_in.statisticReportingInterval,
//                                     configuration_in.messageAllocator))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
//                statistic_.name ()));
//    return false;
//  } // end IF

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_),
                ACE_TEXT (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

  iset_p =
    dynamic_cast<Common_ISetP_T<struct ARDrone_StreamState>*> (module_p->writer ());
  ACE_ASSERT (iset_p);
  iset_p->setP (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  ACE_ASSERT (configuration_in.configuration_.initializeMAVLink);
  if (!configuration_in.configuration_.initializeMAVLink->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (mavlink_stream_name_string_)));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

template <typename ModuleConfigurationType>
void
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::start (Stream_SessionId_t sessionId_in,
                                                         const struct ARDrone_SessionData& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream_T::start"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (sessionData_in.state);
  if (sessionData_in.state->type != ARDRONE_STREAM_MAVLINK)
    return;

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = true;
  } // end lock scope

  try {
    inherited2::startCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::startCB(), continuing\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
  }
}

template <typename ModuleConfigurationType>
void
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::notify (Stream_SessionId_t sessionId_in,
                                                          const enum Stream_SessionMessageType& event_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream_T::notify"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  ACE_UNUSED_ARG (event_in);
}

template <typename ModuleConfigurationType>
void
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream_T::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  try {
    inherited2::endCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::endCB(), continuing\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
  }

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = false;
  } // end lock scope
}

template <typename ModuleConfigurationType>
void
ARDrone_MAVLinkStream_T<ModuleConfigurationType>::messageCB (const struct __mavlink_message& record_in,
                                                             void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream_T::messageCB"));

  ACE_UNUSED_ARG (payload_in);

  switch (record_in.msgid)
  {
    case MAVLINK_MSG_ID_HEARTBEAT: // 0
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_heartbeat_t));
      struct __mavlink_heartbeat_t* message_p =
          reinterpret_cast<struct __mavlink_heartbeat_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_SYS_STATUS: // 1
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_sys_status_t));
      struct __mavlink_sys_status_t* message_p =
          reinterpret_cast<struct __mavlink_sys_status_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_PARAM_VALUE: // 22
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_param_value_t));
      struct __mavlink_param_value_t* message_p =
          reinterpret_cast<struct __mavlink_param_value_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_GPS_RAW_INT: // 24
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_gps_raw_int_t));
      struct __mavlink_gps_raw_int_t* message_p =
          reinterpret_cast<struct __mavlink_gps_raw_int_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_ATTITUDE: // 30
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_attitude_t));
      struct __mavlink_attitude_t* message_p =
          reinterpret_cast<struct __mavlink_attitude_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // 33
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_global_position_int_t));
      struct __mavlink_global_position_int_t* message_p =
          reinterpret_cast<struct __mavlink_global_position_int_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_MISSION_CURRENT: // 42
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_mission_current_t));
      struct __mavlink_mission_current_t* message_p =
          reinterpret_cast<struct __mavlink_mission_current_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown MAVLink message id (was: %u), continuing\n"),
                  ACE_TEXT (mavlink_stream_name_string_),
                  record_in.msgid));
      break;
    }
  } // end SWITCH
}
