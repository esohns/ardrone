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
#include "ardrone_stream_common.h"

template <typename SourceModuleType>
ARDrone_VideoStream_T<SourceModuleType>::ARDrone_VideoStream_T ()
 : inherited ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
 , graphBuilder_ (NULL)
 , mediaSession_ (NULL)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::ARDrone_VideoStream_T"));

  inherited::state_.type = ARDRONE_STREAM_VIDEO;
}

template <typename SourceModuleType>
ARDrone_VideoStream_T<SourceModuleType>::~ARDrone_VideoStream_T ()
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

template <typename SourceModuleType>
bool
ARDrone_VideoStream_T<SourceModuleType>::load (Stream_ModuleList_t& modules_out,
                                               bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::load"));

  Stream_Module_t* module_p = NULL;
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_FileWriter_Module (this,
                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_FILE_SINK_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_AVIEncoder_Module (this,
                                                    ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_ENCODER_AVI_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (inherited::configuration_->configuration_.useMediaFoundation)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_MediaFoundationDisplay_Module (this,
                                                                  ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_MEDIAFOUNDATION_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_DirectShowDisplay_Module (this,
                                                             ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_DIRECTSHOW_DEFAULT_NAME_STRING)),
                    false);
  modules_out.push_back (module_p);
  module_p = NULL;
#else
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_Display_Module (this,
                                                 ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_GTK_PIXBUF_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_H264Decoder_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_PaVEDecoder_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_PAVE_DECODER_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  SourceModuleType (this,
                                    ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

template <typename SourceModuleType>
bool
ARDrone_VideoStream_T<SourceModuleType>::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
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
  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
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
    dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->filterConfiguration);
  ACE_ASSERT (configuration_p->filterConfiguration->pinConfiguration);
  ACE_ASSERT (configuration_p->filterConfiguration->pinConfiguration->format);

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  session_data_p =
    &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());

  if (session_data_p->format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_p->format);
  ACE_ASSERT (!session_data_p->format);
  if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*(configuration_p->filterConfiguration->pinConfiguration->format),
                                                             session_data_p->format))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (session_data_p->format);
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
  if (session_data_p->format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_p->format);
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

template <typename SourceModuleType>
bool
ARDrone_VideoStream_T<SourceModuleType>::collect (struct ARDrone_Statistic& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::collect"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  int result = -1;
  struct ARDrone_SessionData& session_data_r =
      const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (video_stream_name_string_),
                ACE_TEXT (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n"),
                ACE_TEXT (video_stream_name_string_)));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n"),
                  ACE_TEXT (video_stream_name_string_)));
      return false;
    } // end IF
  } // end IF

  session_data_r.statistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Common_IStatistic_T::collect(), continuing\n"),
                ACE_TEXT (video_stream_name_string_)));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Common_IStatistic_T::collect(), aborting\n"),
                ACE_TEXT (video_stream_name_string_)));
  else
    session_data_r.statistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n"),
                  ACE_TEXT (video_stream_name_string_)));
  } // end IF

  return result_2;
}

template <typename SourceModuleType>
void
ARDrone_VideoStream_T<SourceModuleType>::report () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::report"));

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %d] RUNTIME STATISTIC ***\n--> stream statistic @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTIC ***\\END\n"),
              (inherited::state_.sessionData ? static_cast<int> (inherited::state_.sessionData->sessionId) : -1),
              &(inherited::state_.sessionData->lastCollectionTimeStamp),
              inherited::state_.sessionData->statistic.dataMessages,
              inherited::state_.sessionData->statistic.droppedFrames,
              inherited::state_.sessionData->statistic.bytes));
}
