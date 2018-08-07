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

#include "IPCInterface.h"
#include <FleXdIPCMsgTypes.h>
#include <JsonObj.h>
#include <ctime>
#include <chrono>
 
namespace flexd{
    namespace gen{
        IPCInterface::IPCInterface(uint32_t ipcID, flexd::icl::ipc::FleXdEpoll& poller)
        :IPCConnector(ipcID, poller, true),
         GenericClient()
        {
            addPeer (00000);
        }
        
        void IPCInterface::sendCreateClientMsg(std::shared_ptr<GenericClient::Header> header,
                                               uint32_t ID, const std::string& ExternID, 
                                               const std::string& Requester, const std::string& IPAddress,
                                               const std::string& Topic, uint8_t Direction,
                                               bool CleanSession, int Port,
                                               int QOS, int KeepAlive){
            
            int id = 1;
            flexd::icl::JsonObj json;

            json.add<int> ( "/id", id );
            json.add<uint32_t> ( "/payload/ID", ID );
            json.add<std::string> ( "/payload/ExternID", ExternID );
            json.add<std::string> ( "/payload/Requester", Requester );
            json.add<std::string> ( "/payload/IPAddress", IPAddress );
            json.add<std::string> ( "/payload/Topic", Topic );
            json.add<uint8_t> ( "/payload/Direction", Direction );
            json.add<bool> ( "/payload/CleanSession", CleanSession );
            json.add<int> ( "/payload/Port", Port );
            json.add<int> ( "/payload/QOS", QOS );
            json.add<int> ( "/payload/KeepAlive", KeepAlive );
            send(msgWrap(header, json.getJson()), header->to);
        }
        
        void IPCInterface::sendOperationMsg(std::shared_ptr<GenericClient::Header> header, uint32_t ID, const std::string& Requester, uint8_t Operation){
            int id = 2;
           
            flexd::icl::JsonObj json;
	    
            json.add<int>("/id", id);
            json.add<uint32_t>("/payload/ID", ID);
            json.add<std::string>("/payload/Requester", Requester);
            json.add<uint8_t>("/payload/Operation", Operation);
            send(msgWrap(header, json.getJson()), header->to);
        }
        
        void IPCInterface::sendPublishMsg(std::shared_ptr<GenericClient::Header> header, uint32_t ID, const std::string& Topic, const std::string& Requester, const std::string& PayloadMsg)
        {
       
            int id = 3;
           
            flexd::icl::JsonObj json = {};
                
            json.add<int>("/id", id);
            json.add<uint32_t>("/payload/ID", ID);
            json.add<std::string>("/payload/Topic", Topic);
            json.add<std::string>("/payload/Requester", Requester);
            json.add<std::string>("/payload/PayloadMsg", PayloadMsg);
            send(msgWrap(header, json.getJson()), header->to);
        }
        
        
        void IPCInterface::receiveMsg(flexd::icl::ipc::pSharedFleXdIPCMsg msg)
        {
            try{
                if(!msg) {
                    return;
                }
                
                flexd::icl::JsonObj json(msgUnwrap(msg));
                if(json.exist("/id"))
                {
                    int id; 
                    json.get<int>("/id", id);
                    switch(id)
                    {
                        case 4: {
                            uint32_t ID;
                            uint8_t RequestAck;
                            
                            bool tmp = true;
			    
                            if(json.exist("/payload/ID")){
                                json.get<uint32_t>("/payload/ID", ID); 
                            } else {
                                tmp = false;
                            }
                            
                            if(json.exist("/payload/RequestAck")){
                                json.get<uint8_t>("/payload/RequestAck", RequestAck); 
                            } else {
                                tmp = false;
                            }
                            if(tmp){
                                receiveRequestAckMsg(m_header, ID, RequestAck);}
                            break; }
		     
                        case 5: {
                            uint32_t ID;
                            std::string PayloadMsg;
                                        
                            bool tmp = true;
                            
                            if(json.exist("/payload/ID")){
                                json.get<uint32_t>("/payload/ID", ID); 
                            } else {
                                tmp = false;}
                            
                            if(json.exist("/payload/PayloadMsg")){
                                json.get<std::string>("/payload/PayloadMsg", PayloadMsg); 
                            } else {
                                tmp = false;}
                            
                                        
                            if(tmp){
                                receiveBackMsg(m_header, ID, PayloadMsg);}
                            break; 
                        }
                    }
                }
            }catch(...){
                return;
            }
        }
       
        void IPCInterface::send(std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> Msg, uint32_t peerID) {
            if(sendMsg(Msg, peerID)) {
                m_counter++;
            }
        }
    } //namespace bus
} //namespace flexd

