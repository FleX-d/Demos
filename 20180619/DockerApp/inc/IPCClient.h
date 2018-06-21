
/*
Copyright (c) 2017, Globallogic s.r.o.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
 * Neither the name of the Globallogic s.r.o. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC S.R.O. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * File:   IPCClient.h
 * Author: Martin Strenger
 *
 * Created on June 21, 2018, 8:56 AM
 */

#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#include "IPCInterface.h"
#include <FleXdEpoll.h>
#include <string>

namespace flexd {
    namespace docker {

        class IPCClient : public flexd::gen::IPCInterface {
        public :
            IPCClient(flexd::icl::ipc::FleXdEpoll& poller);
            virtual ~IPCClient();

            void init();
            void createClient();
            void sendSubScribe();
            bool writeToFile(std::string AppID);
            virtual void receiveBackMsg(const std::string& PayloadMsg) override;
            virtual void receiveRequestAckMsg(uint32_t ID, uint8_t RequestAck) override;
            virtual void receiveRequestCoreAckMsg(bool OperationAck, const std::string& Message, const std::string& AppID) override;
            virtual void onConnectPeer(uint32_t peerID, bool genericPeer) override;

            IPCClient(const IPCClient& orig) = delete;
            IPCClient& operator= (const IPCClient& orig) = delete;
            IPCClient(IPCClient&& orig) = delete;
            IPCClient& operator= (IPCClient&& orig) = delete;

        private:
            int m_counter;
            std::string m_ipAddress;
            std::string m_topic;
            int m_direction;
            bool m_cleanSession;
            int m_port;
            int m_qos;
            int m_keepAlive;
            std::string m_path;
            std::string m_buffer;
        };

    }
}
#endif //IPCCLIENT_H
