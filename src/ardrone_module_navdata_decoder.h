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

#ifndef ARDRONE_MODULE_NAVDATA_DECODER_H
#define ARDRONE_MODULE_NAVDATA_DECODER_H

#include "ace/Global_Macros.h"

#include "common_ilock.h"
#include "common_iscanner.h"
#include "common_time_common.h"

#include "common_parser_common.h"

#include "stream_misc_parser.h"

#include "ardrone_common.h"
#include "ardrone_types.h"
#include "ardrone_navdata_scanner.h"

// forward declaration(s)
class ACE_Message_Block;
class Stream_IAllocator;

extern const char ardrone_default_navdata_decoder_module_name_string[];

class ARDrone_NavData_IParser
 : public Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     struct _navdata_t>
 , virtual public Common_ILexScanner_T<struct Common_FlexScannerState,
                                       void>
{
 public:
  // convenient types
  typedef Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     struct _navdata_t> IPARSER_T;
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

  virtual void addOption (unsigned int) = 0; // offset

  ARDrone_NavData_IParser ()
   : finished_ (false)
   , state_ (NULL)
  {}

  bool     finished_;
  yyscan_t state_;
};

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
class ARDrone_Module_NavDataDecoder_T
 : public Stream_Module_Parser_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 ARDrone_NavData_IParser,
                                 struct Stream_UserData>
{
  typedef Stream_Module_Parser_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 ARDrone_NavData_IParser,
                                 struct Stream_UserData> inherited;

 public:
//  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ARDrone_Module_NavDataDecoder_T (ISTREAM_T*); // stream handle
//#else
  ARDrone_Module_NavDataDecoder_T (typename inherited::ISTREAM_T*); // stream handle
//#endif
  virtual ~ARDrone_Module_NavDataDecoder_T ();

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) ARDrone_NavData_IParser
  inline virtual bool initialize (const struct Common_FlexBisonParserConfiguration& configuration_in) { ACE_UNUSED_ARG (configuration_in); return true; }
  inline virtual void dump_state () const {}
  inline virtual bool parse (ACE_Message_Block*) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
  inline virtual void error (const struct YYLTYPE& location_in, const std::string& string_in) { ACE_UNUSED_ARG (location_in); error (string_in); }
  inline virtual void error (const yy::location& location_in, const std::string& string_in) { ACE_UNUSED_ARG (location_in); error (string_in); }
  inline virtual struct _navdata_t& current () { ACE_ASSERT (buffer_); return const_cast<struct _navdata_t&> (buffer_->getR ().getR ().NavData.NavData); }
  virtual void record (struct _navdata_t*&); // record handle
  inline virtual bool hasFinished () const { return false; }
  inline virtual void addOption (unsigned int offset_in) { ACE_ASSERT (buffer_); const_cast<typename DataMessageType::DATA_T::DATA_T&> (buffer_->getR ().getR ()).NavData.NavDataOptionOffsets.push_back (offset_in); }

  // implement (part of) Common_ILexScanner_T
  //inline virtual const Common_ScannerState& getR_3 () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (Common_ScannerState ()); ACE_NOTREACHED (return Common_ScannerState ();) }
  //inline virtual const ARDrone_NavData_IParser* const getP_2 () const { return this; }
  //inline virtual void setP (ARDrone_NavData_IParser*) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual ACE_Message_Block* buffer () { return buffer_; }
  inline virtual bool debug () const { return ARDrone_NavData_Scanner_get_debug (inherited::state_); }
  inline virtual bool isBlocking () const { return true; }
  inline virtual unsigned int offset () const { return ARDrone_NavData_Scanner_get_column (inherited::state_); }
  virtual bool begin (const char*,   // buffer handle
                      unsigned int); // buffer size
  virtual void end ();
  virtual bool switchBuffer (bool = false); // unlink current buffer ?
  virtual void waitBuffer ();
  inline virtual void offset (unsigned int offset_in) { ARDrone_NavData_Scanner_set_column (offset_in, inherited::state_); }
  virtual void error (const std::string&);
  inline virtual void setDebug (yyscan_t, bool) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void reset() { ARDrone_NavData_Scanner_set_column (1, inherited::state_); ARDrone_NavData_Scanner_set_lineno (1, inherited::state_); }
  inline virtual bool initialize (yyscan_t& state_in, void*) { return (ARDrone_NavData_Scanner_lex_init_extra (this, &state_in) == 0); }
  inline virtual void finalize (yyscan_t&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual struct yy_buffer_state* create (yyscan_t, char*, size_t) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (NULL); ACE_NOTREACHED (return NULL;) }
  inline virtual void destroy (yyscan_t, struct yy_buffer_state*&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual bool lex (yyscan_t state_in) { return (ARDrone_NavData_Scanner_lex (state_in) == 0); }

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_NavDataDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_NavDataDecoder_T (const ARDrone_Module_NavDataDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_NavDataDecoder_T& operator= (const ARDrone_Module_NavDataDecoder_T&))

  // override some ACE_Task_T methods
  virtual int svc (void);

  DataMessageType*        buffer_;
  struct yy_buffer_state* bufferState_;
  bool                    isFirst_;
  unsigned int            numberOfOptions_; // current-
  //std::string             scannerTables_;
};

// include template definition
#include "ardrone_module_navdata_decoder.inl"

#endif
