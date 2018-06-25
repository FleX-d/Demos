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
 * File:   1WSim.cpp
 * Author: Jakub Pekar
 */
 
 
 #include "FleXdLogger.h"
 #include "IPCInterface.h"
 #include "FleXdEpoll.h"
 #include "FleXdTimer.h"
 #include <ctime>
 #include <iostream>
 
namespace flexd{
    namespace bus{ 
        class OneWSim: public flexd::bus::IPCInterface{
        public:
            void onTimer() override{
                srand(time(NULL));
                int number = std::rand();
                std::string message = "Data from One Wired: Voltage = " + std::to_string(number % 6) + "." +std::to_string(number % 10); 
                std::vector<uint8_t> data(message.begin(),message.end());
                FLEX_LOG_TRACE("-> Sending data to MCM: ", message );
                sendPublishMsg(101,"backend/in","OneWired",message);
            }
    
            OneWSim(flexd::icl::ipc::FleXdEpoll& poller)
            :IPCInterface(101, poller),
            m_periodTime(8),
            m_OneWTimer(poller, m_periodTime, 0, true, [this](void){ this->onTimer(); })
            {
                FLEX_LOG_INIT(poller,"OneWiredSimulator");
                if(m_OneWTimer.start())
                {
                    FLEX_LOG_INFO("-> FleXdTimer.start() successful");
                } else {
                    FLEX_LOG_INFO("-> FleXdTimer.start() failed");
                }
                sendCreateClientMsg(101,"OneW","OneWired","127.0.0.1", "backend/in", 2, true, 1883, 0, 60);
            }
    
            ~OneWSim(){}

        private:
            uint16_t m_periodTime;
            flexd::icl::ipc::FleXdTimer m_OneWTimer;    
        };

    } //namespace bus
} //namespace flexd

int main(int argc, char** argv) {
    flexd::icl::ipc::FleXdEpoll poller(10);
    flexd::bus::OneWSim wiredsimulator(poller);
    poller.loop();
}