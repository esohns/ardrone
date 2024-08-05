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

#ifndef ARDRONE_MODULE_CONTROL_DECODER_H
#define ARDRONE_MODULE_CONTROL_DECODER_H

#include "ace/Global_Macros.h"

#include "common.h"
#include "common_time_common.h"

#include "stream_misc_parser.h"

#include "ardrone_common.h"
#include "ardrone_control_scanner.h"
#include "ardrone_types.h"

// forward declaration(s)
class ACE_Message_Block;
class Stream_IAllocator;

extern const char ardrone_default_control_decoder_module_name_string[];

class ARDrone_Control_IParser
 : public Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     ARDrone_DeviceConfiguration_t>
 , virtual public Common_ILexScanner_T<struct Common_FlexScannerState,
                                       void>
{
 public:
  // convenient types
  typedef Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     ARDrone_DeviceConfiguration_t> IPARSER_T;
  typedef Common_ILexScanner_T<struct Common_FlexScannerState,
                               void> ISCANNER_T;

  using IPARSER_T::initialize;
  //using IPARSER_T::error;

  virtual bool initialize (const struct Common_FlexBisonParserConfiguration& configuration_in)
  {
    ACE_UNUSED_ARG (configuration_in);

    ACE_ASSERT (!state_);
    bool result = false;
    ISCANNER_T* scanner_p = this;
    try {
      result = scanner_p->initialize (state_,
                                      this);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Common_ILexScanner_T::initialize(): \"%m\", continuing\n")));
    }
    if (!result)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_ILexScanner_T::initialize(): \"%m\", aborting\n")));
      return false;
    } // end IF
    ACE_ASSERT (state_);

    return true;
  }
  inline virtual bool parse (ACE_Message_Block*) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
  inline virtual const struct Common_FlexScannerState& getR () const { ACE_ASSERT (false); static struct Common_FlexScannerState dummy; return dummy; }
  //  using Common_IScanner::error;

  ARDrone_Control_IParser ()
   : finished_ (false)
   , state_ (NULL)
  {}

  bool     finished_;
  yyscan_t state_;
};

////////////////////////////////////////////

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
class ARDrone_Module_ControlDecoder_T
 : public Stream_Module_Parser_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 ARDrone_Control_IParser,
                                 struct Stream_UserData>
{
  typedef Stream_Module_Parser_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 ARDrone_Control_IParser,
                                 struct Stream_UserData> inherited;

 public:
  // convenient types
  typedef Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     ARDrone_DeviceConfiguration_t> IPARSER_T;

//  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ARDrone_Module_ControlDecoder_T (ISTREAM_T*); // stream handle
//#else
  ARDrone_Module_ControlDecoder_T (typename inherited::ISTREAM_T*); // stream handle
//#endif
  inline virtual ~ARDrone_Module_ControlDecoder_T () {}

  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) ARDrone_Control_IParser
  inline virtual void dump_state () const {}
  inline virtual void error (const struct YYLTYPE& location_in, const std::string& string_in) { ACE_UNUSED_ARG (location_in); error (string_in); }
  inline virtual void error (const yy::location& location_in, const std::string& string_in) { ACE_UNUSED_ARG (location_in); error (string_in); }
  inline virtual ARDrone_DeviceConfiguration_t& current () { return configuration_; }
  virtual void record (ARDrone_DeviceConfiguration_t*&); // record handle
  inline virtual bool hasFinished () const { ACE_ASSERT (buffer_); return buffer_->total_length () == 0; }

  // implement (part of) Common_ILexScanner_T
//  inline virtual const Common_ScannerState& getR_3 () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (Common_ScannerState ()); ACE_NOTREACHED (return Common_ScannerState ();) }
//  inline virtual const IPARSER_T* const getP_2 () const { return this; }
//  inline virtual void setP (IPARSER_T*) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual ACE_Message_Block* buffer () { ACE_ASSERT (false); ACE_NOTSUP_RETURN (NULL); ACE_NOTREACHED (return NULL;) }
  inline virtual bool debug () const { return ARDrone_Control_Scanner_get_debug (inherited::state_); }
  inline virtual bool isBlocking () const { return true; }
  inline virtual unsigned int offset () const { return ARDrone_Control_Scanner_get_column (inherited::state_); }
  virtual bool begin (const char*,   // buffer handle
                      unsigned int); // buffer size
  virtual void end ();
  virtual bool switchBuffer (bool = false);
  virtual void waitBuffer ();
  inline virtual void offset (unsigned int offset_in) { offset_ += offset_in; }
  virtual void error (const std::string&);
  inline virtual void setDebug (yyscan_t state_in, bool debug_in) { ACE_ASSERT (state_in); ARDrone_Control_Scanner_set_debug ((debug_in ? 1 : 0), state_in); }
  inline virtual void reset () { ARDrone_Control_Scanner_set_lineno (1, inherited::state_); ARDrone_Control_Scanner_set_column (1, inherited::state_); }
  inline virtual bool initialize (yyscan_t& state_in, void* = NULL) { return (ARDrone_Control_Scanner_lex_init_extra (this, &state_in) == 0); }
  inline virtual void finalize (yyscan_t&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual struct yy_buffer_state* create (yyscan_t, char*, size_t) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (NULL); ACE_NOTREACHED (return NULL;) }
  inline virtual void destroy (yyscan_t, struct yy_buffer_state*&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual bool lex (yyscan_t state_in) { return (ARDrone_Control_Scanner_lex (state_in) == 0); }

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_ControlDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_ControlDecoder_T (const ARDrone_Module_ControlDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_ControlDecoder_T& operator= (const ARDrone_Module_ControlDecoder_T&))

  // override some ACE_Task_T methods
  int svc (void);

  DataMessageType*              buffer_; // current head
  YY_BUFFER_STATE               bufferState_;
  ARDrone_DeviceConfiguration_t configuration_;
  ACE_Message_Block*            fragment_; // current-
  bool                          isFirst_;
  unsigned int                  offset_;
  ARDrone_IDeviceConfiguration* subscriber_;
};

// include template definition
#include "ardrone_module_control_decoder.inl"

#endif
