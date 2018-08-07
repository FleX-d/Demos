/*
 * Copyright (c) 2018, Globallogic s.r.o.
 * All rights reserved.
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
 * File:   monitor.cpp
 * Author: Jakub Pekar
 */
 
 
 #include "FleXdLogger.h"
 #include "IPCInterface.h"
 #include "GenericClient.h"
 #include "FleXdEpoll.h"
 #include <JsonObj.h>
 //#include "FleXdTimer.h"
 #include <unistd.h>
 #include <iostream>
 
 namespace flexd {
     namespace bus {
        class Monitor: public flexd::gen::IPCInterface{
        public:
            int m_counter = 0;
            
            Monitor(flexd::icl::ipc::FleXdEpoll& poller)
            :IPCInterface(00110, poller)
            {   
                
                FLEX_LOG_INIT(poller,"Monitor");
                
                FLEX_LOG_INFO(" -> Monitor initialization!");
                std::cout <<"Terminal:"<< std::endl;
            }
            
            ~Monitor(){};
            
            void createClient(){
                //FLEX_LOG_DEBUG("Send Request for Create Client");
                std::shared_ptr<GenericClient::Header> header = std::make_shared<GenericClient::Header>();
                header->from = getMyID();
                header->to = 00000;
                sendCreateClientMsg(std::move(header), getMyID(), "Monitor", "Monitor", "127.0.0.1", "backend/in", 2, true, 1883, 0, 60);
            }
            
            void sendSubscribe()
            {
                //FLEX_LOG_DEBUG("Send Subscribe");
                std::shared_ptr<GenericClient::Header> header = std::make_shared<GenericClient::Header>();
                header->from = getMyID();
                header->to = 00000;
                sendOperationMsg(std::move(header), getMyID(), "Monitor", 0);
            }
            
            void receiveRequestAckMsg(std::shared_ptr<flexd::gen::GenericClient::Header> header, uint32_t ID, uint8_t RequestAck) override{
                std::cout << "Receive Ack: " << ID << " -> " << (int)RequestAck << std::endl;
                std::shared_ptr<GenericClient::Header> sendheader = std::make_shared<GenericClient::Header>();
                sendheader->from = getMyID();
                sendheader->to = 00000;
                if(m_counter == 0 && RequestAck == 1) {
                    sleep(2);
                  //  FLEX_LOG_DEBUG("Create CLient Success: ", ID);
                    sendSubscribe();
                    m_counter++;
                } else if (m_counter == 0 && RequestAck == 0) {
                    sleep(5);
                    FLEX_LOG_DEBUG("Create CLient Fail: ", ID);
                    createClient();
                } else if (m_counter == 0 && RequestAck == 2) {
                    FLEX_LOG_DEBUG("Create CLient Fail, because is exists: ", ID);
                    sendSubscribe();
                    m_counter++;
                } else if(m_counter == 1 && RequestAck == 1) {
                    FLEX_LOG_DEBUG("Client Subscribe Success: ", ID);
                    m_counter++;
                }else if(m_counter == 1 && RequestAck == 0) {
                    sleep(5);
                    FLEX_LOG_DEBUG("Client Subscribe Fail: ", ID);
                    sendSubscribe();
                } else {
                   FLEX_LOG_DEBUG("Publish MSG to backend Success: ", ID);
                }
            }
            
            
            void receiveBackMsg(std::shared_ptr<flexd::gen::GenericClient::Header> header, uint32_t ID, const std::string& PayloadMsg) override{
                std::cout << ID << " -> " << PayloadMsg << std::endl;
            }
            
            void onConnectPeer(uint32_t peerID, bool genericPeer) {
                createClient();
            }
            void onTimer() override {};
            
        };

    } //namespace bus
 } //namespace flexd         

int main(int argc, char** argv) {
    flexd::icl::ipc::FleXdEpoll poller(10);
    flexd::bus::Monitor monitor(poller);
    poller.loop();
}
