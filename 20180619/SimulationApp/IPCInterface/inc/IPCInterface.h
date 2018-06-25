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


#define MCM_JSON_MSG_TYPE "/msgType"
#define MCM_JSON_MSG_COUNTER "/msgCounter"
#define MCM_JSON_PAYLOAD_CRC "/payloadCRC"
#define MCM_JSON_TIME_STAMP "/timeStamp"
#define MCM_JSON_FROM "/from"
#define MCM_JSON_TO "/to"
#define MCM_JSON_PAYLOAD "/payload"

namespace flexd{
    namespace bus{

        class IPCInterface : public flexd::icl::ipc::IPCConnector{
        public:
            IPCInterface(uint32_t ipdID, flexd::icl::ipc::FleXdEpoll& poller);
            virtual ~IPCInterface() = default;
            /**
             * Override function from base class IPCConnector, that returned received message as a parameter of function.
             * @param msg - received message from IPC
             */
            void receiveMsg ( flexd::icl::ipc::pSharedFleXdIPCMsg msg ) override;
            /**
             * Override function from base class IPCConnector, that is called after peer connecting 
             * @param peerID - identifier of connected peer
             * @param genericPeer - generic interface setting (true - generic interface enable)
             */
            void onConnectPeer ( uint32_t peerID, bool genericPeer ) override;
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
            void sendCreateClientMsg ( uint32_t ID, const std::string& ExternID, const std::string& Requester, const std::string& IPAddress, const std::string& Topic, uint8_t Direction, bool CleanSession, int Port, int QOS, int KeepAlive );
             /**
             * Function for publishing message ti the MCM
             * @param ID - identifier of connected peer
             * @param Topic - 
             * @param Requester -
             * @param PayloadMsg -
             */
            void sendPublishMsg ( uint32_t ID, const std::string& Topic, const std::string& Requester, const std::string& PayloadMsg );
            /**
             * Function implement the send  function from base class. 
             * @param msg - sent message to the IPC
             */
            void send ( std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> msg );
            /**
             * Pure virtual function for timer, the override function will be called after the timer overflows
             */
            virtual void onTimer() = 0;
        private:
            uint32_t getTimestamp();
            std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> msgWrap(const std::string& payload);
            std::string msgUnwrap(const std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg>& msg);
        private:
            uint8_t m_counter;
        };

    } //namespace bus
} //namespace flexd

#endif // IPCINTERFACE_H 
