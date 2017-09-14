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

#include "common_iscanner.h"
#include "common_time_common.h"

#include "stream_task_base_synch.h"

#include "ardrone_common.h"
#include "ardrone_control_scanner.h"
#include "ardrone_types.h"

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
class ARDrone_Module_ControlDecoder_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct ARDrone_UserData>
 , public Stream_IYaccRecordParser_T<struct Common_ParserConfiguration,
                                     ARDrone_DeviceConfiguration_t>
 , public Common_ILexScanner_T<struct Common_ScannerState,
                               Stream_IYaccRecordParser_T<struct Common_ParserConfiguration,
                                                          ARDrone_DeviceConfiguration_t> >
{
  typedef Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct ARDrone_UserData> inherited;

 public:
  // convenient types
  typedef Stream_IYaccRecordParser_T<struct Common_ParserConfiguration,
                                     ARDrone_DeviceConfiguration_t> IPARSER_T;

  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_Module_ControlDecoder_T (ISTREAM_T*); // stream handle
#else
  ARDrone_Module_ControlDecoder_T (typename inherited::ISTREAM_T*); // stream handle
#endif
  virtual ~ARDrone_Module_ControlDecoder_T ();

  // override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement (part of) Stream_IYaccRecordParser_T
  inline virtual bool initialize (const struct Common_ParserConfiguration& configuration_in) { ACE_UNUSED_ARG (configuration_in); return true; };
  inline virtual void dump_state () const {};
  inline virtual bool parse (ACE_Message_Block*) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) };
  inline virtual void error (const yy::location& location_in, const std::string& string_in) { ACE_UNUSED_ARG (location_in); error (string_in); };
  inline virtual ARDrone_DeviceConfiguration_t& current () { return configuration_; };
  virtual void record (ARDrone_DeviceConfiguration_t*&); // record handle
  inline virtual bool hasFinished () const { return true; };

  // implement (part of) Common_ILexScanner_T
  inline virtual const Common_ScannerState& getR_2 () const { ACE_ASSERT (false); ACE_NOTSUP_RETURN (Common_ScannerState ()); ACE_NOTREACHED (return Common_ScannerState ();) };
  inline virtual const IPARSER_T* const getP_2 () const { return this; };
  inline virtual void setP (IPARSER_T*) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual ACE_Message_Block* buffer () { return buffer_; };
//  inline virtual bool debug () const { return ARDrone_Control_Scanner_get_debug (scannerState_); };
  inline virtual bool isBlocking () const { return true; };
  inline virtual void offset (unsigned int offset_in) { ARDrone_Control_Scanner_set_column (offset_in, scannerState_); };
  inline virtual unsigned int offset () const { return ARDrone_Control_Scanner_get_column (scannerState_); };
  virtual bool begin (const char*,   // buffer handle
                      unsigned int); // buffer size
  virtual void end ();
  virtual bool switchBuffer (bool = false); // unlink current buffer ?
  virtual void waitBuffer ();
  virtual void error (const std::string&);
  inline virtual void debug (yyscan_t, bool) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual bool initialize (yyscan_t& state_in, struct Common_ScannerState* state2_in) { return (ARDrone_Control_Scanner_lex_init_extra (this, &state_in) == 0); };
  inline virtual void finalize (yyscan_t&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual struct yy_buffer_state* create (yyscan_t, char*, size_t) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (NULL); ACE_NOTREACHED (return NULL;) };
  inline virtual void destroy (yyscan_t, struct yy_buffer_state*&) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };
  inline virtual bool lex () { return (ARDrone_Control_Scanner_lex (scannerState_) == 0); };

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_ControlDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_ControlDecoder_T (const ARDrone_Module_ControlDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_ControlDecoder_T& operator= (const ARDrone_Module_ControlDecoder_T&))

//  using typename inherited::TASK_BASE_T::get;

  // override some ACE_Task_T methods
  int svc (void);

  DataMessageType*              buffer_;
  struct yy_buffer_state*       bufferState_;
  ARDrone_DeviceConfiguration_t configuration_;
  bool                          isFirst_;
  yyscan_t                      scannerState_;
  bool                          useYYScanBuffer_;
};

// include template definition
#include "ardrone_module_control_decoder.inl"

#endif
