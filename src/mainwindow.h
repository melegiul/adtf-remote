#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "adtfmediasample.h"
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
    void openMapXML();
    void openPreferences();
    void connectNetwork();
    void disconnectNetwork();

    void networkConnected();
    void networkDisconnected();
    void networkReceived(ADTFMediaSample sample);
    void networkErrored(QString errorMsg);

    // testing / demo
    void sendMyADTFMessage(int counter);
private:
    Ui::MainWindow *ui;
    NetworkClient *networkClient;

// graphics import
public:
    void setCarOdometry(tCarOdometry &odo);
    void setTrapezoidCoords(tTrapezoid &coords);
    void setDetectedLine(tDetectedLine &detectedLine);
    void setNearfieldgridmap(tNearfieldGridMapArray &root);
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

signals:
    void nearfieldGridMapUpdated();
    void carUpdated();
    void trapezoidUpdated();
    void detectedLineUpdated();

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
    void focusOnCar();
    void updateStaticFilters(QTreeWidgetItem *item, int column);
    void updateDynamicFilters(QTreeWidgetItem *item, int column);
    void updateMousePosition(QPointF position);
    void setEditingMode(int state);
    void updateSignEditor(StreetSign *s, Lane *l);
    void updateSelectedSign(int state);
    void removeSelectedSign();
    void setSignRotations(double angle);
    void setupNearfieldGridMap();
    void deleteNavigationMarker();
    void deleteAllNavigationMarker();
    void updateMarkerTypesInMarkerList();
    void calculateRoute();
    void exportNavMarker();
    void importNavMarker();

private:
    QGraphicsScene *scene = nullptr;

    ContentManager &manager = ContentManager::getInstance();
    GraphicsViewZoom *zoom_rotate_manager = nullptr;
    tCarOdometry *odo = nullptr;
    tTrapezoid *coords = nullptr;
    tDetectedLine *detectedLine = nullptr;
    MapTreeNode *nearfieldgridmap = nullptr;
    // TODO
    int car_height = 523;
    int car_width = 250;
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

    StreetSign *selected_sign = nullptr;
    Lane *selected_lane = nullptr;
    std::shared_ptr<NavigationMarker> navigationMarker;
    NavigationMarkerItem *navMarkerItem = nullptr;
    QNavigationMarkerListWidgetItem *navMarkerListWidgetItem = nullptr;
};

#endif // __MAINWINDOW_H
