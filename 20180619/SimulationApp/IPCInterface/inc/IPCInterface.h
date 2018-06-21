/*
 * Copyright (c) 2018, Globallogic s.r.o.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Globallogic s.r.o. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC S.R.O. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * File:   IPCInterface.cpp
 * Author: Jakub Pekar
 */
#ifndef IPCINTERFACE_H
#define IPCINTERFACE_H

#include <FleXdEpoll.h>
#include <FleXdIPCMsg.h>
#include "FleXdIPCConnector.h"

namespace flexd{
    namespace bus{

        class IPCInterface : public flexd::icl::ipc::IPCConnector{
        public:
            IPCInterface(uint32_t ipdID, flexd::icl::ipc::FleXdEpoll& poller);
            virtual ~IPCInterface() = default;
    
            void sendDataToMCM(std::vector<uint8_t> data);
    
            void receiveMsg ( flexd::icl::ipc::pSharedFleXdIPCMsg msg ) override;
            void onConnectPeer ( uint32_t peerID, bool genericPeer ) override;
            virtual void onTimer() = 0;

            void sendCreateClientMsg ( const std::string& ID, const std::string& ExternID, const std::string& Requester, const std::string& IPAddress, const std::string& Topic, uint8_t Direction, bool CleanSession, int Port, int QOS, int KeepAlive );
            void sendPublishMsg ( const std::string& ID, const std::string& Topic, const std::string& Requester, const std::string& PayloadMsg );
            void send ( std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> Msg );
        private:
            uint32_t getTimestamp();
        private:
            uint8_t m_counter;
        };

    } //namespace bus
} //namespace flexd

#endif // IPCINTERFACE_H 
