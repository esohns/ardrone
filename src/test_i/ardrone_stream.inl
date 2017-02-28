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

#include <ace/Log_Msg.h>

//#include "common_ui_defines.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#endif

#include "ardrone_defines.h"
#include "ardrone_macros.h"
#include "ardrone_stream_common.h"

template <typename SourceModuleType>
ARDrone_VideoStream_T<SourceModuleType>::ARDrone_VideoStream_T ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("ARDroneVideoStream"))
#if defined (ACE_WIN32) || defined (ACE_WIN64)
 , graphBuilder_ (NULL)
 , mediaSession_ (NULL)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::ARDrone_VideoStream_T"));

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
        (result != MF_E_SHUTDOWN)) // already shut down...
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
    mediaSession_->Release ();
  } // end IF
#endif

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

template <typename SourceModuleType>
bool
ARDrone_VideoStream_T<SourceModuleType>::load (Stream_ModuleList_t& modules_out,
                                               bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::load"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  Stream_Module_t* module_p = NULL;
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter"),
                                                    NULL,
                                                    false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (inherited::configuration_->useMediaFoundation)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_MediaFoundationDisplay_Module (ACE_TEXT_ALWAYS_CHAR ("Display"),
                                                                  NULL,
                                                                  false),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_DirectShowDisplay_Module (ACE_TEXT_ALWAYS_CHAR ("Display"),
                                                             NULL,
                                                             false),
                    false);
  modules_out.push_back (module_p);
  module_p = NULL;
#else
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_Display_Module (ACE_TEXT_ALWAYS_CHAR ("Display"),
                                                 NULL,
                                                 false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_H264Decoder_Module (ACE_TEXT_ALWAYS_CHAR ("H264Decoder"),
                                                     NULL,
                                                     false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_PaVEDecoder_Module (ACE_TEXT_ALWAYS_CHAR ("PaVEDecoder"),
                                                     NULL,
                                                     false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  SourceModuleType (ACE_TEXT_ALWAYS_CHAR ("NetCamSource"),
                                    NULL,
                                    false),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

template <typename SourceModuleType>
bool
ARDrone_VideoStream_T<SourceModuleType>::initialize (const ARDrone_StreamConfiguration& configuration_in,
                                                     bool setupPipeline_in,
                                                     bool resetSessionData_in)
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

  // allocate a new session state, reset stream
  if (!inherited::initialize (configuration_in,
                              false,
                              resetSessionData_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name ().c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (inherited::sessionData_);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first)
  ARDrone_SessionData& session_data_r =
    const_cast<ARDrone_SessionData&> (inherited::sessionData_->get ());
  session_data_r.sessionID = configuration_in.sessionID;
  //  ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration->streamState = &inherited::state_;

  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------

  // ******************* Display Handler ***************************************
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("Display")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("Display")));
    return false;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_Module_MediaFoundationDisplay* mediafoundation_display_impl_p = NULL;
  ARDrone_Module_DirectShowDisplay* directshow_display_impl_p = NULL;
  if (configuration_in.useMediaFoundation)
  {
    mediafoundation_display_impl_p =
      dynamic_cast<ARDrone_Module_MediaFoundationDisplay*> (module_p->writer ());
    if (!mediafoundation_display_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<ARDrone_Module_MediaFoundationDisplay*> failed, aborting\n")));
      return false;
    } // end IF
  } // end IF
  else
  {
    directshow_display_impl_p =
      dynamic_cast<ARDrone_Module_DirectShowDisplay*> (module_p->writer ());
    if (!directshow_display_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<ARDrone_Module_DirectShowDisplay*> failed, aborting\n")));
      return false;
    } // end IF
  } // end ELSE

  // sanity check(s)
  // *TODO*: remove type inference
  ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
  ACE_ASSERT (configuration_in.moduleHandlerConfiguration->format);

  if (configuration_in.useMediaFoundation)
  {
    // sanity check(s)
    ACE_ASSERT (mediafoundation_display_impl_p);

    //if (!mediafoundation_display_impl_p->initialize (*configuration_in.moduleHandlerConfiguration->format))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to initialize \"%s\" module, aborting\n"),
    //              ACE_TEXT ("Display")));
    //  return false;
    //} // end IF
  } // end IF
  else
  {
    //// sanity check(s)
    //ACE_ASSERT (inherited::sessionData_);

    //const struct ARDrone_SessionData& session_data_r =
    //  inherited::sessionData_->get ();

    //// sanity check(s)
    //ACE_ASSERT (session_data_r.graphBuilder);

    //struct _AllocatorProperties allocator_properties;
    //ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
    //// *TODO*: IMemAllocator::SetProperties returns VFW_E_BADALIGN (0x8004020e)
    ////         if this is -1/0 (why ?)
    ////allocator_properties.cbAlign = -1;  // <-- use default
    //allocator_properties.cbAlign = 1;
    //allocator_properties.cbBuffer = configuration_in.bufferSize;
    //allocator_properties.cbPrefix = -1; // <-- use default
    //allocator_properties.cBuffers =
    //  MODULE_DEV_CAM_DIRECTSHOW_DEFAULT_DEVICE_BUFFERS;
    //IAMBufferNegotiation* buffer_negotiation_p = NULL;
    //if (!Stream_Module_Device_DirectShow_Tools::getBufferNegotiation (session_data_r.graphBuilder,
    //                                                                  (configuration_in.moduleHandlerConfiguration->push ? MODULE_MISC_DS_WIN32_FILTER_NAME_SOURCE_L
    //                                                                                                                     : MODULE_MISC_DS_WIN32_FILTER_NAME_ASYNCH_SOURCE_L),
    //                                                                  buffer_negotiation_p))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::getBufferNegotiation(), aborting\n")));
    //  goto error;
    //} // end IF
    //ACE_ASSERT (buffer_negotiation_p);
    //HRESULT result_2 =
    //    buffer_negotiation_p->SuggestAllocatorProperties (&allocator_properties);
    //if (FAILED (result_2))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to IAMBufferNegotiation::SuggestAllocatorProperties(): \"%s\", aborting\n"),
    //              ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
    //
    //  // clean up
    //  buffer_negotiation_p->Release ();
    //
    //  goto error;
    //} // end IF
    //buffer_negotiation_p->Release ();

    //if (!directshow_display_impl_p->initialize (configuration_in.moduleHandlerConfiguration->format))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to initialize \"%s\" module, aborting\n"),
    //              ACE_TEXT ("Display")));
    //  return false;
    //} // end IF
  } // end IF
#endif

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
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("NetCamSource")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("NetCamSource")));
    return false;
  } // end IF

  typename SourceModuleType::WRITER_T* sourceWriter_impl_p =
    dynamic_cast<typename SourceModuleType::WRITER_T*> (module_p->writer ());
  if (!sourceWriter_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<SourceModuleType::WRITER_T> failed, aborting\n")));
    return false;
  } // end IF

  if (!sourceWriter_impl_p->initialize (inherited::state_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
                module_p->name ()));
    return false;
  } // end IF

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // -------------------------------------------------------------

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  bool graph_loaded = false;
//  bool COM_initialized = false;
//  HRESULT result_2 = E_FAIL;
//  ULONG reference_count = 0;
//
//  result_2 = CoInitializeEx (NULL,
//                             (COINIT_MULTITHREADED    |
//                              COINIT_DISABLE_OLE1DDE  |
//                              COINIT_SPEED_OVER_MEMORY));
//  if (FAILED (result_2))
//  {
//    if (result_2 == RPC_E_CHANGED_MODE)
//      ACE_DEBUG ((LM_WARNING,
//                  ACE_TEXT ("failed to CoInitializeEx(): \"%s\", continuing\n"),
//                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//    else
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to CoInitializeEx(): \"%s\", aborting\n"),
//                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//      return false;
//    } // end ELSE
//  } // end IF
//  COM_initialized = true;
//
//  IDirect3DDeviceManager9* direct3D_manager_p = NULL;
//  struct _D3DPRESENT_PARAMETERS_ d3d_presentation_parameters;
//  IMFTopology* topology_p = NULL;
//  IMFMediaType* media_type_p = NULL;
//
//  if (!configuration_in.useMediaFoundation)
//  {
//    // --> using direct show
//
//    // sanity check(s)
//    ACE_ASSERT (configuration_in.moduleHandlerConfiguration);
//    ACE_ASSERT (directshow_display_impl_p);
//
//    struct _AllocatorProperties allocator_properties;
//    IAMBufferNegotiation* buffer_negotiation_p = NULL;
//    bool release_builder = false;
//    ULONG reference_count = 0;
//    IAMStreamConfig* stream_config_p = NULL;
//    IMediaFilter* media_filter_p = NULL;
//    Stream_Module_Device_DirectShow_Graph_t graph_configuration;
//    struct Stream_Module_Device_DirectShow_GraphEntry graph_entry;
//    graph_entry.filterName =
//      (configuration_in.moduleHandlerConfiguration->push ? MODULE_MISC_DS_WIN32_FILTER_NAME_SOURCE_L
//                                                         : MODULE_MISC_DS_WIN32_FILTER_NAME_ASYNCH_SOURCE_L);
//    std::string log_file_name;
//
//    if (configuration_in.moduleHandlerConfiguration->graphBuilder)
//    {
//      reference_count =
//        configuration_in.moduleHandlerConfiguration->graphBuilder->AddRef ();
//      graphBuilder_ = configuration_in.moduleHandlerConfiguration->graphBuilder;
//
//      if (!Stream_Module_Device_DirectShow_Tools::resetGraph (graphBuilder_,
//                                                              GUID_NULL))
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::resetGraph(), aborting\n")));
//        goto error;
//      } // end IF
//
//      if (!Stream_Module_Device_DirectShow_Tools::getBufferNegotiation (graphBuilder_,
//                                                                        graph_entry.filterName,
//                                                                        buffer_negotiation_p))
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::getBufferNegotiation(), aborting\n")));
//        goto error;
//      } // end IF
//
//      goto continue_;
//    } // end IF
//
//    if (!Stream_Module_Device_DirectShow_Tools::loadSourceGraph (directshow_display_impl_p,
//                                                                 graph_entry.filterName,
//                                                                 graphBuilder_,
//                                                                 buffer_negotiation_p,
//                                                                 stream_config_p))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::loadDeviceGraph(\"%s\"), aborting\n"),
//                  ACE_TEXT_WCHAR_TO_TCHAR (graph_entry.filterName.c_str ())));
//      goto error;
//    } // end IF
//    ACE_ASSERT (graphBuilder_);
//    ACE_ASSERT (buffer_negotiation_p);
//    ACE_ASSERT (stream_config_p);
//
//    // clean up
//    stream_config_p->Release ();
//    stream_config_p = NULL;
//
//    reference_count = graphBuilder_->AddRef ();
//    configuration_in.moduleHandlerConfiguration->graphBuilder = graphBuilder_;
//    release_builder = true;
//
//continue_:
//#if defined (_DEBUG)
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("%s: capture format: \"%s\"...\n"),
//                ACE_TEXT (inherited::name_.c_str ()),
//                ACE_TEXT (Stream_Module_Device_DirectShow_Tools::mediaTypeToString (*configuration_in.moduleHandlerConfiguration->format, true).c_str ())));
//
//    log_file_name =
//      Common_File_Tools::getLogDirectory (std::string (),
//                                          0);
//    log_file_name += ACE_DIRECTORY_SEPARATOR_STR;
//    log_file_name += MODULE_DEV_DIRECTSHOW_LOGFILE_NAME;
//    Stream_Module_Device_DirectShow_Tools::debug (graphBuilder_,
//                                                  log_file_name);
//#endif
//    // sanity check(s)
//    ACE_ASSERT (!session_data_r.direct3DDevice);
//
//    if (!Stream_Module_Device_Tools::getDirect3DDevice (configuration_in.moduleHandlerConfiguration->window,
//                                                        *configuration_in.moduleHandlerConfiguration->format,
//                                                        session_data_r.direct3DDevice,
//                                                        d3d_presentation_parameters,
//                                                        direct3D_manager_p,
//                                                        session_data_r.resetToken))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_Tools::getDirect3DDevice(), aborting\n")));
//
//      // clean up
//      buffer_negotiation_p->Release ();
//
//      goto error;
//    } // end IF
//    ACE_ASSERT (direct3D_manager_p);
//
//    if (!Stream_Module_Device_DirectShow_Tools::loadTargetRendererGraph (GUID_NULL,
//                                                                         *configuration_in.moduleHandlerConfiguration->format,
//                                                                         configuration_in.moduleHandlerConfiguration->window,
//                                                                         graphBuilder_,
//                                                                         graph_configuration))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::loadVideoRendererGraph(), aborting\n")));
//
//      // clean up
//      buffer_negotiation_p->Release ();
//      direct3D_manager_p->Release ();
//
//      goto error;
//    } // end IF
//    graph_loaded = true;
//    graph_configuration.push_front (graph_entry);
//
//    ACE_ASSERT (buffer_negotiation_p);
//    ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
//    // *TODO*: IMemAllocator::SetProperties returns VFW_E_BADALIGN (0x8004020e)
//    //         if this is -1/0 (why ?)
//    //allocator_properties.cbAlign = -1;  // <-- use default
//    allocator_properties.cbAlign = 1;
//    allocator_properties.cbBuffer = configuration_in.bufferSize;
//    allocator_properties.cbPrefix = -1; // <-- use default
//    allocator_properties.cBuffers =
//      MODULE_DEV_CAM_DIRECTSHOW_DEFAULT_DEVICE_BUFFERS;
//    result_2 =
//        buffer_negotiation_p->SuggestAllocatorProperties (&allocator_properties);
//    if (FAILED (result_2))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to IAMBufferNegotiation::SuggestAllocatorProperties(): \"%s\", aborting\n"),
//                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//
//      // clean up
//      buffer_negotiation_p->Release ();
//      direct3D_manager_p->Release ();
//
//      goto error;
//    } // end IF
//
//    // *NOTE*: the sample grabber has a few limitations:
//    //         --> see also: https://msdn.microsoft.com/en-us/library/windows/desktop/dd377544(v=vs.85).aspx
//    //         •It does not support video types with top - down orientation (negative biHeight).
//    //         •It does not support the VIDEOINFOHEADER2 format structure (format type equal to FORMAT_VideoInfo2).
//    //         •It rejects any video type where the surface stride does not match the video width.
//    //         --> remove it from the pipeline
//
//    if (!Stream_Module_Device_DirectShow_Tools::connect (graphBuilder_,
//                                                         graph_configuration))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::connect(), aborting\n")));
//
//      // clean up
//      buffer_negotiation_p->Release ();
//      direct3D_manager_p->Release ();
//
//      goto error;
//    } // end IF
//    if (!Stream_Module_Device_DirectShow_Tools::connected (graphBuilder_,
//                                                           graph_entry.filterName))
//    {
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("reconnecting...\n")));
//
//      if (!Stream_Module_Device_DirectShow_Tools::connectFirst (graphBuilder_,
//                                                                graph_entry.filterName))
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::connectFirst(), aborting\n")));
//
//        // clean up
//        buffer_negotiation_p->Release ();
//        direct3D_manager_p->Release ();
//
//        goto error;
//      } // end IF
//    } // end IF
//    ACE_ASSERT (Stream_Module_Device_DirectShow_Tools::connected (graphBuilder_,
//                                                                  graph_entry.filterName));
//
//    // debug info
//    // *TODO*: find out why this fails
//    ACE_OS::memset (&allocator_properties, 0, sizeof (allocator_properties));
//    result_2 =
//        buffer_negotiation_p->GetAllocatorProperties (&allocator_properties);
//    if (FAILED (result_2)) // E_FAIL (0x80004005)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to IAMBufferNegotiation::GetAllocatorProperties(): \"%s\", continuing\n"),
//                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//      //goto error;
//    } // end IF
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("allocator properties (buffers/size/alignment/prefix): %d/%d/%d/%d\n"),
//                allocator_properties.cBuffers,
//                allocator_properties.cbBuffer,
//                allocator_properties.cbAlign,
//                allocator_properties.cbPrefix));
//    buffer_negotiation_p->Release ();
//
//    result_2 = graphBuilder_->QueryInterface (IID_PPV_ARGS (&media_filter_p));
//    if (FAILED (result_2))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to IGraphBuilder::QueryInterface(IID_IMediaFilter): \"%s\", aborting\n"),
//                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//
//      // clean up
//      direct3D_manager_p->Release ();
//
//      goto error;
//    } // end IF
//    ACE_ASSERT (media_filter_p);
//    result_2 = media_filter_p->SetSyncSource (NULL);
//    if (FAILED (result_2))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to IMediaFilter::SetSyncSource(): \"%s\", aborting\n"),
//                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//
//      // clean up
//      direct3D_manager_p->Release ();
//
//      goto error;
//    } // end IF
//    media_filter_p->Release ();
//
//    if (session_data_r.format)
//      Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_r.format);
//    ACE_ASSERT (!session_data_r.format);
//    if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*configuration_in.moduleHandlerConfiguration->format,
//                                                               session_data_r.format))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), aborting\n")));
//
//      // clean up
//      direct3D_manager_p->Release ();
//
//      goto error;
//    } // end IF
//    ACE_ASSERT (session_data_r.format);
//
//    if (session_data_r.graphBuilder)
//    {
//      session_data_r.graphBuilder->Release ();
//      session_data_r.graphBuilder = NULL;
//    } // end IF
//    ACE_ASSERT (!session_data_r.graphBuilder);
//    reference_count = graphBuilder_->AddRef ();
//    session_data_r.graphBuilder = graphBuilder_;
//    ACE_ASSERT (session_data_r.graphBuilder);
//
//    direct3D_manager_p->Release ();
//
//    goto setup_;
//  } // end IF
//
//  // --> using media foundation
//
//  if (configuration_in.moduleHandlerConfiguration->session)
//  {
//    enum MFSESSION_GETFULLTOPOLOGY_FLAGS flags =
//      MFSESSION_GETFULLTOPOLOGY_CURRENT;
//    ACE_Time_Value timeout =
//      (COMMON_TIME_NOW +
//       ACE_Time_Value (COMMON_WIN32_MEDIAFOUNDATION_TOPOLOGY_GET_TIMEOUT, 0));
//
//    // *NOTE*: IMFMediaSession::SetTopology() is asynchronous; subsequent calls
//    //         to retrieve the topology handle may fail (MF_E_INVALIDREQUEST)
//    //         --> (try to) wait for the next MESessionTopologySet event
//    // *NOTE*: this procedure doesn't always work as expected (GetFullTopology()
//    //         still fails with MF_E_INVALIDREQUEST)
//    do
//    {
//      result_2 = mediaSession_->GetFullTopology (flags,
//                                                 0,
//                                                 &topology_p);
//    } while ((result_2 == MF_E_INVALIDREQUEST) &&
//             (COMMON_TIME_NOW < timeout));
//    if (FAILED (result_2)) // MF_E_INVALIDREQUEST: 0xC00D36B2L
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to IMFMediaSession::GetFullTopology(): \"%s\", aborting\n"),
//                  ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//      goto error;
//    } // end IF
//    ACE_ASSERT (topology_p);
//
//    goto continue_2;
//  } // end IF
//
//  // create a topology
//
//  // sanity check(s)
//  ACE_ASSERT (mediafoundation_display_impl_p);
//
//  result_2 = MFCreateMediaType (&media_type_p);
//  if (FAILED (result_2))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to MFCreateMediaType(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//    goto error;
//  } // end IF
//  result_2 =
//    MFInitMediaTypeFromAMMediaType (media_type_p,
//                                    configuration_in.moduleHandlerConfiguration->format);
//  if (FAILED (result_2))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to MFInitMediaTypeFromAMMediaType(): \"%s\", aborting\n"),
//                ACE_TEXT (Common_Tools::error2String (result_2).c_str ())));
//    goto error;
//  } // end IF
//  // *NOTE*: MFInitMediaTypeFromAMMediaType erroneously sets
//  //         MF_MT_ALL_SAMPLES_INDEPENDENT to 'true' for H264 --> reset
//  result_2 = media_type_p->SetUINT32 (MF_MT_ALL_SAMPLES_INDEPENDENT, 0);
//  ACE_ASSERT (SUCCEEDED (result_2));
//
//  if (!Stream_Module_Device_MediaFoundation_Tools::loadSourceTopology (mediafoundation_display_impl_p,
//                                                                       topology_p))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_MediaFoundation_Tools::loadSourceTopology(), aborting\n")));
//    goto error;
//  } // end IF
//  ACE_ASSERT (topology_p);
//  if (!Stream_Module_Device_MediaFoundation_Tools::loadVideoRendererTopology (media_type_p,
//                                                                              configuration_in.moduleHandlerConfiguration->window,
//                                                                              configuration_in.moduleHandlerConfiguration->rendererNodeId,
//                                                                              topology_p))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_MediaFoundation_Tools::loadVideoRendererTopology(), aborting\n")));
//    goto error;
//  } // end IF
//  graph_loaded = true;
//
//  media_type_p->Release ();
//  media_type_p = NULL;
//
//continue_2:
//  ACE_ASSERT (topology_p);
//#if defined (_DEBUG)
//  Stream_Module_Device_MediaFoundation_Tools::dump (topology_p);
//
//  //ACE_DEBUG ((LM_DEBUG,
//  //            ACE_TEXT ("input format: \"%s\"...\n"),
//  //            ACE_TEXT (Stream_Module_Device_DirectShow_Tools::mediaTypeToString (*configuration_in.moduleHandlerConfiguration->format).c_str ())));
//#endif
//
//  if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*configuration_in.moduleHandlerConfiguration->format,
//                                                             session_data_r.format))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), aborting\n")));
//    goto error;
//  } // end IF
//  ACE_ASSERT (session_data_r.format);
//
//  if (mediaSession_)
//  {
//    // *TODO*: this crashes in CTopoNode::UnlinkInput ()...
//    //result = mediaSession_->Shutdown ();
//    //if (FAILED (result))
//    //  ACE_DEBUG ((LM_ERROR,
//    //              ACE_TEXT ("failed to IMFMediaSession::Shutdown(): \"%s\", continuing\n"),
//    //              ACE_TEXT (Common_Tools::error2String (result).c_str ())));
//    mediaSession_->Release ();
//    mediaSession_ = NULL;
//  } // end IF
//  ACE_ASSERT (!mediaSession_);
//  if (configuration_in.moduleHandlerConfiguration->session)
//  {
//    reference_count =
//      configuration_in.moduleHandlerConfiguration->session->AddRef ();
//    mediaSession_ = configuration_in.moduleHandlerConfiguration->session;
//  } // end IF
//
//  if (!Stream_Module_Device_MediaFoundation_Tools::setTopology (topology_p,
//                                                                mediaSession_,
//                                                                false,
//                                                                true))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Stream_Module_Device_MediaFoundation_Tools::setTopology(), aborting\n")));
//    goto error;
//  } // end IF
//  // *TODO*: this crashes in CMFTopology::Clear()
//  //topology_p->Release ();
//  topology_p = NULL;
//  ACE_ASSERT (mediaSession_);
//
//  //if (!configuration_in.moduleHandlerConfiguration->session)
//  //{
//  //  reference_count = mediaSession_->AddRef ();
//  //  configuration_in.moduleHandlerConfiguration->session = mediaSession_;
//  //} // end IF
//  reference_count = mediaSession_->AddRef ();
//  session_data_r.session = mediaSession_;
//
//setup_:
//#endif

  // ---------------------------------------------------------------------------

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (session_data_r.format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_r.format);
  ACE_ASSERT (!session_data_r.format);
  if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*configuration_in.moduleHandlerConfiguration->format,
                                                             session_data_r.format))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (session_data_r.format);
#endif

  // ---------------------------------------------------------------------------

  if (setupPipeline_in)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to setup pipeline, aborting\n")));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //if (media_type_p)
  //  media_type_p->Release ();
  //if (topology_p)
  //  topology_p->Release ();
  if (session_data_r.direct3DDevice)
  {
    session_data_r.direct3DDevice->Release ();
    session_data_r.direct3DDevice = NULL;
  } // end IF
  if (session_data_r.format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_r.format);
  session_data_r.resetToken = 0;
  if (session_data_r.session)
  {
    session_data_r.session->Release ();
    session_data_r.session = NULL;
  } // end IF
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
void
ARDrone_VideoStream_T<SourceModuleType>::ping ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::ping"));

//  Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
//  protocolHandler_impl = dynamic_cast<Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
//  if (!protocolHandler_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Net_Module_ProtocolHandler> failed, returning\n")));

//    return;
//  } // end IF

//  // delegate to this module
//  protocolHandler_impl->handleTimeout (NULL);

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

template <typename SourceModuleType>
bool
ARDrone_VideoStream_T<SourceModuleType>::collect (ARDrone_RuntimeStatistic_t& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::collect"));

  int result = -1;
  ARDrone_SessionData& session_data_r =
      const_cast<ARDrone_SessionData&> (inherited::sessionData_->get ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("StatisticReport")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("StatisticReport")));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
  } // end IF

  return result_2;
}

template <typename SourceModuleType>
void
ARDrone_VideoStream_T<SourceModuleType>::report () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_VideoStream_T::report"));

  ACE_ASSERT (inherited::state_.currentSessionData);

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTICS ***\n--> Stream Statistics @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTICS ***\\END\n"),
              inherited::state_.currentSessionData->sessionID,
              &(inherited::state_.currentSessionData->lastCollectionTimeStamp),
              inherited::state_.currentSessionData->currentStatistic.dataMessages,
              inherited::state_.currentSessionData->currentStatistic.droppedFrames,
              inherited::state_.currentSessionData->currentStatistic.bytes));
}
