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

#ifndef ARDRONE_MODULE_MAVLINK_DECODER_H
#define ARDRONE_MODULE_MAVLINK_DECODER_H

#include "mavlink.h"

#include "ace/Global_Macros.h"

#include "common.h"
#include "common_time_common.h"

#include "stream_misc_parser.h"

#include "ardrone_common.h"
#include "ardrone_types.h"
#include "ardrone_mavlink_scanner.h"

// forward declaration(s)
class ACE_Message_Block;
class Stream_IAllocator;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataContainerType>
class ARDrone_Module_MAVLinkDecoder_T
 : public Stream_Module_Parser_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Net_IScanner_T<ARDrone_MAVLink_IParser>,
                                 Net_IRecordParser_T<struct Common_ParserConfiguration,
                                                     struct __mavlink_message>,
                                 struct Common_ParserConfiguration,
                                 ARDrone_MAVLink_IParser,
                                 enum Stream_SessionMessageType,
                                 struct ARDrone_UserData>
{
  typedef Stream_Module_Parser_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Net_IScanner_T<ARDrone_MAVLink_IParser>,
                                 Net_IRecordParser_T<struct Common_ParserConfiguration,
                                                     struct __mavlink_message>,
                                 struct Common_ParserConfiguration,
                                 ARDrone_MAVLink_IParser,
                                 enum Stream_SessionMessageType,
                                 struct ARDrone_UserData> inherited;

 public:
  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_Module_MAVLinkDecoder_T (ISTREAM_T*); // stream handle
#else
  ARDrone_Module_MAVLinkDecoder_T (typename inherited::ISTREAM_T*); // stream handle
#endif
  virtual ~ARDrone_Module_MAVLinkDecoder_T ();

  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement ARDrone_MAVLink_IParser
  inline virtual struct __mavlink_message& current () { ACE_ASSERT (buffer_); return const_cast<struct __mavlink_message&> (buffer_->get ().get ().MAVLinkData); };
  inline virtual bool hasFinished () const { return true; };
  virtual void record (struct __mavlink_message*&); // record handle
  inline virtual bool initialize (const struct Common_ParserConfiguration& configuration_in) { ACE_UNUSED_ARG (configuration_in); return true; };
  inline virtual void dump_state () const {};
  inline virtual void error (const yy::location&,
                             const std::string& string_in) { error (string_in); };
  inline virtual bool parse (ACE_Message_Block*) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };

  // *NOTE*: this is the C interface (not needed by C++ scanners)
  inline virtual void debug (yyscan_t, bool) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual bool initialize (yyscan_t&) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };
  inline virtual void finalize (yyscan_t&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual struct yy_buffer_state* create (yyscan_t, char*, size_t) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (NULL); ACE_NOTREACHED (return NULL;) };
  inline virtual void destroy (yyscan_t, struct yy_buffer_state*&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual void set (ARDrone_MAVLink_IParser*) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };

  inline virtual ACE_Message_Block* buffer () { return buffer_; };
  inline virtual bool debugScanner () const { return ARDrone_MAVLink_Scanner_get_debug (scannerState_); };
  inline virtual bool isBlocking () const { return true; };
  virtual void error (const std::string&);
  inline virtual void offset (unsigned int offset_in) { ARDrone_MAVLink_Scanner_set_column (offset_in, scannerState_); };
  inline virtual unsigned int offset () const { return ARDrone_MAVLink_Scanner_get_column (scannerState_); };
  // *IMPORTANT NOTE*: when the parser detects a frame end, it inserts a new
  //                   buffer to the continuation and passes 'true'
  //                   --> separate the current frame from the next
  virtual bool switchBuffer (bool = false); // unlink current buffer ?
  virtual void waitBuffer ();

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MAVLinkDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MAVLinkDecoder_T (const ARDrone_Module_MAVLinkDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MAVLinkDecoder_T& operator= (const ARDrone_Module_MAVLinkDecoder_T&))

  // override some ACE_Task_T methods
  int svc (void);

  // helper methods
  bool scan_begin (const char*,   // buffer handle
                   unsigned int); // buffer size
  void scan_end ();

  DataMessageType*        buffer_;
  struct yy_buffer_state* bufferState_;
  bool                    isFirst_;
  yyscan_t                scannerState_;
  //std::string             scannerTables_;
  bool                    useYYScanBuffer_;
};

// include template definition
#include "ardrone_module_mavlink_decoder.inl"

#endif
