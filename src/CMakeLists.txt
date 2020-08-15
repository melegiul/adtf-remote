cmake_minimum_required(VERSION 3.1.0)
project(uniautonom_remotegui)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(ADTF_DIR /opt/adtf/2.14.3)

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
find_package(ADTF REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(.)
include_directories(include)
include_directories(/usr/local/include/opencv4)
include_directories(build/UNIAutonomRemoteGUI_autogen/include)

adtf_add_executable(UNIAutonomRemoteGUI
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    dialog_preferences.cpp
    dialog_preferences.ui
    dialog_log_analyzer.cpp
    dialog_log_analyzer.ui
    log.cpp

    adtfmediasample.h
    networkclient.cpp

    ../res/res.qrc

    QtWidgets/bottomleftgraphicsview.cpp
    QtWidgets/resizegraphicsview.cpp

    adtf_converters/carodometry.cpp
    adtf_converters/trapezoid.cpp
    adtf_converters/detectedline.cpp
    adtf_converters/nearfieldgridmap.cpp
    adtf_converters/remoteStateMsg.cpp
    adtf_converters/logMsg.cpp
    adtf_converters/speed.cpp
    adtf_converters/mediaDesciptionSingleton.cpp

    CustomGraphicsItems/NearfieldMapItem.cpp
    CustomGraphicsItems/SpecialMarkingItem.cpp
    CustomGraphicsItems/LaneDirectionItem.cpp
    CustomGraphicsItems/QNavigationMarkerListWidget.cpp
    CustomGraphicsItems/LaneItem.cpp
    CustomGraphicsItems/StreetSignItem.cpp
    CustomGraphicsItems/NearfieldCellItem.cpp
    CustomGraphicsItems/InvisibleLineItem.cpp
    CustomGraphicsItems/CrossingAreaItem.cpp
    CustomGraphicsItems/NearfieldGridItem.cpp
    CustomGraphicsItems/QNavigationMarkerListWidgetItem.cpp
    CustomGraphicsItems/VisibleLineItem.cpp
    CustomGraphicsItems/NavigationMarkerItem.cpp
    CustomGraphicsItems/TreeNodeItem.cpp

    include/Navigation/Vector2.cpp
    include/Navigation/MapGraph.cpp
    include/Navigation/NavigationHelper.cpp
    include/Navigation/Helper.cpp

    include/Map/Node.cpp
    include/Map/Crossing.cpp
    include/Map/Lane.cpp
    include/Map/MapArea.cpp
    include/Map/PassableElement.cpp
    include/Map/ContentManager.cpp
    include/Map/SpecialMarking.cpp
    include/Map/LineMarking.cpp
    include/Map/StreetSign.cpp
    include/Map/XMLCore.cpp
    include/Map/NavigationMarker.cpp
    include/Map/Line.cpp

    GUIHelper/GraphicsViewZoom.cpp
    GUIHelper/ItemSignalController.cpp
    src/NearfieldGrid/MapTreeNode.cpp
)

target_link_libraries(UNIAutonomRemoteGUI Qt5::Widgets Qt5::Network ${OpenCV_LIBS})
