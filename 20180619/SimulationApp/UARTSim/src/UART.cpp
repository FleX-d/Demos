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
 * File:   UART.cpp
 * Author: Jakub Pekar
 */
#include "IPCInterface.h"
#include "FleXdEpoll.h"
#include "FleXdTimer.h"
#include "FleXdLogger.h"

namespace flexd{
    namespace bus{ 
        class UARTSim : public IPCInterface{
        public:
            UARTSim(flexd::icl::ipc::FleXdEpoll& poller) 
            :IPCInterface(100, poller),
            m_periodTime(3),
            m_UARTTimer(poller, m_periodTime, 0, true, [this](void){ this->onTimer(); })
            {   
                FLEX_LOG_INIT(poller,"UARTSimulator");

                if(m_UARTTimer.start())
                {
                    FLEX_LOG_INFO("-> FleXdTimer.start() successful");
                } else {
                    FLEX_LOG_INFO("-> FleXdTimer.start() failed");
                }
                sendCreateClientMsg("UART","UART","UARTSim","127.0.0.1", "backend/in", 2, true, 1883, 0, 60);
                FLEX_LOG_INFO("-> Constructor was successful perform");
            }
            ~UARTSim(){ m_UARTTimer.stop(); }
            UARTSim(const UARTSim&) = delete;
    
            void onTimer() override{
        
                srand(time(NULL));
                int number = std::rand() % 35;
                std::string message = "Data from UART: temperature = " + std::to_string(number) + "°C"; 
                std::vector<uint8_t> data(message.begin(),message.end());
    
                FLEX_LOG_TRACE("-> Sending data to MCM: ", message );
                sendPublishMsg("UART","backend/in","UARTSim",message);
            }

        private:
            uint16_t m_periodTime;
            flexd::icl::ipc::FleXdTimer m_UARTTimer;
        };
    } //namespace bus
} //namespace flexd

int main(int argc, char** argv) {
    flexd::icl::ipc::FleXdEpoll poller(10);
    flexd::bus::UARTSim uartsimulator(poller);
    poller.loop();
}