
/*
Copyright (c) 2017, Globallogic s.r.o.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
 * Neither the name of the Globallogic s.r.o. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC S.R.O. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
 * File:   IPCClient.h
 * Author: Matus Bodorik
 *
 * Created on Jul 04, 2018, 11:00 AM
 */


#include "IPCClient.h"
#include <unistd.h>
#include <JsonObj.h>
#include <INIParser.h>
#include <Base64.h>
#include <FleXdIPCCommon.h>
#include <FleXdLogger.h>

namespace flexd {
    namespace docker {

        IPCClient::IPCClient(flexd::icl::ipc::FleXdEpoll& poller)
        : IPCInterface(poller),
          m_counter(0),
          m_ipAddress(),
          m_topic(),
          m_direction(0),
          m_cleanSession(false),
          m_port(0),
          m_qos(0),
          m_keepAlive(0),
          m_path(),
          m_buffer() {
            init();
        }

        IPCClient::~IPCClient() {
        }

        void IPCClient::init() {
            if(ini::INIParser::getInstance().parseFiles("Resources/Config.ini")) {
                ini::INIParser::getInstance().getValue<std::string>("Mqtt:ipAddress", m_ipAddress);
                ini::INIParser::getInstance().getValue<std::string>("Mqtt:topic", m_topic);
                ini::INIParser::getInstance().getValue<int>("Mqtt:direction", m_direction);
                ini::INIParser::getInstance().getValue<bool>("Mqtt:cleanSession", m_cleanSession);
                ini::INIParser::getInstance().getValue<int>("Mqtt:port", m_port);
                ini::INIParser::getInstance().getValue<int>("Mqtt:qos", m_qos);
                ini::INIParser::getInstance().getValue<int>("Mqtt:keepAlive", m_keepAlive);
                ini::INIParser::getInstance().getValue<std::string>("DownloadFiles:path", m_path);
                FLEX_LOG_DEBUG("INIT MQTT: ipAddress: ", m_ipAddress, " Topic: ", m_topic, " Direction: ", m_direction, " cleanSession: ", m_cleanSession, " port: ", m_port, " qos: ", m_qos, " keepAlive: ", m_keepAlive);
                FLEX_LOG_DEBUG("INIT DownloadFiles path: ", m_path);
            } else {
                FLEX_LOG_DEBUG("!!INIT Fail!!");
            }
        }

        void IPCClient::createClient() {
            FLEX_LOG_DEBUG("Send Request for Create Client");
            std::shared_ptr<GenericClient::Header> header = std::make_shared<GenericClient::Header>();
            header->from = 12345;
            header->to = 00000;
            sendCreateClientMsg(std::move(header) ,12345, "DockerApp", "DocApp", m_ipAddress, m_topic, (uint8_t) m_direction, m_cleanSession, m_port, m_qos, m_keepAlive);
        }

        void IPCClient::sendSubScribe() {
            FLEX_LOG_DEBUG("Send Request for subscribe");
            std::shared_ptr<GenericClient::Header> header = std::make_shared<GenericClient::Header>();
            header->from = 12345;
            header->to = 00000;
            sendOperationMsg(std::move(header), 12345, "DocApp", 0);
        }

        bool IPCClient::writeToFile(std::string AppID) {
            base::BinStream b;
            if(flexd::icl::ipc::checkIfFileExist(m_path)) {
                b.setBase64(m_buffer);
                if(b.write(m_path + AppID)) {
                    return true;
                }
                return false;
            } else {
                flexd::icl::ipc::makeParentDir(m_path);
                b.setBase64(m_buffer);
                if(b.write(m_path + AppID)) {
                    return true;
                }
                return false;
            }
        }

        void IPCClient::receiveBackMsg(std::shared_ptr<GenericClient::Header> header, uint32_t ID, const std::string& PayloadMsg) {
            FLEX_LOG_DEBUG("Receive MSG from backend: ", PayloadMsg);
            uint8_t Operation = 0;
            std::string Message = "";
            std::string AppID = "";
            bool temp = true;
            std::shared_ptr<GenericClient::Header> sendheader = std::make_shared<GenericClient::Header>();
            sendheader->from = 12345;
            sendheader->to = 00000;
            if(PayloadMsg.empty()) {
                FLEX_LOG_DEBUG("Message is empty!");
                sendPublishMsg(std::move(sendheader), 12345 , "backend/out", "DocApp", "Msg is empty!");
            } else {
                try {
                    flexd::icl::JsonObj json(PayloadMsg);
                    if(json.exist("/Operation")) {
                        json.get<uint8_t>("/Operation", Operation);
                    } else {
                        temp = false;
                    }
                    if(json.exist("/Message")) {
                        json.get<std::string>("/Message", m_buffer);
                    } else {
                        temp = false;
                    }
                    if(json.exist("/AppID")) {
                        json.get<std::string>("/AppID", AppID);
                    } else {
                        temp = false;
                    }

                    if(!temp) {
                        FLEX_LOG_DEBUG("Message is invalid!");
                        sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", "Msg is invalid!");
                    }
                } catch(...) {
                    FLEX_LOG_DEBUG("Message is invalid!");
                    sendPublishMsg(std::move(sendheader), 12345 ,"backend/out", "DocApp", "Msg is invalid!");
                    temp = false;
                    return;
                }
            }

            if(temp) {
                if(Operation == 0) {
                    if(writeToFile(AppID)) {
                        sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", "Receive valid Message, Sending to Core");
                        sendRequestCoreMsg(Operation, m_path + AppID ,AppID);
                        m_buffer.clear();
                    }
                } else {
                    FLEX_LOG_DEBUG("Receive valid MSG: Operation: ", Operation, " Message: ", Message, " AppID: ", AppID);
                    sendRequestCoreMsg(Operation, m_path + AppID, AppID);
                    sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", "Receive valid Operation MSG, Sending to Core");
                }
            } else {
                FLEX_LOG_DEBUG("Message is invalid!");
                sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", "Msg is invalid!");
            }

        }

        void IPCClient::receiveRequestAckMsg(std::shared_ptr<GenericClient::Header> header, uint32_t ID, uint8_t RequestAck) {
            FLEX_LOG_DEBUG("ID: ", ID, ", RequestAck: ", (int)RequestAck);
            std::shared_ptr<GenericClient::Header> sendheader = std::make_shared<GenericClient::Header>();
            sendheader->from = 12345;
            sendheader->to = 00000;
            if(m_counter == 0 && RequestAck == 1) {
                sleep(2);
                FLEX_LOG_DEBUG("Create CLient Success: ", ID);
                sendPublishMsg(std::move(sendheader), 12345,  "backend/out", "DocApp", "Create CLient Success");
                sendSubScribe();
                m_counter++;
            } else if (m_counter == 0 && RequestAck == 0) {
                sleep(5);
                FLEX_LOG_DEBUG("Create CLient Fail: ", ID);
                sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", "Create CLient Fail");
                createClient();
            } else if (m_counter == 0 && RequestAck == 2) {
                FLEX_LOG_DEBUG("Create CLient Fail, because is exists: ", ID);
                sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", "Create CLient Fail, already exists!");
                sendSubScribe();
                m_counter++;
            } else if(m_counter == 1 && RequestAck == 1) {
                FLEX_LOG_DEBUG("Client Subscribe Success: ", ID);
                sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", "Subscribe CLient Success");
                m_counter++;
            }else if(m_counter == 1 && RequestAck == 0) {
                sleep(5);
                FLEX_LOG_DEBUG("Client Subscribe Fail: ", ID);
                sendSubScribe();
                //sendPublishMsg(12345, "backend/out", "DocApp", "Subscribe CLient Fail");
            } else {
                FLEX_LOG_DEBUG("Publish MSG to backend Success: ", ID);
            }
        }

        void IPCClient::receiveRequestCoreAckMsg(bool OperationAck, const std::string& Message, const std::string& AppID) {
            FLEX_LOG_DEBUG("Receive RequestAck Core Msg ", "OperationAck: ", OperationAck, " Message: ", Message, " AppID: ", AppID);
            std::string temp = "Acknowledge form Core: ";
            flexd::icl::JsonObj json= {};
            json.add<bool>("/OperationAck", OperationAck);
            json.add<std::string>("/Message", Message);
            json.add<std::string>("/AppID", AppID);
            temp += json.getJson();
            std::shared_ptr<GenericClient::Header> sendheader = std::make_shared<GenericClient::Header>();
            sendheader->from = 12345;
            sendheader->to = 00000;
            sendPublishMsg(std::move(sendheader), 12345, "backend/out", "DocApp", temp);
        }

        void IPCClient::onConnectPeer(uint32_t peerID, bool genericPeer) {
            createClient();
        }
    }
}
