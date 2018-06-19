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
#include <FleXdLogger.h>

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
    std::string m_buffer;
    
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
            FLEX_LOG_DEBUG("INIT MQTT: ipAddress: ", ipAddress, " Topic: ", topic, " Direction: ", direction, " cleanSession: ", cleanSession, " port: ", port, " qos: ", qos, " keepAlive: ", keepAlive);
            FLEX_LOG_DEBUG("INIT DownloadFiles path: ", path);
        } else 
        {
            FLEX_LOG_DEBUG("!!INIT Fail!!");
        }
    }
    
    void createClient(){
       FLEX_LOG_DEBUG("Send Request for Create Client");
       sendCreateClientMsg("DockerApp", "DockerApp", "DocApp", ipAddress, topic, (uint8_t)direction, cleanSession, port, qos, keepAlive);
    }

    void sendSubScribe(){
        FLEX_LOG_DEBUG("Send Request for subscribe");
        sendOperationMsg("DockerApp", "DocApp", 0);
    }

    void receiveBackMsg(const std::string& PayloadMsg) override
    {
        FLEX_LOG_DEBUG("Receive MSG from backend: ", PayloadMsg);
        uint8_t Operation = 0;
        std::string Message = "";
        std::string AppID = "";
        bool temp = true;
        if(PayloadMsg.empty()){
            FLEX_LOG_DEBUG("Message is empty!");
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
                json.get<std::string>("/Message", m_buffer);
                } else {
                    temp = false;
                }
                if(json.exist("/AppID")){
                json.get<std::string>("/AppID", AppID);
                } else {
                    temp = false;
                }

                if(!temp){
                    FLEX_LOG_DEBUG("Message is invalid!");
                    sendPublishMsg("DockerApp", "backend/out", "DocApp", "Msg is invalid!");
                }

            }catch(...){
                FLEX_LOG_DEBUG("Message is invalid!");
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
                   m_buffer.clear();
                }
            } else {
            FLEX_LOG_DEBUG("Receive valid MSG: Operation: ", Operation, " Message: ", Message, " AppID: ", AppID);
            sendRequestCoreMsg(Operation, path + AppID, AppID);
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Receive valid Operation MSG, Sending to Core");
            }
        } else {
            FLEX_LOG_DEBUG("Message is invalid!");
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Msg is invalid!");
        }

    }

    void receiveRequestAckMsg(const std::string& ID, uint8_t RequestAck) override
    {
        FLEX_LOG_DEBUG("ID: ", ID, ", RequestAck: ", (int)RequestAck);
        if(counter == 0 && RequestAck == 1)
        {
            sleep(2);
            FLEX_LOG_DEBUG("Create CLient Success: ", ID);
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Create CLient Success");
            sendSubScribe();
            counter++;
        } else if (counter == 0 && RequestAck == 0){
            sleep(5);
            FLEX_LOG_DEBUG("Create CLient Fail: ", ID);
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Create CLient Fail");
            createClient();
        } else if (counter == 0 && RequestAck == 2) {
            FLEX_LOG_DEBUG("Create CLient Fail, because is exists: ", ID);
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Create CLient Fail, already exists!");
            sendSubScribe();
            counter++;
        } else if(counter == 1 && RequestAck == 1){
            FLEX_LOG_DEBUG("Client Subscribe Success: ", ID);
            sendPublishMsg("DockerApp", "backend/out", "DocApp", "Subscribe CLient Success");
            counter++;
        }else if(counter == 1 && RequestAck == 0){
            sleep(5);
            FLEX_LOG_DEBUG("Client Subscribe Fail: ", ID);
            sendSubScribe();
            //sendPublishMsg("DockerApp", "backend/out", "DocApp", "Subscribe CLient Fail");
        } else {
            FLEX_LOG_DEBUG("Publish MSG to backend Success: ", ID);
        }
    }

    void receiveRequestCoreAckMsg(bool OperationAck, const std::string& Message, const std::string& AppID)
    {
        FLEX_LOG_DEBUG("Receive RequestAck Core Msg ", "OperationAck: ", OperationAck, " Message: ", Message, " AppID: ", AppID);
        std::string temp = "Acknowledge form Core: ";
        flexd::icl::JsonObj json= {};
        json.add<bool>("/OperationAck", OperationAck);
        json.add<std::string>("/Message", Message);
        json.add<std::string>("/AppID", AppID);
        temp += json.getJson();
        sendPublishMsg("DockerApp", "backend/out", "DocApp", temp);
    }
    
    bool writeToFile(std::string AppID){
        base::BinStream b;
        if(flexd::icl::ipc::checkIfFileExist(path))
        {
            b.setBase64(m_buffer);
            if(b.write(path + AppID))
            {
                return true;
            }
            return false;
        } else {
            flexd::icl::ipc::makeParentDir(path);
            b.setBase64(m_buffer);
            if(b.write(path + AppID))
            {
                return true;
            }
            return false;
        }
        
    }

    void onConnectPeer(uint32_t peerID, bool genericPeer)
    {
        createClient();
    }

};


int main(int argc, char** argv)
{
    flexd::icl::ipc::FleXdEpoll poller(10);
    FLEX_LOG_INIT(poller, "DockerApp");
    FLEX_LOG_TRACE("DockerApp starting");
    IPCClient client(poller);
    flexd::icl::ipc::FleXdTermEvent e(poller);
    if( e.init()){
        poller.loop();
    }
    return 0;
}
