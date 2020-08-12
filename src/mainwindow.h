#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "adtfmediasample.h"
#include "ui_mainwindow.h"
#include "Map/ContentManager.hpp"

class NetworkClient;

// graphics import
class QTreeWidgetItem;
class QGraphicsScene;
class QGraphicsItem;
class QGraphicsPixmapItem;
class QGraphicsPolygonItem;

struct tCarOdometry;
struct tTrapezoid;
struct tDetectedLine;
//struct tNearfieldGridMapArray;
class StreetSign;
class Lane;
class Line;
class NavigationMarker;
class NavigationMarkerItem;
class Crossing;
class SpecialMarking;
class GraphicsViewZoom;
class MapTreeNode;
class QNavigationMarkerListWidgetItem;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openPreferences();
    void openLogAnalyser();
    void connectNetwork();
    void disconnectNetwork();
    void networkConnected();
    void networkDisconnected();
    void networkReceived(ADTFMediaSample sample);
    void networkErrored(QString errorMsg);

private:
    Ui::MainWindow *ui;
    NetworkClient *networkClient;

    //control tab related
    tState state = tState::NONE;
    QString fileNameMap = nullptr;
    QString fileNameCarConfig = nullptr;
    bool mapreceived = false;
    bool carconfselected = false;
    bool carconfreceived = false;
    bool routeinforreceived = false;
    bool initialization = false;
    bool ready = false;
    bool ad_running = false;
    bool rc_running = false;
    bool stopClick = false;
    bool emergency = false;

// graphics import
public:
    void setCarOdometry(tCarOdometry &odo);
    void setCarSpeed(tSpeed &speedy);
    void setTrapezoidCoords(tTrapezoid &coords);
    void setDetectedLine(std::shared_ptr<tDetectedLineArray> detectedLine);
    void setNearfieldgridmap(tNearfieldGridMapArray &root);
    void setCarState(tRemoteStateMsg &statemsg);
    void clearAndSetupStaticElements();
    void clearAll();
    void stop();
    void resume();
    void setCurrentMapName(const char *currentMapName);

private:
    void addSceneBuffer();
    void setupCar();
    void setupTrapezoid();
    void setupDetectedLine();
    void addLine(Line *line);
    void addSpecialMarking(SpecialMarking *marking);
    void addCrossingArea(Crossing *crossing);
    void addLane(Lane *lane);
    void colorLanesOfInterest() const;
    static void checkSubTree(QTreeWidgetItem *item, Qt::CheckState st);
    void checkAllTrees(Qt::CheckState st);
    void clearAllNavMarkerItems();
    void buildNavigationMarkerItemsFromXmlList();
    void processLogMsg(tLogMsg &logMsg);
    void processRemoteStateMsg(tRemoteStateMsg &rmtStateMsg);
    void resetControlTabVals();
    void resetMemberVariables();
    tCarConfigStruct prepareCarConfigStruct();
    void sendtSignalValueSteer();
    void sendtSignalValueSpeed();

signals:
    void nearfieldGridMapUpdated();
    void carUpdated();
    void trapezoidUpdated();
    void detectedLineUpdated();
    void guiUpdated();

public slots:
    void updateMap();

private slots:
    void addSign(StreetSign *sign, Lane *adjacent_lane);
    void addNavigationMarker(std::shared_ptr<NavigationMarker> &navMarker);
    void updateNavigationMarker(NavigationMarkerItem * navMarkerItem);
    void saveDialog();
    void toggleScoped();
    void updateCar();
    void updateTrapezoid();
    void updateDetectedLine();
    void updateControlTab();
    void focusOnCar();
    void updateStaticFilters(QTreeWidgetItem *item, int column);
    void updateDynamicFilters(QTreeWidgetItem *item, int column);
    void updateMousePosition(QPointF position);
    void setSignRotations(double angle);
    void setupNearfieldGridMap();
    void deleteNavigationMarker();
    void deleteAllNavigationMarker();
    void updateMarkerTypesInMarkerList();
    void calculateRoute();
    void exportNavMarker();
    void importNavMarker();
    void handleLogLevelSelection();
    void handleLogLevelACK();
    void handleMapPushClick();
    void handleMapPushACK();
    void handleCarConfigLoadClick();
    void handleCarConfigPushClick();
    void handleCarConfigPushACK();
    void handleRouteInfoPushClick();
    void handleRouteInfoPushACK();
    void handleStartADClick();
    void handleStartADACK();
    void handleStartRCClick();
    void handleStartRCACK();
    void handleStopClick();
    void handleStopACK();
    void handleAbortClick();
    void handleAbortACK();


private:
    QGraphicsScene *scene = nullptr;

    ContentManager &manager = ContentManager::getInstance();
    GraphicsViewZoom *zoom_rotate_manager = nullptr;
    tCarOdometry *odo = nullptr;
    tTrapezoid *coords = nullptr;
    std::shared_ptr<tDetectedLineArray> detectedLineArray = nullptr;
    MapTreeNode *nearfieldgridmap = nullptr;
    tFloat32 car_speed = 0.0f;
    int car_height = 400;
    int car_width = 240;
    tFloat32 car_init_x = 0.0f;
    tFloat32 car_init_y = 0.0f;
    tFloat32 car_init_orientation = 0.0f;
    tFloat32 car_pos_x = 0.0f;
    tFloat32 car_pos_y = 0.0f;
    tFloat32 car_orientation = 0.0f;
    bool scoped = false;
    bool show_active_lanes = true;
    bool show_trapezoid = true;
    bool show_detected_line = true;

    qreal cumulative_angle = 0;

    QGraphicsItem *static_filter = nullptr;
    QGraphicsItem *visual_filter = nullptr;
    QGraphicsItem *lines_filter = nullptr;
    QGraphicsItem *signs_filter = nullptr;
    QGraphicsItem *special_markings_filter = nullptr;
    QGraphicsItem *stop_lines_filter = nullptr;
    QGraphicsItem *crosswalks_filter = nullptr;
    QGraphicsItem *speed_bumps_filter = nullptr;
    QGraphicsItem *parking_filter = nullptr;
    QGraphicsItem *parking_regular_filter = nullptr;
    QGraphicsItem *parking_women_filter = nullptr;
    QGraphicsItem *parking_disabled_filter = nullptr;
    QGraphicsItem *parking_family_filter = nullptr;
    QGraphicsItem *parking_electric_filter = nullptr;
    QGraphicsItem *meta_filter = nullptr;
    QGraphicsItem *crossing_filter = nullptr;
    QGraphicsItem *invisible_lines_filter = nullptr;
    QGraphicsItem *lanes_filter = nullptr;
    QGraphicsItem *lane_direction_filter = nullptr;
    QGraphicsItem *navMarker_filter = nullptr;
    QGraphicsPixmapItem *car_filter = nullptr;
    QGraphicsPolygonItem *trapezoid_filter = nullptr;
    QGraphicsItem *detected_line_filter = nullptr;
    QGraphicsItem *nearfield_map_filter = nullptr;

    Lane *selected_lane = nullptr;
    std::shared_ptr<NavigationMarker> navigationMarker;
    NavigationMarkerItem *navMarkerItem = nullptr;
    QNavigationMarkerListWidgetItem *navMarkerListWidgetItem = nullptr;


};

#endif // __MAINWINDOW_H
