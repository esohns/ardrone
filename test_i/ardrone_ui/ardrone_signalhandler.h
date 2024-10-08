﻿/***************************************************************************
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

#ifndef ARDRONE_SIGNALHANDLER_H
#define ARDRONE_SIGNALHANDLER_H

#include "ace/Global_Macros.h"

#include "common_signal_handler.h"

#include "ardrone_network.h"
#include "ardrone_types.h"

template <typename ConfigurationType,
          typename TCPConnectorType,
          typename UDPConnectorType> // inherits Net_IConnector_T
class ARDrone_SignalHandler_T
 : public Common_SignalHandler_T<ConfigurationType>
{
  typedef Common_SignalHandler_T<ConfigurationType> inherited;

 public:
  ARDrone_SignalHandler_T ();
  inline virtual ~ARDrone_SignalHandler_T () {}

  // override Common_IInitialize_T
  virtual bool initialize (const ConfigurationType&);

  // implement Common_ISignal
  virtual void handle (const struct Common_Signal&);

 private:
  //ACE_UNIMPLEMENTED_FUNC (ARDrone_SignalHandler_T ());
  ACE_UNIMPLEMENTED_FUNC (ARDrone_SignalHandler_T (const ARDrone_SignalHandler_T&));
  ACE_UNIMPLEMENTED_FUNC (ARDrone_SignalHandler_T& operator= (const ARDrone_SignalHandler_T&));
};

// include template definition
#include "ardrone_signalhandler.inl"

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ARDrone_SignalHandler_T<struct ARDrone_DirectShow_SignalConfiguration,
                                ARDrone_DirectShow_ITCPConnector_t,
                                ARDrone_DirectShow_IUDPConnector_t> ARDrone_DirectShow_SignalHandler_t;
typedef ARDrone_SignalHandler_T<struct ARDrone_MediaFoundation_SignalConfiguration,
                                ARDrone_MediaFoundation_ITCPConnector_t,
                                ARDrone_MediaFoundation_IUDPConnector_t> ARDrone_MediaFoundation_SignalHandler_t;
#else
typedef ARDrone_SignalHandler_T<struct ARDrone_SignalConfiguration,
                                ARDrone_ITCPConnector_t,
                                ARDrone_IUDPConnector_t> ARDrone_SignalHandler_t;
#endif // ACE_WIN32 || ACE_WIN64

#endif
