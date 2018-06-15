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

#include "IPCInterface.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <JsonObj.h>
#include <FleXdEpoll.h>
#include <FleXdEvent.h>

class IPCClient : public flexd::gen::IPCInterface{
    public :

    int counter = 0;
    IPCClient(flexd::icl::ipc::FleXdEpoll& poller)
    : IPCInterface(poller)
    {
    }

    virtual ~IPCClient(){
    }

    void createClient(){
       std::cout << "Send Request for Create Client"<< std::endl;
       sendCreateClientMsg("DockerApp", "DockerApp", "DocApp", "127.0.0.1", "backend/in", 2, true, 1883, 0, 60);
    }

    void sendSubScribe(){
        std::cout << "Send Request for subscribe"<< std::endl;
        sendOperationMsg("DockerApp", "DocApp", 0);
    }


    void receiveBackMsg(const std::string& PayloadMsg) override
    {
        std::cout << "Receive MSG from backend: " << PayloadMsg << std::endl;
        uint8_t Operation = 0;
        std::string Message = "";
        std::string AppID = "";
        bool temp = true;
        if(PayloadMsg.empty()){
            std::cout << "Message is empty!"<< std::endl;
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Msg is empty!");
        } else
        {
            try{
                flexd::icl::JsonObj json(PayloadMsg);
                if(json.exist("/Operation")){
                json.get<uint8_t>("/Operation", Operation);
                } else {
                    temp = false;
                }
                if(json.exist("/Message")){
                json.get<std::string>("/Message", Message);
                } else {
                    temp = false;
                }
                if(json.exist("/AppID")){
                json.get<std::string>("/AppID", AppID);
                } else {
                    temp = false;
                }

                if(!temp){
                    std::cout << "Message is invalid!"<< std::endl;
                    sendPublishMsg("DockerApp", "backend/out", "DocApp", "Msg is invalid!");
                }

            }catch(...){
                std::cout << "Message is invalid!"<< std::endl;
                sendPublishMsg("DockerApp", "backend/out", "DocApp", "Msg is invalid!");
                temp = false;
                return;
            }
        }

        if(temp){
            std::cout << "Receive valid MSG: Operation: " << Operation << " Message: " << Message << " AppID: " << AppID << std::endl;
            sendRequestCoreMsg(Operation, Message, AppID);
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Receive valid Operation MSG, Sending to Core");
        } else {
            std::cout << "Message is invalid!"<< std::endl;
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Msg is invalid!");
        }

    }

    void receiveRequestAckMsg(const std::string& ID, uint8_t RequestAck) override
    {
        std::cout << "ID: " << ID << ", RequestAck: " << (int)RequestAck << std::endl;
        if(counter == 0 && RequestAck == 1)
        {
            sleep(2);
            std::cout << "Create CLient Success: " << ID << std::endl;
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Create CLient Success");
            sendSubScribe();
            counter++;
        } else if (counter == 0 && RequestAck == 0){
            std::cout << "Create CLient Fail: " << ID << std::endl;
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Create CLient Fail");
            createClient();
        } else if (counter == 0 && RequestAck == 2) {
            std::cout << "Create CLient Fail, because is exists: " << ID << std::endl;
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Create CLient Fail, already exists!");
            sendSubScribe();
            counter++;
        } else if(counter == 1 && RequestAck == 1){
            std::cout << "Client Subscribe Success: " << ID << std::endl;
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Subscribe CLient Success");
            counter++;
        }else if(counter == 1 && RequestAck == 0){
            std::cout << "Client Subscribe Fail: " << ID << std::endl;
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Subscribe CLient Fail");
        } else {
            std::cout << "Publish MSG to backend Success: " << ID << std::endl;
        }
    }

    void receiveRequestCoreAckMsg(bool OperationAck, const std::string& Message, const std::string& AppID)
    {
        std::cout << "Receive RequestAck Core Msg " << "OperationAck: " << OperationAck << " Message: " << Message << " AppID: " << AppID << std::endl;
        std::string temp = "Acknowledge form Core: ";
        flexd::icl::JsonObj json= {};
        json.add<bool>("/OperationAck", OperationAck);
        json.add<std::string>("/Message", Message);
        json.add<std::string>("/AppID", AppID);
        temp += json.getJson();
        sendPublishMsg("DockerApp", "backend/out", "DocApp", temp);
    }

    void receiveBackMsgSegmented(uint8_t segment, uint8_t count, const std::string& PayloadMsg) override
    {
        std::cout << "Receive Segmented Msg -> Segment: " << (int)segment << " Count: " << (int)count << " PayloadMsg: " << PayloadMsg << std::endl;
        sendRequestCoreSegmented(segment, count, PayloadMsg);
    }

    void onConnectPeer(uint32_t peerID) override
    {
        createClient();
    }

};


int main(int argc, char** argv)
{
    flexd::icl::ipc::FleXdEpoll poller(10);
    IPCClient client(poller);
    flexd::icl::ipc::FleXdTermEvent e(poller);
    if( e.init()){
        poller.loop();
    }
    return 0;
}

