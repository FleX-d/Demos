
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

#include "GenericClient.h"
#include "JsonObj.h"
#include "FleXdIPCMsg.h"
#include "FleXdIPCMsgTypes.h"
#include <vector>
#include <ctime>
#include <chrono>


namespace flexd {
    namespace gen {
        


        GenericClient::GenericClient()
        : m_header(std::make_shared<Header>()), 
          m_counter(0)
        {
        }

        GenericClient::~GenericClient() {
        }
        
        std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> GenericClient::msgWrap(std::shared_ptr<GenericClient::Header> header, const std::string& payload) {
            flexd::icl::JsonObj json;

            json.add<uint8_t>(MCM_JSON_MSG_TYPE, 2);
            json.add<uint8_t>(MCM_JSON_MSG_COUNTER, m_counter);
            json.add<uint16_t>(MCM_JSON_PAYLOAD_CRC, flexd::icl::ipc::FleXdIPCMsg::calcCRC16(&payload[0] , payload.size()));
            json.add<uint32_t>(MCM_JSON_TIME_STAMP, getTimestamp());
            json.add<uint32_t>(MCM_JSON_FROM, header->from);
            json.add<uint32_t>(MCM_JSON_TO, header->to);
            json.add<std::string>(MCM_JSON_PAYLOAD, payload);
            std::string strJson(json.getJson());
            std::vector<uint8_t> msgPayload(strJson.begin(), strJson.end());
            return std::make_shared<flexd::icl::ipc::FleXdIPCMsg>(IPC_MSG_HEADER_PARAM_TYPE_MSG_TYPE, IPC_MSG_HEADER_IN_PAYLOAD_FLAG, std::move(msgPayload));
        }

        std::string GenericClient::msgUnwrap(const std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg>& msg) {
            //TODO returns only payload, should return whole message
            if(msg->getHeaderParamType() == IPC_MSG_HEADER_PARAM_TYPE_MSG_TYPE && msg->getHeaderParam() == IPC_MSG_HEADER_IN_PAYLOAD_FLAG) {
                std::string strJson(msg->getPayload().begin(), msg->getPayload().end());
                flexd::icl::JsonObj json(strJson);
                bool tmp = true;
                std::string payload;
                
                if(json.exist(MCM_JSON_MSG_TYPE)){
                    json.get<uint8_t>(MCM_JSON_MSG_TYPE, m_header->msgType);
                } else {
                    tmp = false;
                }
                    
                if(json.exist(MCM_JSON_MSG_COUNTER)){
                    json.get<uint8_t>(MCM_JSON_MSG_COUNTER, m_header->counter);
                } else {
                    tmp = false;
                }
                
                if(json.exist(MCM_JSON_PAYLOAD_CRC)){
                    json.get<uint16_t>(MCM_JSON_PAYLOAD_CRC, m_header->crc);
                } else {
                    tmp = false;
                }
                
                if(json.exist(MCM_JSON_TIME_STAMP)){
                    json.get<uint32_t>(MCM_JSON_TIME_STAMP, m_header->timeStamp);
                } else {
                    tmp = false;
                }
                
                if(json.exist(MCM_JSON_FROM)){
                    json.get<uint32_t>(MCM_JSON_FROM, m_header->from);
                } else {
                    tmp = false;
                }
                
                if(json.exist(MCM_JSON_TO)){
                    json.get<uint32_t>(MCM_JSON_TO, m_header->to);
                } else {
                    tmp = false;
                }
                

                if(tmp && json.exist(MCM_JSON_PAYLOAD)) {
                    json.get<std::string>(MCM_JSON_PAYLOAD, payload);
                }
                return payload;
            }
            return std::string(msg->getPayload().begin(), msg->getPayload().end());
        }
        
        
        uint32_t GenericClient::getTimestamp() {
            std::chrono::time_point<std::chrono::system_clock> p;
            p = std::chrono::system_clock::now();
            std::time_t time = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch()).count();
            return static_cast<uint32_t> (time);
        }
    }
}
