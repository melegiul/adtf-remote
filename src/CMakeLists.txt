cmake_minimum_required(VERSION 3.1.0)
project(autonomous_controller)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(ADTF_DIR /mnt/data/VMs/uniautonom-lxc/opt/adtf/2.14.2)

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
find_package(ADTF REQUIRED)

adtf_add_executable(AutonomousController
    mainwindow.ui
    mainwindow.cpp
    mainwindow.h
    dialog_preferences.cpp
    dialog_preferences.h
    dialog_preferences.ui
    adtfmediasample.h
    networkclient.cpp
    networkclient.h
    main.cpp
    resources.qrc

    adtf_structs/tmyadtfmessage.cpp
    adtf_structs/tmyadtfmessage.h
    adtf_structs/carodometry.cpp
    adtf_structs/carodometry.h
)

target_link_libraries(AutonomousController Qt5::Core Qt5::Widgets Qt5::Network)
