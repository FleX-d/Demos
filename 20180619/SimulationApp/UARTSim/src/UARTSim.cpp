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
 * File:   UARTSim.cpp
 * Author: Jakub Pekar
 */

#include "UARTSim.h"
#include "FleXdLogger.h"
#include <ctime>
#include <iostream>


void intToField(std::vector<uint8_t>& data, int number){
    data.push_back((number) & 0xFF);
    data.push_back((number >> 8) & 0xFF);
    data.push_back((number >> 16) & 0xFF);
    data.push_back((number >> 24) & 0xFF);
}

UARTSim::UARTSim(flexd::icl::ipc::FleXdEpoll& poller)
:IPCInterface(100, poller),
m_periodTime(2),
m_UARTTimer(poller, m_periodTime, 0, true, [this](void){ this->onTimer(); })
{   
    FLEX_LOG_INIT(poller,"UARTSimulator");
    
    if(m_UARTTimer.start())
    {
        FLEX_LOG_INFO(" -> FleXdTimer.start() successful");
    } else {
        FLEX_LOG_INFO(" -> FleXdTimer.start() failed");
    }
    FLEX_LOG_INFO(" -> Constructor was successful perform");
}

UARTSim::~UARTSim()
{
    m_UARTTimer.stop();
}


void UARTSim::onTimer() 
{   
    //uint32_t peerID = 0;
    srand(time(NULL));
    int number = std::rand() % 35;
    std::string message = "Data from UART: temperature = " + std::to_string(number) + "°C"; 
    std::vector<uint8_t> data(message.begin(),message.end());
    /*auto msg = std::make_shared<flexd::icl::ipc::FleXdIPCMsg>(std::move(data));
    
    msg->getAdditionalHeader()->setValue_1(0);
    msg->getAdditionalHeader()->setValue_4(getMyID());
    msg->getAdditionalHeader()->setValue_5(peerID);
    sendMsg(msg);*/
    FLEX_LOG_TRACE(" -> Sending data to MCM: ", message );
}


