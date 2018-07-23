
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
 * Author: Matus Bodorik
 *
 * Created on Jul 04, 2018, 11:00 AM
 */

#ifndef GENERICCLIENT_H
#define GENERICCLIENT_H

#include <string>
#include "FleXdIPCMsg.h"

#define MCM_JSON_MSG_TYPE "/msgType"
#define MCM_JSON_MSG_COUNTER "/msgCounter"
#define MCM_JSON_PAYLOAD_CRC "/payloadCRC"
#define MCM_JSON_TIME_STAMP "/timeStamp"
#define MCM_JSON_FROM "/from"
#define MCM_JSON_TO "/to"
#define MCM_JSON_PAYLOAD "/payload"

namespace flexd {
    namespace gen {

        class GenericClient{
        public:
            struct Header {
                uint8_t msgType;
                uint8_t counter;
                uint16_t crc; 
                uint32_t timeStamp; 
                uint32_t from; 
                uint32_t to; 
            };
        public:
            GenericClient ();
            virtual ~GenericClient();
           
            std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> msgWrap(std::shared_ptr<GenericClient::Header> header, const std::string& payload);
            std::string msgUnwrap(const std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg>& msg);
            uint32_t getTimestamp();

        public:
            std::shared_ptr<GenericClient::Header> m_header;
            uint8_t m_counter;
        };

    }
}
#endif //GENERICCLIENT_H
