# FleX-d demo

FleX-d is flexibile multiplaform minimal distro for every architecture.
This demo is example of installing and running all part of flexd. 

# How to install all part

## Prerequisities:
    sudo apt-get install pkg-config cmake git libsqlite3-dev

## Install
1. Create folder for project and open it
2. Download repository of libs and install in following order

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

There is a bug in the CMakeList in the folder man - required to add .xml prefix to all files in cmakelist

### Nlohman jsonparser (optional)

Library for parsing Json

    git clone https://github.com/nlohmann/json.git
    cd json
    mkdir build
    cd build 
    cmake .. 
    make
    sudo make install
    sudo ldconfig

### ICL Internal comunication layer 

Internal communication layer provides communication among aplications via unix domain sokets 
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
Mosquitto Comunication Manager - it is an aplication using Mosquitto Client facade for managing multiple clients and also using ICL layer to communicate with other aplications

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

Coreapp is base of FleX-d, database stores records of all aplications and commands for this aplications.
Coreapp can start an aplication or a container.

*Download and compile CoreApp 

    https://github.com/FleX-d/Core.git
    cd Core
    git checkout dev 
    mkdir build 
    cd build 
    cmake .. 
    make 

    Option to Enable tests: cmake -DENABLE_TESTS=ON ..

*Run script initDB.sh to prepare the database

    cd Core/apps/CoreApp/tool/initDB.sh
    ./initDB.sh

*Copy prepared db file to bin CoreApp

    cp Core/apps/CoreApp/tool/CoreAppDb.db build/apps/CoreApp/CoreAppDb.db

For adding example it is required to edit the database file, e.g. use Sqlitebrowser for editing. It is required to add record with appName appVer and with a command for terminal. 
Example: echo install colum; echo command separated with semicolon; echo added in one colum
Coreapp executes this command when recives message with right name and operation.
Now the CoreApp is ready to launch and run docker container or other aplication.


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

Operation can be: 1-install, 2-uninstall, 3-start, 4-stop, 5-freeze, 6-unfreeze, 7-update, 8-255 is unused (set as invalid) and retun false operation result

    mosquitto_pub -t backend/in -m '{"AppID":"*Your App Name*","Message":"*Binary dif for install at base64*","Operation": 1-255 }'

## Subscribe topic from another terminal 

    mosquitto_sub -t "backend/out"
