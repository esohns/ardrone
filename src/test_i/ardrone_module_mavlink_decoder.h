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

#include "stream_iparser.h"

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
                                 struct Common_ScannerState,
                                 Stream_IYaccStreamParser_T<struct Common_ParserConfiguration,
                                                            struct __mavlink_message>,
                                 struct Common_ParserConfiguration,
                                 Stream_IYaccStreamParser_T<struct Common_ParserConfiguration,
                                                            struct __mavlink_message>,
                                 enum Stream_SessionMessageType,
                                 struct ARDrone_UserData>
{
  typedef Stream_Module_Parser_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 struct Common_ScannerState,
                                 Stream_IYaccStreamParser_T<struct Common_ParserConfiguration,
                                                            struct __mavlink_message>,
                                 struct Common_ParserConfiguration,
                                 Stream_IYaccStreamParser_T<struct Common_ParserConfiguration,
                                                            struct __mavlink_message>,
                                 enum Stream_SessionMessageType,
                                 struct ARDrone_UserData> inherited;

 public:
  // convenient types
  typedef Stream_IYaccStreamParser_T<struct Common_ParserConfiguration,
                                     struct __mavlink_message> IPARSER_T;

  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_Module_MAVLinkDecoder_T (ISTREAM_T*); // stream handle
#else
  ARDrone_Module_MAVLinkDecoder_T (typename inherited::ISTREAM_T*); // stream handle
#endif
  inline virtual ~ARDrone_Module_MAVLinkDecoder_T () {};

  // implement/override (part of) Stream_IYaccStreamParser_T
  inline virtual void dump_state () const {};
  inline virtual void error (const yy::location& location_in, const std::string& string_in) { ACE_UNUSED_ARG (location_in); error (string_in); };
  inline virtual struct __mavlink_message& current () { ACE_ASSERT (inherited::fragment_); return const_cast<struct __mavlink_message&> (inherited::fragment_->getR ().getR ().MAVLinkData); };
  virtual void record (struct __mavlink_message*&); // record handle

  // implement/override (part of) Common_ILexScanner_T
//  inline virtual bool debug () const { return ARDrone_MAVLink_Scanner_get_debug (inherited::state_); };
  inline virtual void offset (unsigned int offset_in) { ARDrone_MAVLink_Scanner_set_column (offset_in, inherited::state_); };
  inline virtual unsigned int offset () const { return ARDrone_MAVLink_Scanner_get_column (inherited::state_); };
  virtual bool begin (const char*,   // buffer handle
                      unsigned int); // buffer size
  virtual void end ();
  virtual void waitBuffer ();
  virtual void error (const std::string&);
  inline virtual void debug (yyscan_t state_in, bool debug_in) { ACE_ASSERT (state_in); ARDrone_MAVLink_Scanner_set_debug ((debug_in ? 1 : 0), state_in); };
  inline virtual bool initialize (yyscan_t& state_inout, struct Common_ScannerState* state2_in) { ACE_ASSERT (state_inout); int result = ARDrone_MAVLink_Scanner_lex_init_extra (this, &state_inout); return (result == 0); };
  inline virtual void finalize (yyscan_t& state_inout) { ACE_ASSERT (state_inout); ARDrone_MAVLink_Scanner_lex_destroy (state_inout); state_inout = NULL; };
  inline virtual struct yy_buffer_state* create (yyscan_t state_in, char* buffer_in, size_t size_in) { ACE_ASSERT (state_in); ACE_ASSERT (buffer_in); ACE_ASSERT (size_in); return ARDrone_MAVLink_Scanner__scan_buffer (buffer_in, size_in, state_in); };
  inline virtual void destroy (yyscan_t state_in, struct yy_buffer_state*& buffer_inout) { ACE_ASSERT (state_in); ACE_ASSERT (buffer_inout); ARDrone_MAVLink_Scanner__delete_buffer (buffer_inout, state_in); buffer_inout = NULL; };
  inline virtual bool lex () { return (ARDrone_MAVLink_Scanner_lex (inherited::state_) == 0); };

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MAVLinkDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MAVLinkDecoder_T (const ARDrone_Module_MAVLinkDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MAVLinkDecoder_T& operator= (const ARDrone_Module_MAVLinkDecoder_T&))

  // override some ACE_Task_T methods
  virtual int svc (void);
};

// include template definition
#include "ardrone_module_mavlink_decoder.inl"

#endif
