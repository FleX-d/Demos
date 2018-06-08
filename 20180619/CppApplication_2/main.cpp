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

#include <cstdlib>
#include <iostream>
#include <nlohmann/json.hpp>
#include "IPCInterface.h"
#include "JsonObj.h"
#include "FleXdEpoll.h"


class IPCClient : public flexd::gen::IPCInterface{
    public :
        
    int counter = 0;
    IPCClient(flexd::icl::ipc::FleXdEpoll& poller) 
    : IPCInterface(poller)
    {
    }
    void createClient(){
       sendCreateClientMsg("DockerApp", "DockerApp", "DocApp", "127.0.0.1", "backend/in", 2, true, 1883, 0, 60);
    }
    
    void sendSubScribe(){
        sendOperationMsg("DockerApp", "DocApp", 0);
    }
    
    
    void receiveBackMsg(const std::string& PayloadMsg) override
    {
        bool allright = false;
        uint8_t Operation = 0;
        std::string Message = "";
        std::string AppID = 0;
        std::cout << "Receive MSG from backend" << std::endl;
        if(PayloadMsg.empty()){
            std::cout << "PayloadMessage is empty" << std::endl;
        } else 
        {
            try {
                flexd::icl::JsonObj json(PayloadMsg);
                allright = json.get<uint8_t>("/Operation", Operation);
                allright = json.get<std::string>("/Message", Message);
                allright = json.get<std::string>("/AppID", AppID);
                std::cout << "Operation: " << Operation << " Message: " << Message << " AppID: " << AppID << std::endl;
            }catch (nlohmann::detail::parse_error& e)
            {
                std::cout << "nlohmann::detail::parse_error" <<  std::endl;
            }
        }
        
        if(allright){
            std::cout << "SendRequestCoreMsg " << std::endl;
            sendRequestCoreMsg(Operation, Message, AppID);
        }
        
    }
    
    void receiveRequestAckMsg(const std::string& ID, uint8_t RequestAck) override
    {
        std::cout << "ID: " << ID << ", RequestAck: " << (int)RequestAck << std::endl;
        if(counter == 0 && RequestAck == 1)
        {
            std::cout << "Create CLient Success: " << ID << std::endl;
            sendSubScribe();
            counter++;
        } else if(counter == 1 && RequestAck == 1)
        {
            std::cout << "Client Subscribe Success: " << ID << std::endl;
        }
    }
    
    void receiveRequestCoreAckMsg(bool OperationAck, const std::string& Message, const std::string& AppID) override
    {
        std::cout << "Receive Request Core Ack Msg " <<  std::endl;
        std::string temp = {};
        flexd::icl::JsonObj json= {};
        json.add<bool>("/OperationAck", OperationAck);
        json.add<std::string>("/Message", Message);
        json.add<std::string>("/AppID", AppID);
        std::cout << "OperationAck: " << OperationAck << " Message: " << Message << " AppID: " << AppID << std::endl;
        temp = json.getJson();
        
        sendPublishMsg("DockerApp", "backend/out", "DocApp", temp);
    }

    void onConnectPeer(uint32_t peerID) override
    {
    }
    
};


int main(int argc, char** argv)
{
    flexd::icl::ipc::FleXdEpoll poller(10);
    IPCClient client(poller);
    client.createClient();
    poller.loop();
    return 0;
}

