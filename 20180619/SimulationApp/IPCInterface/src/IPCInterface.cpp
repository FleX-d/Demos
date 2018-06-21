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
#include <JsonObj.h>
#include <ctime>
#include <chrono>


namespace flexd{
    namespace bus{
        IPCInterface::IPCInterface(uint32_t ipcID, flexd::icl::ipc::FleXdEpoll& poller)
        :IPCConnector(ipcID, poller, true),
        m_counter(0){
            addPeer (00000);
        }

        void IPCInterface::sendDataToMCM(std::vector<uint8_t> data){       
            uint32_t peerID = 0;
            auto msg = std::make_shared<flexd::icl::ipc::FleXdIPCMsg>(std::move ( data ));
            msg->getAdditionalHeader()->setValue_1(0);
            msg->getAdditionalHeader()->setValue_4(getMyID());
            msg->getAdditionalHeader()->setValue_5(peerID);
            send(msg);
        }


        void IPCInterface::onConnectPeer(uint32_t peerID, bool genericPeer){

        }
        void IPCInterface::receiveMsg(flexd::icl::ipc::pSharedFleXdIPCMsg msg){

        }

        uint32_t IPCInterface::getTimestamp(){
            std::chrono::time_point<std::chrono::system_clock> p;
            p = std::chrono::system_clock::now();
            std::time_t time = std::chrono::duration_cast<std::chrono::milliseconds> ( p.time_since_epoch() ).count();
            return static_cast<uint32_t> ( time );
        }

        void IPCInterface::sendCreateClientMsg(const std::string& ID, const std::string& ExternID, 
                                               const std::string& Requester, const std::string& IPAddress,
                                               const std::string& Topic, uint8_t Direction,
                                               bool CleanSession, int Port,
                                               int QOS, int KeepAlive){
            uint8_t msgtype = 1;
            uint8_t msgCounter = m_counter;
            uint32_t timeStamp = getTimestamp();
            uint32_t from = getMyID();
            uint32_t to = 00000;
            int id = 1;

            flexd::icl::JsonObj json = {};

            json.add<int> ( "/id", id );
            json.add<std::string> ( "/payload/ID", ID );
            json.add<std::string> ( "/payload/ExternID", ExternID );
            json.add<std::string> ( "/payload/Requester", Requester );
            json.add<std::string> ( "/payload/IPAddress", IPAddress );
            json.add<std::string> ( "/payload/Topic", Topic );
            json.add<uint8_t> ( "/payload/Direction", Direction );
            json.add<bool> ( "/payload/CleanSession", CleanSession );
            json.add<int> ( "/payload/Port", Port );
            json.add<int> ( "/payload/QOS", QOS );
            json.add<int> ( "/payload/KeepAlive", KeepAlive );

            std::string tmp = json.getJson();
            std::vector<uint8_t> payload ( tmp.begin(), tmp.end() );

            auto msg = std::make_shared<flexd::icl::ipc::FleXdIPCMsg> ( std::move ( payload ),true );
            auto addHeader = msg->getAdditionalHeader();

            addHeader->setValue_0 ( msgtype );
            addHeader->setValue_1 ( msgCounter );
            addHeader->setValue_3 ( timeStamp );
            addHeader->setValue_4 ( from );
            addHeader->setValue_5 ( to );
            send ( msg );
        }
        
        void IPCInterface::sendPublishMsg(const std::string& ID, const std::string& Topic, const std::string& Requester, const std::string& PayloadMsg ){
            uint8_t msgtype = 1;
            uint8_t msgCounter = m_counter;
            uint32_t timeStamp = getTimestamp();
            uint32_t from = getMyID();
            uint32_t to = 00000;
            int id = 3;

            flexd::icl::JsonObj json = {};

            json.add<int> ( "/id", id );
            json.add<std::string> ( "/payload/ID", ID );
            json.add<std::string> ( "/payload/Topic", Topic );
            json.add<std::string> ( "/payload/Requester", Requester );
            json.add<std::string> ( "/payload/PayloadMsg", PayloadMsg );

            std::string tmp = json.getJson();
            std::vector<uint8_t> payload ( tmp.begin(), tmp.end() );

            auto msg = std::make_shared<flexd::icl::ipc::FleXdIPCMsg> ( std::move ( payload ),true );
            auto addHeader = msg->getAdditionalHeader();

            addHeader->setValue_0 ( msgtype );
            addHeader->setValue_1 ( msgCounter );
            addHeader->setValue_3 ( timeStamp );
            addHeader->setValue_4 ( from );
            addHeader->setValue_5 ( to );
            send ( msg );
        }
        
 /*       
        void IPCInterface::receiveMsg(flexd::icl::ipc::pSharedFleXdIPCMsg msg)
        {
            try{
                std::string str(msg->getPayload().begin(),msg->getPayload().end());
                flexd::icl::JsonObj json(str);
                if(json.exist("/id"))
                {
                    int id;
                    json.get<int>("/id", id);
                    switch(id)
                    {
                        case 4: {
                            std::string ID;
                            uint8_t RequestAck;

                            bool tmp = true;

                            if(json.exist("/payload/ID")){
                                json.get<std::string>("/payload/ID", ID);
                            } else {
                                tmp = false;}

                            if(json.exist("/payload/RequestAck")){
                                json.get<uint8_t>("/payload/RequestAck", RequestAck);
                            } else {
                                tmp = false;}


                            if(tmp){
                               receiveRequestAckMsg(ID, RequestAck);}
                            break; }

                        case 5: {
                            std::string PayloadMsg;

                            bool tmp = true;

                            if(json.exist("/payload/PayloadMsg")){
                                json.get<std::string>("/payload/PayloadMsg", PayloadMsg);
                            } else {
                                tmp = false;}


                            if(tmp){
                               receiveBackMsg(PayloadMsg);}
                            break; }

                        case 2: {
                            bool OperationAck;
                            std::string Message;
                            std::string AppID;

                            bool tmp = true;

                            if(json.exist("/payload/OperationAck")){
                                json.get<bool>("/payload/OperationAck", OperationAck);
                            } else {
                                tmp = false;}

                            if(json.exist("/payload/Message")){
                                json.get<std::string>("/payload/Message", Message);
                            } else {
                                tmp = false;}

                            if(json.exist("/payload/AppID")){
                                json.get<std::string>("/payload/AppID", AppID);
                            } else {
                                tmp = false;}


                            if(tmp){
                               receiveRequestCoreAckMsg(OperationAck, Message, AppID);}
                            break; }
                   }
                }
           }catch(...){
                return;
           }
        }
*/
        void IPCInterface::send ( std::shared_ptr<flexd::icl::ipc::FleXdIPCMsg> Msg )
        {
            if(sendMsg(Msg, 00000)){
            m_counter++;
            }
        }
    } //namespace bus
} //namespace flexd

