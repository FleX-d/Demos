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
#include "GenericClient.h"
#include "JsonObj.h"
#include <memory>
#include <cstdint>

namespace flexd{
    namespace gen{

        class IPCInterface : public flexd::icl::ipc::IPCConnector, public flexd::gen::GenericClient {
        public:
            IPCInterface(uint32_t ipdID, flexd::icl::ipc::FleXdEpoll& poller);
            virtual ~IPCInterface() = default;
            /**
             * Override function from base class IPCConnector, that is called after peer connecting 
             * @param peerID - identifier of connected peer
             * @param genericPeer - generic interface setting (true - generic interface enable)
             */
            void onConnectPeer ( uint32_t peerID, bool genericPeer ) = 0;
             /**
             * Function for initialization the client on the Mosquito Communication Manager (PeerID = 00000)
             * @param ID - identifier of connected peer
             * @param ExternID -
             * @param Requester -
             * @param IPAddress -
             * @param Topic - 
             * @param Direction - 
             * @param CleanSession - 
             * @param Port - 
             * @param QOS - 
             * @param KeepAlive - 
             */
            void sendCreateClientMsg ( std::shared_ptr<GenericClient::Header> header, uint32_t ID, const std::string& ExternID, const std::string& Requester, const std::string& IPAddress, const std::string& Topic, uint8_t Direction, bool CleanSession, int Port, int QOS, int KeepAlive );
             /**
             * Function for publishing message ti the MCM
             * @param ID - identifier of connected peer
             * @param Topic - 
             * @param Requester -
             * @param PayloadMsg -
             */
            void sendPublishMsg (std::shared_ptr<GenericClient::Header> header, uint32_t ID, const std::string& Topic, const std::string& Requester, const std::string& PayloadMsg );
            
            
            /**
             * Pure virtual function for timer, the override function will be called after the timer overflows
             */
            virtual void onTimer() = 0;
            void sendOperationMsg(std::shared_ptr<GenericClient::Header> header,uint32_t ID, const std::string& Requester, uint8_t Operation);
        private:
            void send ( std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> msg, uint32_t peerID );
            
            virtual void receiveMsg(flexd::icl::ipc::pSharedFleXdIPCMsg Msg) override;
            virtual void receiveRequestAckMsg(std::shared_ptr<GenericClient::Header> header, uint32_t ID, uint8_t RequestAck) = 0;
            virtual void receiveBackMsg(std::shared_ptr<GenericClient::Header> header, uint32_t ID, const std::string& PayloadMsg) = 0;
        };

    } //namespace bus
} //namespace flexd

#endif // IPCINTERFACE_H 
