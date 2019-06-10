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

#include "ace/Log_Msg.h"

#include "stream_macros.h"

#include "ardrone_tools.h"
#include "ardrone_types.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename MediaType,
          typename UserDataType>
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                             TimePolicyType,
                             ConfigurationType,
                             ControlMessageType,
                             DataMessageType,
                             SessionMessageType,
                             MediaType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                             UserDataType>::ARDrone_Module_PaVEDecoder_T (ISTREAM_T* stream_in)
#else
                             UserDataType>::ARDrone_Module_PaVEDecoder_T (typename inherited::ISTREAM_T* stream_in)
#endif // ACE_WIN32 || ACE_WIN64
 : inherited (stream_in)
 , buffer_ (NULL)
 , header_ ()
 , headerDecoded_ (false)
 , videoMode_ (ARDRONE_VIDEOMODE_INVALID)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::ARDrone_Module_PaVEDecoder_T"));

  ACE_OS::memset (&header_, 0, sizeof (parrot_video_encapsulation_t));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename MediaType,
          typename UserDataType>
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                             TimePolicyType,
                             ConfigurationType,
                             ControlMessageType,
                             DataMessageType,
                             SessionMessageType,
                             MediaType,
                             UserDataType>::~ARDrone_Module_PaVEDecoder_T ()
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::~ARDrone_Module_PaVEDecoder_T"));

  // clean up any unprocessed (chained) buffer(s)
  if (unlikely (buffer_))
    buffer_->release ();
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename MediaType,
          typename UserDataType>
bool
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                             TimePolicyType,
                             ConfigurationType,
                             ControlMessageType,
                             DataMessageType,
                             SessionMessageType,
                             MediaType,
                             UserDataType>::initialize (const ConfigurationType& configuration_in,
                                                        Stream_IAllocator* allocator_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::initialize"));

  if (unlikely (inherited::isInitialized_))
  {
    if (buffer_)
    {
      buffer_->release (); buffer_ = NULL;
    } // end IF
    videoMode_ = ARDRONE_VIDEOMODE_INVALID;
  } // end IF

  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename MediaType,
          typename UserDataType>
void
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                             TimePolicyType,
                             ConfigurationType,
                             ControlMessageType,
                             DataMessageType,
                             SessionMessageType,
                             MediaType,
                             UserDataType>::handleDataMessage (DataMessageType*& message_inout,
                                                               bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::handleDataMessage"));

  int result = -1;
  ACE_Message_Block* message_block_p, *message_block_2 = NULL;
  unsigned int skipped_bytes, trailing_bytes = 0;
  unsigned int length = 0;
  unsigned int buffered_bytes, missing_bytes, bytes_to_copy;
  enum ARDrone_VideoMode video_mode_e = ARDRONE_VIDEOMODE_INVALID;

  // initialize return value(s)
  // *NOTE*: the default behavior is to pass all messages along
  //         --> in this case, the individual frames are extracted and passed
  //             as such
  passMessageDownstream_out = false;

  // form a chain of buffers
  if (buffer_)
  {
    message_block_p = buffer_->cont ();
    if (message_block_p)
    {
      while (message_block_p->cont ()) // skip to end
        message_block_p = message_block_p->cont ();
    } // end IF
    else
      message_block_p = buffer_;
    message_block_p->cont (message_inout); // chain the buffer
  } // end IF
  else
    buffer_ = message_inout;

next:
  buffered_bytes = buffer_->total_length ();
  message_block_p = buffer_;

  if (unlikely (!headerDecoded_))
  {
    // PaVE header has not been received yet

    // --> wait for more data ?
    if (buffered_bytes < sizeof (parrot_video_encapsulation_t))
      return; // done

    // received a PaVE header --> decode
    char* buffer_p = reinterpret_cast<char*> (&header_);
    skipped_bytes = 0;
    for (;
         message_block_p;
         message_block_p = message_block_p->cont ())
    {
      length = message_block_p->length ();
      bytes_to_copy =
        ((length < (sizeof (parrot_video_encapsulation_t) - skipped_bytes)) ? length
                                                                            : (sizeof (parrot_video_encapsulation_t) - skipped_bytes));
      ACE_OS::memcpy (buffer_p, message_block_p->rd_ptr (), bytes_to_copy);
      message_block_p->rd_ptr (bytes_to_copy);
      skipped_bytes += bytes_to_copy;
      if (skipped_bytes == sizeof (parrot_video_encapsulation_t))
        break;
      buffer_p += bytes_to_copy;
    } // end FOR
#if defined (_DEBUG)
    // *WARNING*: the PAVE_CHECK macro is endian- (i.e. platform-)dependent and
    //            therefore needs to be generated/compiled for each targeted
    //            platform separately (see: video_encapsulation.h)
    if (!PAVE_CHECK (header_.signature))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: corrupt PaVE header, continuing\n"),
                  inherited::mod_->name ()));
#endif // _DEBUG

    buffered_bytes -= sizeof (parrot_video_encapsulation_t);

    headerDecoded_ = true;
  } // end IF

  // update video mode ?
  video_mode_e =
    ((header_.display_width == ARDRONE_H264_360P_VIDEO_WIDTH) ? ARDRONE_VIDEOMODE_360P
                                                              : ARDRONE_VIDEOMODE_720P);
  if (unlikely (videoMode_ == ARDRONE_VIDEOMODE_INVALID))
    videoMode_ = video_mode_e;
  else
  {
    if (unlikely (videoMode_ != video_mode_e))
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: detected video mode change (was: %d, is: %d)\n"),
                  inherited::mod_->name (),
                  videoMode_, video_mode_e));
      videoMode_ = video_mode_e;

      // update session data (see below) and notify downstream
      typename SessionMessageType::DATA_T* session_data_container_p =
        inherited::sessionData_;
      if (session_data_container_p)
        session_data_container_p->increase ();
      if (!inherited::putSessionMessage (STREAM_SESSION_MESSAGE_RESIZE,
                                         session_data_container_p,
                                         NULL))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Stream_TaskBase_T::putSessionMessage(%d), continuing\n"),
                    inherited::mod_->name (),
                    STREAM_SESSION_MESSAGE_RESIZE));
    } // end IF
  } // end ELSE

  // --> wait for more data ?
  missing_bytes =
      header_.header_size - sizeof (parrot_video_encapsulation_t);
  ACE_ASSERT (missing_bytes >= 0);
  if (buffered_bytes < (missing_bytes + header_.payload_size))
    return; // done

  // skip over (undocumented) header bytes
  skipped_bytes = 0;
  for (;
       message_block_p;
       message_block_p = message_block_p->cont ())
  {
    length = message_block_p->length ();
    bytes_to_copy = ((length < missing_bytes) ? length : missing_bytes);
    message_block_p->rd_ptr (bytes_to_copy);
    skipped_bytes += bytes_to_copy;
    if (skipped_bytes == missing_bytes)
      break;
  } // end FOR
  buffered_bytes -= missing_bytes;

  // forward a H264 frame

  if (buffered_bytes > header_.payload_size)
  {
    missing_bytes = header_.payload_size;
    trailing_bytes = 0;
    for (ACE_Message_Block* message_block_3 = buffer_;
         missing_bytes;
         message_block_3 = message_block_3->cont ())
    { ACE_ASSERT (message_block_3);
      length = message_block_3->length ();
      length = (missing_bytes <= length ? missing_bytes : length);
      if (missing_bytes <= length)
      {
        message_block_p = message_block_3;
        trailing_bytes = message_block_3->length () - missing_bytes;
        break;
      } // end IF
      missing_bytes -= length;
    } // end FOR

    message_block_2 = message_block_p->duplicate ();
    if (unlikely (!message_block_2))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to MessageType::duplicate(): \"%m\", returning\n"),
                  inherited::mod_->name ()));
      return;
    } // end IF
    message_block_p->cont (NULL);

    if (trailing_bytes)
    {
      message_block_p->length (length);
      message_block_2->rd_ptr (length);
    } // end IF
  } // end IF
  else
    message_block_2 = NULL;
  ACE_ASSERT (buffer_->total_length () == header_.payload_size);
  buffer_->set (ARDRONE_MESSAGE_VIDEO);
  //buffer_->set_2 (inherited::stream_);
  result = inherited::put_next (buffer_, NULL);
  if (unlikely (result == -1))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task::put_next(): \"%m\", returning\n"),
                inherited::mod_->name ()));
    if (message_block_2)
      message_block_2->release ();
    buffer_->release (); buffer_ = NULL;
    return;
  } // end IF
  if (message_block_2)
  {
    buffer_ = dynamic_cast<DataMessageType*> (message_block_2);
    ACE_ASSERT (buffer_);
  } // end IF
  else
    buffer_ = NULL;

  // reset header
  headerDecoded_ = false;

  // parse next frame ?
  if (buffer_)
    goto next;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename MediaType,
          typename UserDataType>
void
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                             TimePolicyType,
                             ConfigurationType,
                             ControlMessageType,
                             DataMessageType,
                             SessionMessageType,
                             MediaType,
                             UserDataType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                  bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // update session data
      ACE_ASSERT (inherited::sessionData_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->getR ());
      // sanity check(s)
      // *NOTE*: the stream usually ought to initialize the format stack
      if (session_data_r.formats.empty ())
      {
        struct Stream_MediaFramework_FFMPEG_MediaType format_s;
        format_s.format = AV_PIX_FMT_NONE; // video is header- + H.264-encoded
        ARDroneVideoModeToResolution (videoMode_,
                                      format_s.resolution);
        MediaType media_type_s;
        inherited2::getMediaType (format_s,
                                  media_type_s);
         session_data_r.formats.push_back (media_type_s);
      } // end IF

      break;
    }
    case STREAM_SESSION_MESSAGE_RESIZE:
    {
      // update session data
      ACE_ASSERT (inherited::sessionData_);

      typename SessionMessageType::DATA_T::DATA_T& session_data_r =
          const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->getR ());
      // sanity check(s)
//      ACE_ASSERT (!session_data_r.formats.empty ());
//      MediaType& format_r =
//        const_cast<MediaType&> (session_data_r.formats.back ());
//      ARDroneVideoModeToResolution (videoMode_,
//                                    format_r.resolution);
      break;
    }
    default:
      break;
  } // end SWITCH
}
