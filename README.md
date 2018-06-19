# FleX-d demo

FleX-d is flexibile multiplaform minimal distro for all architekture
This demo is example of instaling and running all part of flexd. 

# How install all part

## Prerequisities:
    sudo apt-get install pkg-config cmake git libsqlite3-dev sqlite3

## Install
1. Create folder for project and open it
2. Download repositary of libs and install in this order

### Mosquitto

Mosquitto library 

    git clone https://github.com/eclipse/mosquitto.git
    cd mosquitto
    mkdir build
    cd build 
    cmake ..
    make	
    sudo make install
    sudo ldconfig

bug with cmakelist in folder man - have to add .xml prefix to all file at cmakelist

### Nlohman jsonparser (optional)

Library for parse Json

    git clone https://github.com/nlohmann/json.git
    cd json
    mkdir build
    cd build 
    cmake .. 
    make
    sudo make install
    sudo ldconfig

### ICL Internal comunication layer 

Internal communication layer provide communication between aplications via unix domain sokets 
and contains tools for working with Json data, encode/decode file to Base64

    git clone https://github.com/FleX-d/ICL.git
    cd ICL
    git checkout dev
    mkdir build
    cd build 
    cmake ..
    make
    sudo make install
    sudo ldconfig

    Option to Enable tests and Examples: cmake -DENABLE_TESTS=ON -DENABLE_EXAMPLES=ON ..

### DLT (optional)

Logging tool 

    git clone https://github.com/GENIVI/dlt-daemon.git
    cd dlt-daemon
    mkdir build
    cd build 
    cmake ..
    make
    sudo make install
    sudo ldconfig

### LTD

Log & Trace and debug tools for applications in FleXd project.

    git clone https://github.com/FleX-d/LTD.git
    cd LTD
    git checkout dev
    mkdir build 
    cd build 
    cmake ..
    make 
    sudo make install
    sudo ldconfig

    Option to Enable tests and Examples: cmake -DENABLE_TESTS=ON -DENABLE_EXAMPLES=ON ..

### RSM

Remote Servise Manager contains:
Mosquitto client facade based on Mosquitto library https://github.com/eclipse/mosquitto
Mosquitto Comunication Manager  - is aplication witch is using Mosquitto Client facade for manage multiple clients and also using ICL layer to communicate with other aplications

    git clone https://github.com/FleX-d/RSM.git
    cd RSM
    git checkout dev
    mkdir build
    cd build 
    cmake ..
    make
    sudo make install
    sudo ldconfig

    Option to Enable tests: cmake -DENABLE_TESTS=ON ..

### CORE

Coreapp is base of FleX-d, database store records of all aplications and commands for this aplications.
Coreapp can start aplication or container.

*Download and compile CoreApp 

    https://github.com/FleX-d/Core.git
    cd Core
    git checkout dev 
    mkdir build 
    cd build 
    cmake .. 
    make 

    Option to Enable tests: cmake -DENABLE_TESTS=ON ..

*Run script initDB.sh to prepare database

    cd Core/apps/CoreApp/tool/initDB.sh
    ./initDB.sh

*Copy prepared db file to bin CoreApp

    cp Core/apps/CoreApp/tool/CoreAppDb.db build/apps/CoreApp/CoreAppDb.db

For adding you example you must edit database file. We use sqlitebroser for this. Add record with appName appVer and with command for terminal 
example: echo install colum; echo command separated with semicolon; echo added in one colum
Coreapp execute this command when recive message with right name and operation
Now you are ready lunch CoreApp and run docker container or other aplication


# DEMO

## RUN Mosquitto

    /usr/local/sbin/mosquitto -v

## Run mosquitto manager

    cd RSM/build/apps/MCManager
    ./mcManager

## Build and run demo docker app 

    git clone https://github.com/FleX-d/Demos.git
    cd Demos
    git checkout dev
    mkdir build 
    cd build 
    cmake ..
    make 
    cd Demos/20180619/DockerApp/build
    ./DockerApp

## Run Core app 

    cd  build/apps/CoreApp
    ./CoreApp

## Use command to send message from another terminal

Operation can be: 1-install, 2-uninstall, 3-start, 4-stop, 5-freeze, 6-unfreeze, 7-update, 8-255 is unused (set als invalid) retun false operation result

    mosquitto_pub -t backend/in -m '{"AppID":"*Your App Name*","Message":"*Binary dif for install at base64*","Operation": 1-255 }'

## Subscribe topic from another terminal 

    mosquitto_sub -t "backend/out"
