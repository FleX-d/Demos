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
 * File:   main.cpp
 * Author: Jakub Pekar
 */
 
 #include "FleXdLogger.h"
 #include "IPCInterface.h"
 #include "GenericClient.h"
 #include "FleXdEpoll.h"
 #include "FleXdTimer.h"
 #include <iostream>
 #include <dirent.h>
 #include <cerrno>
 #include <cstring>
 #include <fstream>
 #include <string>
 
 namespace flexd{
    namespace bus{ 
        class OneWired: public flexd::gen::IPCInterface{
        public:
            
    
            OneWired(flexd::icl::ipc::FleXdEpoll& poller)
            :IPCInterface(00201, poller),
            m_periodTime(8),
            m_tmpSensorConnected(false),
            m_OneWTimer(poller, m_periodTime, 0, true, [this](void){ this->onTimer(); }),
            m_tempReadPath("")
            {
                FLEX_LOG_INIT(poller,"OneWiredTemperature");
            }
            
            bool initOneWired(){
                if(initTemperature()){
                    m_tmpSensorConnected = true;
                }
                if(m_OneWTimer.start())
                {
                    FLEX_LOG_INFO("-> FleXdTimer.start() successful");
                } else {
                    FLEX_LOG_WARN("-> FleXdTimer.start() failed");
                }
                flexd::gen::GenericClient::Header head = {0,0,0,0, getMyID(), 00000};
                auto header = std::make_shared<flexd::gen::GenericClient::Header>(head);
                sendCreateClientMsg(header,00201,"OneW","OneWired","127.0.0.1", "backend/in", 2, true, 1883, 0, 60);
                return true;
            }
            
            ~OneWired(){}
            
            bool initTemperature()
            {
                DIR*     dir;
                dirent*  pdir = nullptr;
                std::string deviceAddr;

                dir = opendir(onewirePath);
                if(dir == NULL)
                {   
                    FLEX_LOG_ERROR("-> Error opening dir:", onewirePath," ERRNO:",std::strerror(errno));
                    return false;
                }

                while ((pdir = readdir(dir)) != nullptr)
                {
                    std::string file(pdir->d_name);
                    if(file.find("28-00000") != std::string::npos)
                    {
                        deviceAddr = file;
                        m_tempReadPath = onewirePath + deviceAddr + "/w1_slave";
                        FLEX_LOG_INFO("-> Read dir :", m_tempReadPath);
                        return true;
                    }
                }
                closedir(dir);
                FLEX_LOG_WARN("-> Problem with opening file with temperature :", m_tempReadPath);
                return false;
            }
            
            float readTemp()
            {
                std::string line;
                std::string temp;
                std::ifstream myfile(m_tempReadPath.c_str());
                if (myfile.is_open())
                {
                    while ( std::getline (myfile,line) )
                    {
                        FLEX_LOG_TRACE(line);
                        size_t pos = line.find("t=");
                        if(pos != std::string::npos)
                        {
                            temp = line.substr(pos+2);
                            myfile.close();
                            return float(std::stoi(temp)/1000.0);
                        }
                    }
                    myfile.close();
                }
                else
                {
                    FLEX_LOG_ERROR("-> Unable to open file :", m_tempReadPath);
                    return -1;
                }
                return -1;
            }
            
            void onTimer() override{
                if(m_tmpSensorConnected){
                    float temperature = readTemp();
                    if(temperature != -1){
                        std::string message("Temperature is: ");
                        message += std::to_string(temperature);
                        std::vector<uint8_t> data(message.begin(),message.end());
                        FLEX_LOG_DEBUG("-> Sending data to MCM: ", message );
                        
                        flexd::gen::GenericClient::Header head = {0,0,0,0, getMyID(), 00000};
                        auto header = std::make_shared<flexd::gen::GenericClient::Header>(head);
                        
                        sendPublishMsg(header,00101,"backend/in","OneWired",message);
                    } else {
                        FLEX_LOG_WARN("-> Temperature can`t be readed from file");
                    }
                } else {
                    FLEX_LOG_ERROR("-> Temperature sensor is not connected, data failed");
                }
            }
            void receiveRequestAckMsg(std::shared_ptr<flexd::gen::GenericClient::Header> header, uint32_t ID, uint8_t RequestAck) override{
                
            }
            void receiveBackMsg(std::shared_ptr<flexd::gen::GenericClient::Header> header, uint32_t ID, const std::string& PayloadMsg) override{
                
            }
            
            void onConnectPeer(uint32_t peerID, bool genericPeer) {
           
            }

        private:
            uint16_t m_periodTime;
            bool m_tmpSensorConnected;
            flexd::icl::ipc::FleXdTimer m_OneWTimer;
            static constexpr const char* onewirePath = "/sys/bus/w1/devices/";
            /*Path for testing on the computer*/
            //static constexpr const char* onewirePath = "/home/dev/work/w1_bus_master/";
            std::string m_tempReadPath;
        };

    } //namespace bus
} //namespace flexd
 
 int main(int argc, char** argv) {
    flexd::icl::ipc::FleXdEpoll poller(10);
    flexd::bus::OneWired OneWiredTemperature(poller);
    OneWiredTemperature.initOneWired();
    poller.loop();
 }
