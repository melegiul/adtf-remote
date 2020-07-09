cmake_minimum_required(VERSION 3.1.0)
project(autonomous_controller)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
find_package(ADTF REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(.)
include_directories(include)
include_directories(/usr/include/opencv4)

adtf_add_executable(AutonomousController
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    dialog_preferences.cpp
    dialog_preferences.ui

    adtfmediasample.h
    networkclient.cpp

    ../res/res.qrc

    QtWidgets/bottomleftgraphicsview.cpp
    QtWidgets/resizegraphicsview.cpp

    adtf_converters/carodometry.cpp
    adtf_converters/trapezoid.cpp
    adtf_converters/detectedline.cpp
    adtf_converters/nearfieldgridmap.cpp

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

    Navigation/Vector2.cpp
    Navigation/MapGraph.cpp
    Navigation/NavigationHelper.cpp
    Navigation/Helper.cpp

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

target_link_libraries(AutonomousController Qt5::Widgets Qt5::Network ${OpenCV_LIBS})
