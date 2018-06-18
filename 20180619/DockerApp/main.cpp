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
#include "INIParser.h"
#include "Base64.h"
#include "FleXdIPCCommon.h"
#include <FleXdEvent.h>

class IPCClient : public flexd::gen::IPCInterface{
    public :

    int counter = 0;
    std::string ipAddress;
    std::string topic;
    int direction;
    bool cleanSession;
    int port; 
    int qos;
    int keepAlive;
    std::string path;
    std::string m_segmentBuffer;
    
    IPCClient(flexd::icl::ipc::FleXdEpoll& poller)
    : IPCInterface(poller)
    {
        init();
    }

    virtual ~IPCClient(){
    }

    void init(){
        if(ini::INIParser::getInstance().parseFiles("../Resources/Config.ini"))
        {
            ini::INIParser::getInstance().getValue<std::string>("Mqtt:ipAddress", ipAddress);
            ini::INIParser::getInstance().getValue<std::string>("Mqtt:topic", topic);
            ini::INIParser::getInstance().getValue<int>("Mqtt:direction", direction);
            ini::INIParser::getInstance().getValue<bool>("Mqtt:cleanSession", cleanSession);
            ini::INIParser::getInstance().getValue<int>("Mqtt:port", port);
            ini::INIParser::getInstance().getValue<int>("Mqtt:qos", qos);
            ini::INIParser::getInstance().getValue<int>("Mqtt:keepAlive", keepAlive);
            ini::INIParser::getInstance().getValue<std::string>("DownloadFiles:path", path);
            std::cout << "INIT MQTT: ipAddress: " << ipAddress << " Topic: " << topic << " Direction: " << direction << " cleanSession: " << cleanSession << " port: " << port << " qos: " << qos << " keepAlive: " << keepAlive << std::endl;
            std::cout << "INIT DownloadFiles path: " << path << std::endl;
        } else 
        {
            std::cout << "!!INIT Fail!!" << std::endl;
        }
    }
    
    void createClient(){
       std::cout << "Send Request for Create Client"<< std::endl;
       sendCreateClientMsg("DockerApp", "DockerApp", "DocApp", ipAddress, topic, (uint8_t)direction, cleanSession, port, qos, keepAlive);
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
            if(Operation == 0 && Operation == 6)
            {
                if(writeToFile(AppID))
                {
                   sendPublishMsg("DockerApp", "backend/out", "DocApp", "Receive valid Message, Sending to Core");
                   sendRequestCoreMsg(Operation, path + AppID ,AppID);
                   m_segmentBuffer.clear();
                }
            } else {
            std::cout << "Receive valid MSG: Operation: " << Operation << " Message: " << Message << " AppID: " << AppID << std::endl;
            sendRequestCoreMsg(Operation, Message, AppID);
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Receive valid Operation MSG, Sending to Core");
            }
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
            sleep(5);
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
            sleep(5);
            std::cout << "Client Subscribe Fail: " << ID << std::endl;
            sendSubScribe();
            //sendPublishMsg("DockerApp", "backend/out", "DocApp", "Subscribe CLient Fail");
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
        std::cout << "Receive Segmented Msg -> Segment: " << (int)segment << " Count: " << (int)count << " PayloadMsg: " << PayloadMsg.size() << std::endl;

           m_segmentBuffer += PayloadMsg;
           if(segment == count){               
               flexd::icl::JsonObj json(m_segmentBuffer);
               uint8_t Operation;
               std::string Message;
               std::string AppID;

               if(json.exist("/Operation")){
                   json.get<uint8_t>("Operation", Operation);
               }
               if(json.exist("/Message")){
                   json.get<std::string>("/Message", Message);
               }
               if(json.exist("/AppID")){
                   json.get<std::string>("/AppID", AppID);
               }
              
               if(writeToFile(AppID))
               {
                   sendPublishMsg("DockerApp", "backend/out", "DocApp", "Receive valid segmented Message, Sending to Core");
                   sendRequestCoreMsg(Operation, path + AppID ,AppID);
                   m_segmentBuffer.clear();
               }
           }
    }
    
    bool writeToFile(std::string AppID){
        base::BinStream b;
        if(flexd::icl::ipc::checkIfFileExist(path))
        {
            b.setBase64(m_segmentBuffer);
            if(b.write(path + AppID))
            {
                return true;
            }
            return false;
        } else {
            flexd::icl::ipc::makeParentDir(path);
            b.setBase64(m_segmentBuffer);
            if(b.write(path + AppID))
            {
                return true;
            }
            return false;
        }
        
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

