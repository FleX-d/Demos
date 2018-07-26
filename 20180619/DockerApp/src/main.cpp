/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: dev
 *
 * Created on June 4, 2018, 10:19 AM
 */

#include "IPCClient.h"
#include <FleXdEvent.h>
#include <FleXdLogger.h>

int main(int argc, char** argv)
{
    flexd::icl::ipc::FleXdEpoll poller(10);
    FLEX_LOG_INIT(poller, "DockerApp");
    FLEX_LOG_TRACE("DockerApp starting");
    flexd::docker::IPCClient client(poller);
    flexd::icl::ipc::FleXdTermEvent e(poller);
    if(e.init()) {
        poller.loop();
    }
    FLEX_LOG_UNINIT();
    return 0;
}
