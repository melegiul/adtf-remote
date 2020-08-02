#include <vector>
#include <memory>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QSettings>

#include <QGraphicsItem>
#include <adtf_converters/mediaDesciptionSingleton.h>

#include "mainwindow.h"
#include "dialog_preferences.h"
#include "networkclient.h"
#include "adtf_converters/carodometry.h"
#include "adtf_converters/trapezoid.h"
#include "adtf_converters/detectedline.h"
#include "adtf_converters/nearfieldgridmap.h"
#include "adtf_converters/remoteStateMsg.h"
#include "adtf_converters/logMsg.h"
#include "adtf_converters/speed.h"
#include "adtf_converters/mediaDesciptionSingleton.h"

#include "Map/ContentManager.hpp"
#include "CustomGraphicsItems/TreeNodeItem.h"
#include "CustomGraphicsItems/NavigationMarkerItem.h"
#include "CustomGraphicsItems/LaneDirectionItem.h"
#include "CustomGraphicsItems/CrossingAreaItem.h"
#include "CustomGraphicsItems/QNavigationMarkerListWidgetItem.h"
#include "src/NearfieldGrid/MapTreeNode.h"
#include "CustomGraphicsItems/NearfieldMapItem.h"
#include "CustomGraphicsItems/StreetSignItem.h"
#include "CustomGraphicsItems/VisibleLineItem.h"
#include "CustomGraphicsItems/InvisibleLineItem.h"
#include "GUIHelper/GraphicsViewZoom.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), networkClient(new NetworkClient(this))
{
    QSettings settings;
    int ui_background = settings.value("ui/background", 180).toInt();
    QSettings carSettings(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString(), QSettings::IniFormat);

    mediaDesciptionSingleton::getInstance().loadDescription(settings.value("description/path", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/description/uni_autonom.description").toString().toStdString());
    ui->setupUi(this);

    connect(ui->map_view, &ResizeGraphicsView::resized, ui->measure_bar, &BottomLeftGraphicsView::resize);

    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferences);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectNetwork);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectNetwork);

    connect(this->networkClient, &NetworkClient::connected, this, &MainWindow::networkConnected);
    connect(this->networkClient, &NetworkClient::disconnected, this, &MainWindow::networkDisconnected);
    connect(this->networkClient, &NetworkClient::received, this, &MainWindow::networkReceived);
    connect(this->networkClient, &NetworkClient::errored, this, &MainWindow::networkErrored);

    connect(this, SIGNAL(guiUpdated()), this, SLOT(updateControlTab()));
    connect(ui->loglevel_combo, SIGNAL(currentTextChanged(QString)), this, SLOT(handleLogLevelSelection()));
    connect(ui->map_load_button, SIGNAL(clicked()), this, SLOT(handleMapPushClick()));
    connect(ui->car_config_load_button, SIGNAL(clicked()), this, SLOT(handleCarConfigLoadClick()));
    connect(ui->car_config_push_button, SIGNAL(clicked()), this, SLOT(handleCarConfigPushClick()));
    connect(ui->navi_route_push_button, SIGNAL(clicked()), this, SLOT(handleRouteInfoPushClick()));
    connect(ui->start_ad_button, SIGNAL(clicked()), this, SLOT(handleStartADClick()));
    connect(ui->start_rc_button, SIGNAL(clicked()), this, SLOT(handleStartRCClick()));
    connect(ui->running_stop_button, SIGNAL(clicked()), this, SLOT(handleStopClick()));
    connect(ui->abort_button, SIGNAL(clicked()), this, SLOT(handleAbortClick()));

    // view tab related code
    ui->tabWidget->setCurrentIndex(0);
    ui->dynamic_tree->expandAll();
    ui->static_tree->expandAll();
    ui->map_view->setBackgroundBrush(QBrush(QColor(ui_background, ui_background, ui_background)));
    ui->map_view->setFocusPolicy(Qt::NoFocus);
    ui->map_view->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->map_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->map_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->drivingTasksListWidget->setVerticalScrollBarPolicy((Qt::ScrollBarAsNeeded));
    scene = new QGraphicsScene();
    ui->map_view->setScene(scene);

    clearAndSetupStaticElements();

    zoom_rotate_manager = new GraphicsViewZoom(ui->map_view, ui->compass_rose, ui->measure_bar);
    zoom_rotate_manager->setEnabled(true);
    connect(zoom_rotate_manager, SIGNAL(zoomed()), this, SLOT(focusOnCar()));
    connect(zoom_rotate_manager, SIGNAL(rotated(double)), this, SLOT(setSignRotations(double)));
    connect(zoom_rotate_manager, SIGNAL(rotated(double)), this, SLOT(focusOnCar()));
    connect(zoom_rotate_manager, SIGNAL(mousePositionUpdated(QPointF)), this, SLOT(updateMousePosition(QPointF)));


    connect(this, SIGNAL(carUpdated()), this, SLOT(updateCar()));
    connect(this, SIGNAL(nearfieldGridMapUpdated()),this,SLOT(setupNearfieldGridMap()));
    connect(this, SIGNAL(trapezoidUpdated()), this, SLOT(updateTrapezoid()));
    connect(this, SIGNAL(detectedLineUpdated()), this, SLOT(updateDetectedLine()));

    connect(ui->scope_button, SIGNAL(clicked()), this, SLOT(toggleScoped()));
    connect(ui->actionExportMap, SIGNAL(triggered()), this, SLOT(saveDialog()));
    connect(ui->exportNavMarkerButton, SIGNAL(clicked()), this, SLOT(exportNavMarker()));
    connect(ui->importNavMarkerButton, SIGNAL(clicked()), this, SLOT(importNavMarker()));
    connect(ui->static_tree, SIGNAL(itemChanged(QTreeWidgetItem * , int)), this,
            SLOT(updateStaticFilters(QTreeWidgetItem * , int)));
    connect(ui->dynamic_tree, SIGNAL(itemChanged(QTreeWidgetItem * , int)), this,
            SLOT(updateDynamicFilters(QTreeWidgetItem * , int)));
    connect(ui->removeNavigationMarkerButton, SIGNAL(clicked()), this, SLOT(deleteNavigationMarker()));
    connect(ui->removeAllNavigationMarkerButton, SIGNAL(clicked()), this, SLOT(deleteAllNavigationMarker()));
    connect(ui->calculateRouteButton, SIGNAL(clicked()), this, SLOT(calculateRoute()));

    ItemSignalController &cont = ItemSignalController::getInstance();
    connect(&cont, SIGNAL(updateMap()), this, SLOT(updateMap()));
    connect(&cont, SIGNAL(addNavigationMarkerClicked(std::shared_ptr<NavigationMarker> & )), this, SLOT(addNavigationMarker(std::shared_ptr<NavigationMarker> & )));
    connect(&cont, SIGNAL(markerMoved(NavigationMarkerItem *)), this, SLOT(updateNavigationMarker(NavigationMarkerItem *)));
    connect(&cont, SIGNAL(markerUpdateRequired()), this, SLOT(updateMarkerTypesInMarkerList()));
    connect(&cont, SIGNAL(deleteMarker()), this, SLOT(deleteNavigationMarker()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openPreferences() {
    PreferencesDialog prefDialog(this);
    prefDialog.exec();
}

void MainWindow::connectNetwork()
{
    QSettings settings;
    QString host = settings.value("network/ipaddress").toString();
    uint16_t port = settings.value("network/port").toUInt();

    this->networkClient->connectNetwork(host, port);
}

void MainWindow::disconnectNetwork()
{
    //send RemoteControlSignal Abort
    ADTFMediaSample sample;
    tRemoteCommandMsg command = tRemoteCommandMsg(tRemoteControlSignal::ABORT, tFilterLogType::NONE, 0);
    sample.length = sizeof(command);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "tRemoteCommandMsg";
    sample.mediaType = "tRemoteCommandMsg";
    sample.streamTime = 0;
    memcpy(sample.data.get(), &command, sample.length);
    this->networkClient->send(sample);

    this->networkClient->disconnectNetwork();
}

void MainWindow::networkConnected() {
    this->state = tState::INITIALIZATION;
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->statusbar->showMessage("Connected to " + this->networkClient->getPeer());
    ui->connection_val_label->setText(QString::fromStdString("connected"));
    ui->connection_val_label->setStyleSheet("QLabel { background-color : green}");
    ui->loglevel_combo->setEnabled(true);
    emit(guiUpdated());
}

void MainWindow::networkDisconnected() {
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->connection_val_label->setText(QString::fromStdString("not connected"));
    ui->connection_val_label->setStyleSheet("QLabel { background-color : red}");
    ui->loglevel_combo->setEnabled(false);
    resetControlTabVals();
    emit(guiUpdated());
}

void MainWindow::networkReceived(ADTFMediaSample sample)
{
    if (sample.pinName == "tCarOdometryOut" && sample.mediaType == "tCarOdometry") {
        tCarOdometry odometry = adtf_converter::from_network::carOdometry(sample);
        this->setCarOdometry(odometry);
    } else if (sample.pinName == "tTrapezoidOut" && sample.mediaType == "tTrapezoid") {
        tTrapezoid trapezoid = adtf_converter::from_network::trapezoid(sample);
        this->setTrapezoidCoords(trapezoid);
    } else if (sample.pinName == "tDetectedLineArrayOut" && sample.mediaType == "tDetectedLineArray") {
        std::vector<tDetectedLine> lines = adtf_converter::from_network::detectedLineArray(sample);
        std::unique_ptr<tDetectedLineArray> tDetLines = adtf_converter::from_network::detectedLine(lines);
        this->setDetectedLine(std::move(tDetLines));
    } else if (sample.pinName == "tNearfieldGridMapArrayOut" && sample.mediaType == "tNearfieldGridMapArray") {
        std::unique_ptr<tNearfieldGridMapArray> nearfieldGrid = adtf_converter::from_network::nearfieldGridmap(sample);
        this->setNearfieldgridmap(*nearfieldGrid);
    } else if (sample.pinName == "tRemoteStateMsgOut" && sample.mediaType == "tRemoteStateMsg") {
        std::unique_ptr<tRemoteStateMsg> statemsg = adtf_converter::from_network::remoteStateMsg(sample);
        this->setCarState(*statemsg);
        this->processRemoteStateMsg(*statemsg);
    } else if (sample.pinName == "tLogMsgOut" && sample.mediaType == "tLogMsg") {
        std::unique_ptr<tLogMsg> logmsg = adtf_converter::from_network::logMsg(sample);
        this->processLogMsg(*logmsg);
    } else if (sample.pinName == "tSpeedOut" && sample.mediaType == "tSpeed") {
        tSpeed speedy = adtf_converter::from_network::speed(sample);
        this->setCarSpeed(speedy);
    }
}

void MainWindow::networkErrored(QString errorMsg) {
    ui->statusbar->showMessage(errorMsg);
}

void MainWindow::processRemoteStateMsg(tRemoteStateMsg & rmtStateMsg){
    if(rmtStateMsg.state == tState::INITIALIZATION) {
        handleAbortACK();
    } else if (rmtStateMsg.state == tState::READY) {
        if(stopClick){
            handleStopACK();
            stopClick = false;
        }else{
            handleCarConfigPushACK();
        }
    } else if (rmtStateMsg.state == tState::AD_RUNNING) {
        handleStartADACK();
    } else if (rmtStateMsg.state == tState::RC_RUNNING) {
        handleStartRCACK();
    } else if (rmtStateMsg.state == tState::EMERGENCY) {
        //TODO
    } else {
        //default case
    }
}

void MainWindow::processLogMsg(tLogMsg &logMsg){
    if(logMsg.filterLogType == tFilterLogType::ACK){
        if(logMsg.uniaFilter == tUniaFilter::STATEMACHINE){
            if(logMsg.logContext == tLogContext::NONE){
                handleLogLevelACK();
            } else if(logMsg.logContext == tLogContext::MAP){
                handleMapPushACK();
            } else if(logMsg.logContext == tLogContext::AC){
                handleCarConfigPushACK();
            } else if(logMsg.logContext == tLogContext::RI){
                handleRouteInfoPushACK();
            }
        }
    }
    qDebug() << "[" << logMsg.timestamp << "] [" << QString::fromStdString(tFilterLogTypeMap.find(logMsg.filterLogType)->second) << "] [" << QString::fromStdString(tUniaFilterMap.find(logMsg.uniaFilter)->second) << "] [" << QString::fromStdString(tLogContextMap.find(logMsg.logContext)->second) << "] Payload -> " << QString::fromStdString(std::string(reinterpret_cast<char const *>(logMsg.ui8Data), logMsg.payloadLength));
    //TODO handle log in general
}


// import from old widget code
void MainWindow::updateStaticFilters(QTreeWidgetItem *item, int column) {

    if (item->text(0) == "Visual") visual_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Street Lines") lines_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Street Signs") signs_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Special Markings") special_markings_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Stop Lines") stop_lines_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Crosswalks") crosswalks_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Speed Bumps") speed_bumps_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Parking Spots") parking_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Regular") parking_regular_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Disabled") parking_disabled_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Women") parking_women_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Family") parking_family_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Electric Vehicles") parking_electric_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Meta") meta_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Crossing Areas") crossing_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Invisible Street Lines") invisible_lines_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Street Areas") lanes_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Direction of Traffic") lane_direction_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Navigation Marker") navMarker_filter->setVisible(item->checkState(column));

    // needed to resolve a bug within QT: if a parent node is selected when every child was unselected by hand,
    // events are only generated for children, not for the parent -> parents stays invisible
    if (item->text(0) == "Crossing Areas" && item->checkState(column) == 2) meta_filter->setVisible(true);
    if (item->text(0) == "Regular" && item->checkState(column) == 2) parking_filter->setVisible(true);
    if (item->text(0) == "Stop Lines" && item->checkState(column) == 2) special_markings_filter->setVisible(true);
    if (item->text(0) == "Street Lines" && item->checkState(column) == 2) visual_filter->setVisible(true);
}


void MainWindow::updateDynamicFilters(QTreeWidgetItem *item, int column) {
    if (car_filter == nullptr) setupCar();
    if (trapezoid_filter == nullptr) setupTrapezoid();
    if (detected_line_filter == nullptr) setupDetectedLine();
    if (item->text(0) == "Car") car_filter->setVisible(item->checkState(column));
    //FIXME when we are back in car context, nearfieldgridmap needs to be visualized
    else if (item->text(0) == "Nearfieldgridmap") return;//nearfield_map_filter->setVisible(item->checkState(column));
    else if (item->text(0) == "Active Lane") {
        show_active_lanes = item->checkState(column);
        colorLanesOfInterest();

    } else if (item->text(0) == "Trapezoid") {
        show_trapezoid = item->checkState(column);
        if (trapezoid_filter)
            trapezoid_filter->setVisible(item->checkState(column));
    } else if (item->text(0) == "Detected Lines") {
        show_detected_line = item->checkState(column);
        updateDetectedLine();
    }
}


void MainWindow::clearAll() {
    if (static_filter != nullptr) {
        scene->removeItem(static_filter);
        delete static_filter;
        static_filter = nullptr;
    }
    if (nearfield_map_filter != nullptr){
        scene->removeItem(nearfield_map_filter);
        delete nearfield_map_filter;
        nearfield_map_filter = nullptr;
    }
    if (trapezoid_filter != nullptr) {
        scene->removeItem(trapezoid_filter);
        delete trapezoid_filter;
        trapezoid_filter = nullptr;
    }
    if (detected_line_filter != nullptr) {
        scene->removeItem(detected_line_filter);
        delete detected_line_filter;
        detected_line_filter = nullptr;
    }
    if (car_filter != nullptr) {
        scene->removeItem(car_filter);
        delete car_filter;
        car_filter = nullptr;
    }

    ui->drivingTasksListWidget->clear();
    ui->navigationMarkerListWidget->clear();
    NavigationMarker::resetIds();
    if(navMarker_filter != nullptr){
        scene->removeItem(navMarker_filter);
        delete navMarker_filter;
        navMarker_filter = nullptr;
    }
}


void MainWindow::clearAndSetupStaticElements() {
    if (static_filter != nullptr) {
        scene->removeItem(static_filter);
        delete static_filter;
    }
    static_filter = new TreeNodeItem();
    visual_filter = new TreeNodeItem(static_filter);
    meta_filter = new TreeNodeItem(static_filter);
    lines_filter = new TreeNodeItem(visual_filter);
    signs_filter = new TreeNodeItem(visual_filter);
    special_markings_filter = new TreeNodeItem(visual_filter);
    stop_lines_filter = new TreeNodeItem(special_markings_filter);
    crosswalks_filter = new TreeNodeItem(special_markings_filter);
    speed_bumps_filter = new TreeNodeItem(special_markings_filter);
    parking_filter = new TreeNodeItem(special_markings_filter);
    parking_regular_filter = new TreeNodeItem(parking_filter);
    parking_women_filter = new TreeNodeItem(parking_filter);
    parking_disabled_filter = new TreeNodeItem(parking_filter);
    parking_family_filter = new TreeNodeItem(parking_filter);
    parking_electric_filter = new TreeNodeItem(parking_filter);
    crossing_filter = new TreeNodeItem(meta_filter);
    invisible_lines_filter = new TreeNodeItem(meta_filter);
    lanes_filter = new TreeNodeItem(meta_filter);
    lane_direction_filter = new TreeNodeItem(meta_filter);
    navMarker_filter = new TreeNodeItem(meta_filter);

    // z-value is only relevant in relation with sibling items
    meta_filter->setZValue(-1);
    visual_filter->setZValue(0);

    lanes_filter->setZValue(-1);
    crossing_filter->setZValue(1);
    lane_direction_filter->setZValue(2);

    special_markings_filter->setZValue(1);
    signs_filter->setZValue(2);

    navMarker_filter->setZValue(3);

    scene->addItem(static_filter);
}


void MainWindow::updateMap() {
    QRectF scene_rect = scene->itemsBoundingRect();

    clearAndSetupStaticElements();

    for (auto el: manager.line_map) {
        addLine(el.second);
    }
    for (auto el: manager.s_marking_map) {
        addSpecialMarking(el.second);
    }
    for (auto el: manager.lane_map) {
        addLane(el.second);
        for (auto sign: el.second->signs) {
            addSign(sign, el.second);
        }
    }
    for (auto el: manager.crossing_map) {
        addCrossingArea(el.second);
    }

    if (scene_rect != scene->itemsBoundingRect()) {
        addSceneBuffer();
        ui->map_view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        zoom_rotate_manager->adjustMeasure();
    }
    colorLanesOfInterest();
}


// adds empty space around the current scene to enable free dragging
void MainWindow::addSceneBuffer() {
    qreal scene_padding = 2 * std::max(scene->itemsBoundingRect().width(), scene->itemsBoundingRect().height());
    QPointF top_left = scene->itemsBoundingRect().topLeft() + QPointF(-scene_padding, -scene_padding);
    QPointF bottom_right = scene->itemsBoundingRect().bottomRight() + QPointF(scene_padding, scene_padding);
    ui->map_view->setSceneRect(QRectF(top_left, bottom_right));
}

void MainWindow::setupCar() {
    if (car_filter != nullptr) {
        scene->removeItem(car_filter);
        delete car_filter;
    }
    QPixmap car(":/car.png");
    car = car.scaled(car_width, car_height);
    car_filter = new QGraphicsPixmapItem(car);
    car_filter->setTransformOriginPoint(car_filter->boundingRect().center());
    car_filter->setZValue(2); // put car on top
    scene->addItem(car_filter);

    //FIXME for future use in car context
    //setupNearfieldGridMap();
}

//Setup the trapezoid by creating a QGraphicsPolygonItem and set properties
void MainWindow::setupTrapezoid() {
    if (this->coords == nullptr)
        return;

    if (trapezoid_filter != nullptr) {
        scene->removeItem(trapezoid_filter);
        delete trapezoid_filter;
    }

    trapezoid_filter = new QGraphicsPolygonItem(QPolygonF(QVector<QPointF>() << QPointF(coords->pos_A.x, coords->pos_A.y)
                                                            << QPointF(coords->pos_B.x, coords->pos_B.y)
                                                            << QPointF(coords->pos_C.x, coords->pos_C.y)
                                                            << QPointF(coords->pos_D.x, coords->pos_D.y)));

    trapezoid_filter->setPen(QPen(Qt::green, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    trapezoid_filter->setZValue(1000);
    scene->addItem(trapezoid_filter);
}
//Setup the detected Lines and add it to the scene
void MainWindow::setupDetectedLine() {
    if (detectedLineArray == nullptr)
        return;

    if (detected_line_filter != nullptr) {
        scene->removeItem(detected_line_filter);
        delete detected_line_filter;
    }

    if (detectedLineArray->arraySize >= 0 && show_detected_line) {

        for (int i = 0; i < detectedLineArray->arraySize; i++) {

            QLineF line = QLineF((qreal) detectedLineArray->detectedLines[i].fromPos.x, (qreal) detectedLineArray->detectedLines[i].fromPos.y,
                                 (qreal) detectedLineArray->detectedLines[i].toPos.x, (qreal) detectedLineArray->detectedLines[i].toPos.y);


            QGraphicsLineItem *lineItem = new QGraphicsLineItem(line, detected_line_filter);

            lineItem->setPen(QPen(Qt::red, 40, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            lineItem->setZValue(1);
            scene->addItem(lineItem);
        }
    }
}


void MainWindow::updateCar() {
    if(carconfreceived){
        if(car_filter == nullptr) setupCar();
        if(odo == nullptr) return;

        auto angle = odo->orientation * 180 / M_PI;

        if (angle < -0.5) angle += 360;

        car_filter->setPos(odo->pos.x - car_width / 2., odo->pos.y - car_height / 2.);
        car_filter->setRotation(-angle);
        if (show_active_lanes) colorLanesOfInterest();

        focusOnCar();
    }
}

//Update the position of the trapezoid
void MainWindow::updateTrapezoid() {
    if (trapezoid_filter == nullptr) setupTrapezoid();
    if (coords == nullptr) return;

    trapezoid_filter->setPolygon(QPolygonF(QVector<QPointF>() << QPointF(coords->pos_A.x,  coords->pos_A.y)
                                                              << QPointF(coords->pos_B.x, coords->pos_B.y)
                                                              << QPointF(coords->pos_C.x, coords->pos_C.y)
                                                              << QPointF(coords->pos_D.x, coords->pos_D.y)));
}
//Update detected Lines. First remove all Lines then add the new detected lines
void MainWindow::updateDetectedLine() {
    if (detected_line_filter == nullptr && show_detected_line) setupDetectedLine();
    if (detectedLineArray == nullptr) return;


    if (detectedLineArray->arraySize >= 0) {

            foreach (QGraphicsItem *item, scene->items()) {
                QGraphicsLineItem *lineItem = dynamic_cast<QGraphicsLineItem*>(item);
                if (lineItem != nullptr && lineItem->parentItem() == detected_line_filter) scene->removeItem(lineItem);
            }

        if (show_detected_line) {
            for (int i = 0; i < detectedLineArray->arraySize; i++) {

                QLineF line = QLineF((qreal) detectedLineArray->detectedLines[i].fromPos.x,
                                     (qreal) detectedLineArray->detectedLines[i].fromPos.y,
                                     (qreal) detectedLineArray->detectedLines[i].toPos.x,
                                     (qreal) detectedLineArray->detectedLines[i].toPos.y);


                QGraphicsLineItem *lineItem = new QGraphicsLineItem(line, detected_line_filter);

                lineItem->setPen(QPen(Qt::red, 40, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                lineItem->setZValue(500);
                scene->addItem(lineItem);
            }
        }
    }
}

// "lanes of interest" are 1. lanes on which the car is currently driving, and 2. lanes that belong to a selected sign
void MainWindow::colorLanesOfInterest() const {
    if (lanes_filter == nullptr || static_filter == nullptr || car_filter == nullptr) return;
    for (auto item: lanes_filter->childItems()) {
        auto item_cast = qgraphicsitem_cast<LaneItem *>(item);
        if (item_cast != nullptr) {
            if(/*ui->editor_checkbox->isChecked() &&*/ item_cast->isEqual(selected_lane)){
                item_cast->colorSelected();
            } else {
                item_cast->colorRegular();
            }
        }
    }

    if (show_active_lanes) {
        auto list_of_items = scene->collidingItems(car_filter);
        for (auto item : list_of_items) {
            auto lane = qgraphicsitem_cast<LaneItem *>(item);
            if (lane != nullptr) {
                if(!lane->isEqual(selected_lane) /*|| !ui->editor_checkbox->isChecked()*/) {
                    lane->colorCarOnTop();
                }
            }
        }
    }
}


void MainWindow::toggleScoped() {
    scoped = !scoped;
    if (scoped) {
        ui->scope_button->setText("Scope Car: Active");
        ui->map_view->setDragMode(QGraphicsView::NoDrag);
        focusOnCar();
    } else {
        ui->scope_button->setText("Scope Car: Inactive");
        ui->map_view->setDragMode(QGraphicsView::ScrollHandDrag);
    }
}


void MainWindow::focusOnCar() {
    if (scoped && odo != nullptr) {
        QTransform trans = ui->map_view->transform();
        qreal x_scale = sqrt(trans.m11() * trans.m11() + trans.m12() * trans.m12());
        qreal y_scale = sqrt(trans.m21() * trans.m21() + trans.m22() * trans.m22());
        qreal rotation_rad = odo->orientation + M_PI;
        qreal rotation_deg = rotation_rad * 180 / M_PI;

        QTransform trans_new(x_scale, 0, 0, 0, y_scale, 0, 0, 0);
        trans_new.rotateRadians(rotation_rad);
        ui->map_view->setTransform(trans_new);
        ui->map_view->centerOn(odo->pos.x, odo->pos.y);

        QTransform t_rose;
        t_rose.rotate(rotation_deg);
        ui->compass_rose->setTransform(t_rose);

        if (cumulative_angle != -rotation_deg) {
            setSignRotations(cumulative_angle + rotation_deg);
            cumulative_angle = -rotation_deg;
        }
    }
}

void MainWindow::addLine(Line *line) {
    int num_markings = static_cast<int>(line->markings.size());
    if (num_markings == 0) {
        // lines without markings drawn as semi-transparent dotted lines
        new InvisibleLineItem(line, car_width / 6, invisible_lines_filter);
    } else if (num_markings == 1) {
        new VisibleLineItem(line->markings.front(), line, 0, car_width / 6, lines_filter);
    } else {
        int parallel_distance = car_width / 3;
        int offset = -parallel_distance * (num_markings - 1) / 2;
        for (auto mar: line->markings) {
            // Using car width to scale line width dynamically
            new VisibleLineItem(mar, line, offset, car_width / 6, lines_filter);
            offset += parallel_distance;
        }
    }
}

void MainWindow::addSpecialMarking(SpecialMarking *marking) {
    auto area = manager.area_map.find(marking->getArea());
    if (area != manager.area_map.end()) {
        QPolygon outline;
        for (auto el2: area->second->outline) {
            outline.append(QPoint(el2->x, el2->y));
        }
        auto *item = new SpecialMarkingItem(marking, outline);

        if (marking->type == eSpecialMarking::stopLine)
            item->setParentItem(stop_lines_filter);
        else if (marking->type == eSpecialMarking::crossWalk)
            item->setParentItem(crosswalks_filter);
        else if (marking->type == eSpecialMarking::speedBump)
            item->setParentItem(speed_bumps_filter);
        else if (marking->type == eSpecialMarking::ParkingRegular)
            item->setParentItem(parking_regular_filter);
        else if (marking->type == eSpecialMarking::ParkingWomen)
            item->setParentItem(parking_women_filter);
        else if (marking->type == eSpecialMarking::ParkingFamily)
            item->setParentItem(parking_family_filter);
        else if (marking->type == eSpecialMarking::ParkingElectric)
            item->setParentItem(parking_electric_filter);
        else if (marking->type == eSpecialMarking::ParkingDisabled)
            item->setParentItem(parking_disabled_filter);
    }
}

void MainWindow::addSign(StreetSign *sign, Lane *adjacent_lane) {
    auto *item = new StreetSignItem(sign, adjacent_lane, car_width, signs_filter);
    item->setTransform(QTransform::fromTranslate(item->pixmap().width() / 2., item->pixmap().height() / 2.), true);
    item->setRotation(item->rotation() + cumulative_angle); // ignore rotation of scene
    item->setTransform(QTransform::fromTranslate(-item->pixmap().width() / 2., -item->pixmap().height() / 2.), true);
}

void MainWindow::addCrossingArea(Crossing *crossing) {
    auto area = manager.area_map.find(crossing->area_id);
    if (area != manager.area_map.end()) {
        new CrossingAreaItem(crossing, area->second, crossing_filter);
    }
}

void MainWindow::addLane(Lane *lane) {
    new LaneItem(lane, lanes_filter);

    auto left = manager.line_map.find(lane->getLeft());
    auto right = manager.line_map.find(lane->getRight());
    if (left != manager.line_map.end() && right != manager.line_map.end() && !(left->second->markings.empty())) {
        //using car size to scale arrows;
        new LaneDirectionItem(left->second, right->second, car_width, lane_direction_filter);
    }
}


void MainWindow::updateMousePosition(QPointF position) {
    ui->mouse_x->setText(QString::fromStdString(std::to_string((int) std::round(position.x()))));
    ui->mouse_y->setText(QString::fromStdString(std::to_string((int) std::round(position.y()))));
}


void MainWindow::saveDialog() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString strFile = dialog.getSaveFileName(nullptr, "Save as XML", "", tr("XML Files (*.xml)"));
    ContentManager::getInstance().exportToXml(strFile.toLocal8Bit().data());
}


void MainWindow::setCarOdometry(tCarOdometry &odom) {
    // ensures that carUpdate is called within Widget (only Widget is allowed to update the map)
    if(ready || ad_running || rc_running){
        this->odo = &odom;
        emit(carUpdated());
        emit(guiUpdated());
    }
}

void MainWindow::setCarSpeed(tSpeed &speedy) {
    // ensures that carUpdate is called within Widget (only Widget is allowed to update the map)
    if(ready || ad_running || rc_running){
        this->speed = &speedy;
    }
}


void MainWindow::setTrapezoidCoords(tTrapezoid &coordis) {
    if(ready || ad_running || rc_running) {
        this->coords = &coordis;
        //qDebug()  << "global trapezoid " << coords->pos_A.x << " " << coords->pos_A.y << " " << coords->pos_B.x << " " << coords->pos_B.y << " " << coords->pos_C.x << " " << coords->pos_C.y << " " << coords->pos_D.x << " " << coords->pos_D.y ;
        emit(trapezoidUpdated());
    }
}

void MainWindow::setDetectedLine(std::shared_ptr<tDetectedLineArray> detectedLineArray) {
    if(ready || ad_running || rc_running) {
        this->detectedLineArray = detectedLineArray;
        emit(detectedLineUpdated());
    }
}

void MainWindow::setNearfieldgridmap(tNearfieldGridMapArray &root) {
    if(ready || ad_running || rc_running) {
        this->nearfieldgridmap = MapTreeNode::generateFromArray(&root);
        if (this->nearfieldgridmap == nullptr) {
            //printw("Nullptr\n");
            this->nearfieldgridmap = new MapTreeNode(1);
        }
        emit(nearfieldGridMapUpdated());
    }
}

void MainWindow::setCarState(tRemoteStateMsg &statemsg) {
    state = statemsg.state;
    emit(guiUpdated());
}


void MainWindow::setSignRotations(double angle) {
    for (auto sign: signs_filter->childItems()) {
        auto sign_c = qgraphicsitem_cast<StreetSignItem *>(sign);
        sign->setTransform(QTransform::fromTranslate(sign_c->pixmap().width() / 2., sign_c->pixmap().height() / 2.), true);
        sign->setRotation(sign->rotation() - angle);
        sign->setTransform(QTransform::fromTranslate(-sign_c->pixmap().width() / 2., -sign_c->pixmap().height() / 2.), true);
    }
    cumulative_angle -= angle;
}

void MainWindow::checkSubTree(QTreeWidgetItem *item, Qt::CheckState st) {
    item->setCheckState(0, st);
    for(int i=0;i<item->childCount();i++)
        checkSubTree(item->child(i), st);
}

void MainWindow::checkAllTrees(Qt::CheckState st) {
    //these signals would result in SegFaults, because this method can be called before the filters are updated
    ui->static_tree->blockSignals(true);
    ui->dynamic_tree->blockSignals(true);
    for(int i=0;i<ui->static_tree->topLevelItemCount();i++){
        checkSubTree(ui->static_tree->topLevelItem(i), st);
    }
    for(int i=0;i<ui->dynamic_tree->topLevelItemCount();i++){
        checkSubTree(ui->dynamic_tree->topLevelItem(i), st);
    }
    show_active_lanes = true;
    show_trapezoid = true;
    show_detected_line = true;
    ui->static_tree->blockSignals(false);
    ui->dynamic_tree->blockSignals(false);
}

void MainWindow::addNavigationMarker(std::shared_ptr<NavigationMarker> &navMarker) {

    NavigationMarkerItem *navMarkerItem = new NavigationMarkerItem(navMarker, navMarker_filter);
    manager.addNavigationMarkerToList(navMarker);
    navMarkerItem->setToolTipMarker();
    string markerTitle = "Marker: ";
    markerTitle.append(std::to_string(navMarker->getId()));
    QString markerString = QString::fromStdString(markerTitle);
    QNavigationMarkerListWidgetItem *labelItem = new QNavigationMarkerListWidgetItem(markerString, ui->navigationMarkerListWidget,
                                                                       navMarkerItem);

    ui->navigationMarkerListWidget->addItem(labelItem);

    //if there is more than 1 navigation marker specified keep track of which marker is final and which are intermediate
    if (manager.navigationMarkerList.size() >= 1) {
        updateMarkerTypesInMarkerList();
    }
}

void MainWindow::updateMarkerTypesInMarkerList() {
    if(ui->navigationMarkerListWidget->count() > 0){
        int rowCount = ui->navigationMarkerListWidget->count();
        for(int i = 0; i < (rowCount - 1); i++ ){
            QNavigationMarkerListWidgetItem *item = dynamic_cast<QNavigationMarkerListWidgetItem *> (ui->navigationMarkerListWidget->item(i));
            item->setENavType(Intermediate);
        }
        navMarkerListWidgetItem = dynamic_cast<QNavigationMarkerListWidgetItem *>(ui->navigationMarkerListWidget->item(rowCount - 1));
        if(navMarkerListWidgetItem != nullptr){
            navMarkerListWidgetItem->setENavType(Final);
        }
    }
}

void MainWindow::updateNavigationMarker(NavigationMarkerItem * navMarkerItem1){
    //if a navigation marker is dragged onto a "Non-PassableElement" it is set back to its start position
    if(!(navMarkerItem1->isChanged())){
        QPointF point = QPointF(navMarkerItem1->getX(),navMarkerItem1->getY());
        navMarkerItem1->setPos(point);
    }
}

void MainWindow::deleteNavigationMarker(){
    navMarkerListWidgetItem = dynamic_cast<QNavigationMarkerListWidgetItem *>(ui->navigationMarkerListWidget->takeItem(
            ui->navigationMarkerListWidget->currentRow()));
    if(navMarkerListWidgetItem != nullptr) {
        navMarkerItem = navMarkerListWidgetItem->getNavMarkerItem();
        navigationMarker = navMarkerItem->getNavMarker();
        scene->removeItem(navMarkerItem);
        manager.navigationMarkerList.remove(navigationMarker);
        delete navMarkerListWidgetItem;
        delete navMarkerItem;
        navMarkerListWidgetItem = nullptr;
        navMarkerItem = nullptr;
        manager.checkNavigationMarkerList();
    }
    updateMarkerTypesInMarkerList();
}

void MainWindow::deleteAllNavigationMarker() {
    while (ui->navigationMarkerListWidget->count() > 0){
        navMarkerListWidgetItem = dynamic_cast<QNavigationMarkerListWidgetItem *>(ui->navigationMarkerListWidget->item(0));
        if(navMarkerListWidgetItem != nullptr){
            navMarkerItem = navMarkerListWidgetItem->getNavMarkerItem();
            scene->removeItem(navMarkerItem);
            delete navMarkerListWidgetItem;
            delete navMarkerItem;
            navMarkerListWidgetItem = nullptr;
            navMarkerItem = nullptr;
        }

    }
    manager.navigationMarkerList.clear();
    ui->navigationMarkerListWidget->clear();
    ui->drivingTasksListWidget->clear();
    manager.checkNavigationMarkerList();
}


void MainWindow::calculateRoute(){
    Lane * laneOfCar = nullptr;
    ui->drivingTasksListWidget->clear();

    if(ui->navigationMarkerListWidget->count() <= 0){
        ui->drivingTasksListWidget->addItem(QString("No navigation targets specified"));
        return;
    }

    QList<QGraphicsItem *> list_of_items = scene->collidingItems(car_filter);
    for (QGraphicsItem  *item : list_of_items) {
        LaneItem *laneItem = qgraphicsitem_cast<LaneItem *>(item);
        if (laneItem != nullptr) {
            laneOfCar = laneItem->lane;
        }
    }

    if(laneOfCar == nullptr){
        ui->drivingTasksListWidget->addItem(QString("Please navigate the car to a lane or parking lot"));
    } else {
        vector<InstructionSet> instructionSetVector = manager.getInstructions(odo->pos.x, odo->pos.y,
                                                                              laneOfCar);

        for(auto instructionSet : instructionSetVector) {
            for(unsigned int j = 0; j < instructionSet.size(); j++){
                if (j == 0) {
                    ui->drivingTasksListWidget->addItem(QString::fromStdString(instructionSet.at(j)));
                } else {
                    ui->drivingTasksListWidget->addItem(QString::fromStdString(("\t" + instructionSet.at(j))));
                }

            }
        }
        laneOfCar = nullptr;
    }
}

void MainWindow::exportNavMarker() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString strFile = dialog.getSaveFileName(nullptr, "Save as XML", "", tr("XML Files (*.xml)"));
    manager.exportNavMarkerToXml(strFile.toLocal8Bit().data());
}

void MainWindow::importNavMarker() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QString strFile = dialog.getOpenFileName(nullptr, "Open XML file","",tr("XML Files (*.xml)"));
    bool status =  manager.importNavMarkerFromXml(strFile.toLocal8Bit().data());
   if(status){
        clearAllNavMarkerItems();
        buildNavigationMarkerItemsFromXmlList();
    } else {
        return;
    }
}

void MainWindow::clearAllNavMarkerItems() {
   for(int i = 0; i < ui->navigationMarkerListWidget->count(); i++) {
        navMarkerListWidgetItem = dynamic_cast<QNavigationMarkerListWidgetItem *>(ui->navigationMarkerListWidget->item(i));
        if (navMarkerListWidgetItem != nullptr) {
            navMarkerItem = navMarkerListWidgetItem->getNavMarkerItem();
            scene->removeItem(navMarkerItem);
            delete navMarkerItem;
        }
    }
   ui->navigationMarkerListWidget->clear();
   ui->drivingTasksListWidget->clear();

        navMarkerListWidgetItem = nullptr;
        navMarkerItem = nullptr;
}


void MainWindow::buildNavigationMarkerItemsFromXmlList() {
    for (string msg: manager.messages) {
        QString msgItem = QString::fromStdString((msg));
        ui->drivingTasksListWidget->addItem(msgItem);
    }

    for (std::shared_ptr<NavigationMarker> el: manager.navigationMarkerList) {
        NavigationMarkerItem *navMarkerItem = new NavigationMarkerItem(el, navMarker_filter);
        navMarkerItem->setToolTipMarker();
        string markerTitle = "Marker: ";
        markerTitle.append(std::to_string(el->getId()));
        QString markerString = QString::fromStdString(markerTitle);
        QNavigationMarkerListWidgetItem *labelItem = new QNavigationMarkerListWidgetItem(markerString, ui->navigationMarkerListWidget,
                                                                                         navMarkerItem);
        ui->navigationMarkerListWidget->addItem(labelItem);
    }
}

//wallerni 20.2.20
//generates the occupancy gridmap of ultrasonic- and LiDAR-sensordata in the nearfield of the car
void MainWindow::setupNearfieldGridMap() {
    if (nearfield_map_filter != nullptr) {
        scene->removeItem(nearfield_map_filter);
        delete nearfield_map_filter;
    } else{
        if(nearfieldgridmap == nullptr)
        {//TODO delete Example nearfielgridmap = new MapTreeNode()

            auto n1 = new MapTreeNode(20);
            auto n2 = new MapTreeNode(100);
            auto n3 = new MapTreeNode(200);
            auto n4 = new MapTreeNode();
            auto n5 = new MapTreeNode();
            auto n6 = new MapTreeNode(50);
            auto n7 = new MapTreeNode(1);
            auto n8 = new MapTreeNode();
            auto n9 = new MapTreeNode(40);
            auto n10 = new MapTreeNode(80);
            n4->SouthEastChild = n5;
            n4->NorthEastChild = n2;
            n4->NorthWestChild = n6;
            n5 ->NorthWestChild = n1;
            n5->SouthWestChild = n3;
            n5->NorthEastChild = n7;
            n5->SouthEastChild = n8;
            n8->SouthEastChild = n9;
            n8->NorthEastChild = n10;
            nearfieldgridmap = n4;
        }
    }
    if (nearfield_map_filter != nullptr) {
        nearfield_map_filter = new NearfieldMapItem(nearfieldgridmap, car_filter);
        nearfield_map_filter->setRotation(nearfield_map_filter->rotation() + 180);//car starts with front down
        nearfield_map_filter->setOpacity(0.7);
        nearfield_map_filter->setPos(car_width / 2 + GRIDMAP_WIDTH / 2,
                                     car_height / 2 + GRIDMAP_HEIGHT / 2 + HEIGHT_DIFF_GRIDMAPMID_CARMID);
        nearfield_map_filter->setTransformOriginPoint(car_filter->boundingRect().center());
        nearfield_map_filter->setFlag(QGraphicsItem::GraphicsItemFlag::ItemStacksBehindParent, true);
    }
}

void MainWindow::handleLogLevelSelection(){
    ui->statusbar->showMessage("Handle Loglevel Selection!");

    //determine the loglevel
    tFilterLogType loglevel = tFilterLogType::NONE;
    QString text = ui->loglevel_combo->currentText();
    if(text == "DEBUG"){
        loglevel = tFilterLogType::DEBUG;
    }else if (text == "INFO") {
        loglevel = tFilterLogType::INFO;
    } else if (text == "WARNING") {
        loglevel = tFilterLogType::WARNING;
    } else if (text == "ERROR"){
        loglevel = tFilterLogType::ERROR;
    }

    if(!initialization){
        ADTFMediaSample sample;
        tRemoteCommandMsg command = tRemoteCommandMsg(tRemoteControlSignal::ABORT, tFilterLogType::NONE, 0);
        sample.length = sizeof(command);

        sample.data.reset(new char[sample.length]);
        sample.pinName = "tRemoteCommandMsg";
        sample.mediaType = "tRemoteCommandMsg";
        sample.streamTime = 0;
        memcpy(sample.data.get(), &command, sample.length);
        this->networkClient->send(sample);

    }

    //send Loglevel set command
    ADTFMediaSample sample;
    tRemoteCommandMsg command = tRemoteCommandMsg(tRemoteControlSignal::NONE, loglevel, 0);
    sample.length = sizeof(command);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "tRemoteCommandMsg";
    sample.mediaType = "tRemoteCommandMsg";
    sample.streamTime = 0;
    memcpy(sample.data.get(), &command, sample.length);
    this->networkClient->send(sample);
}

void MainWindow::handleLogLevelACK() {
    ui->statusbar->showMessage("Handle Loglevel ACK!");

    resetControlTabVals();
    initialization = true;
    emit(guiUpdated());
}

void MainWindow::handleMapPushClick(){
    ui->statusbar->showMessage("Handle Map Push Click!");

    //open dialog
    fileNameMap = QFileDialog::getOpenFileName(this, "Choose map XML", "/home/uniautonom/smds-uniautonom-remotecontrol-src/");
    qDebug() << "XML map file" << fileNameMap << "selected for opening";
    if (fileNameMap.isNull()) return;

    //pack and send map
    ADTFMediaSample sample;
    std::ifstream is (fileNameMap.toUtf8(), std::ifstream::binary | std::ifstream::ate);
    if (!is) {
        QMessageBox::warning(this, "Cannot open file", "Cannot open file: " + fileNameMap);
        return;
    }
    sample.length = is.tellg();
    is.seekg(0);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "MapXML";
    sample.mediaType = "";
    sample.streamTime = 0;
    is.read(sample.data.get(), sample.length);
    this->networkClient->send(sample);
}

void MainWindow::handleMapPushACK(){
    ui->statusbar->showMessage("Handle Map Push ACK!");

    ContentManager &contentManager = ContentManager::getInstance();
    contentManager.clear();
    contentManager.importFromXml(fileNameMap.toUtf8());

    this->updateMap();
    NavigationHelper::makeMapGraph();

    mapreceived = true;
    emit(guiUpdated());
}

void MainWindow::handleCarConfigLoadClick(){
    ui->statusbar->showMessage("Handle Car Config Load Click!");

    //open dialog
    fileNameCarConfig = QFileDialog::getOpenFileName(this, "Choose car configuration ini file");
    qDebug() << "car configuration ini file" << fileNameCarConfig << "selected for opening";
    if (fileNameCarConfig.isNull()) return;

    QSettings carSettings(fileNameCarConfig, QSettings::IniFormat);
    car_height = carSettings.value("car/length", 400).toInt();
    car_width = carSettings.value("car/width", 240).toInt();
    car_init_x = carSettings.value("odoinit/posx", 200.0).toFloat();
    car_init_y = carSettings.value("odoinit/posy", 200.0).toFloat();
    car_init_orientation = carSettings.value("odoinit/orientation", 0.1).toFloat();

    carconfselected = true;
    emit(guiUpdated());
}

void MainWindow::handleCarConfigPushClick(){
    ui->statusbar->showMessage("Handle Car Config Push Click!");

    //send tCarConfigStruct
    ADTFMediaSample sample;
    tCarConfigStruct config = prepareCarConfigStruct();
    sample.length = sizeof(config);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "tCarConfigStruct";
    sample.mediaType = "tCarConfigStruct";
    sample.streamTime = 0;
    memcpy(sample.data.get(), &config, sample.length);
    this->networkClient->send(sample);
}

void MainWindow::handleCarConfigPushACK() {
    ui->statusbar->showMessage("Handle Car Config Push ACK!");

    initialization = false;
    carconfselected = false;
    mapreceived = false;
    carconfreceived = true;
    ready = true;
    tCarOdometry startOdo = tCarOdometry(cv::Point2f(car_init_x, car_init_y), car_init_orientation, 0);
    this->setCarOdometry(startOdo);
}

void MainWindow::handleRouteInfoPushClick(){
    ui->statusbar->showMessage("Handle Route Info Push Click!");

    //send route info
    //TODO

    //TODO DEVELOPMENT
    handleRouteInfoPushACK();
}

void MainWindow::handleRouteInfoPushACK() {
    ui->statusbar->showMessage("Handle Route Info Push ACK!");

    routeinforreceived = true;
    emit(guiUpdated());
}

void MainWindow::handleStartADClick(){
    ui->statusbar->showMessage("Handle Start AD Click!");

    //send start ad signal
    ADTFMediaSample sample;
    tRemoteCommandMsg command = tRemoteCommandMsg(tRemoteControlSignal::START, tFilterLogType::NONE, 0);
    sample.length = sizeof(command);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "tRemoteCommandMsg";
    sample.mediaType = "tRemoteCommandMsg";
    sample.streamTime = 0;
    memcpy(sample.data.get(), &command, sample.length);
    this->networkClient->send(sample);
}

void MainWindow::handleStartADACK() {
    ui->statusbar->showMessage("Handle Start AD ACK!");

    ad_running = true;
    rc_running = false;
    emit(guiUpdated());
}

void MainWindow::handleStartRCClick(){
    ui->statusbar->showMessage("Handle Start RC Click!");

    //send start RC signal
    ADTFMediaSample sample;
    tRemoteCommandMsg command = tRemoteCommandMsg(tRemoteControlSignal::RC, tFilterLogType::NONE, 0);
    sample.length = sizeof(command);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "tRemoteCommandMsg";
    sample.mediaType = "tRemoteCommandMsg";
    sample.streamTime = 0;
    memcpy(sample.data.get(), &command, sample.length);
    this->networkClient->send(sample);
}

void MainWindow::handleStartRCACK() {
    ui->statusbar->showMessage("Handle Start RC ACK!");

    rc_running = true;
    ad_running = false;
    emit(guiUpdated());
}

void MainWindow::handleStopClick(){
    ui->statusbar->showMessage("Handle Stop Click!");
    stopClick = true;
    //send stop signal
    ADTFMediaSample sample;
    tRemoteCommandMsg command = tRemoteCommandMsg(tRemoteControlSignal::STOP, tFilterLogType::NONE, 0);
    sample.length = sizeof(command);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "tRemoteCommandMsg";
    sample.mediaType = "tRemoteCommandMsg";
    sample.streamTime = 0;
    memcpy(sample.data.get(), &command, sample.length);
    this->networkClient->send(sample);
}

void MainWindow::handleStopACK() {
    ui->statusbar->showMessage("Handle Stop ACK!");

    ad_running = false;
    rc_running = false;

    emit(guiUpdated());
}

void MainWindow::handleAbortClick(){
    ui->statusbar->showMessage("Handle Abort Click!");

    //send abort signal
    ADTFMediaSample sample;
    tRemoteCommandMsg command = tRemoteCommandMsg(tRemoteControlSignal::ABORT, tFilterLogType::NONE, 0);
    sample.length = sizeof(command);

    sample.data.reset(new char[sample.length]);
    sample.pinName = "tRemoteCommandMsg";
    sample.mediaType = "tRemoteCommandMsg";
    sample.streamTime = 0;
    memcpy(sample.data.get(), &command, sample.length);
    this->networkClient->send(sample);
}

void MainWindow::handleAbortACK() {
    ui->statusbar->showMessage("Handle Abort ACK!");

    ui->loglevel_combo->setCurrentIndex(0);
    resetControlTabVals();
    resetMemberVariables();
    emit(guiUpdated());
}

void MainWindow::updateControlTab() {
    //update car state
    if(state != tState::NONE){
        ui->state_val_label->setText(QString::fromStdString(tStateMap.find(state)->second));
        switch(state){
            case tState::INITIALIZATION:
            {
                ui->state_val_label->setStyleSheet("QLabel { background-color : yellow}");
                break;
            }
            case tState::READY:
            {
                ui->state_val_label->setStyleSheet("QLabel { background-color : green}");
                break;
            }
            case tState::AD_RUNNING:
            {
                ui->state_val_label->setStyleSheet("QLabel { background-color : orange}");
                break;
            }
            case tState::RC_RUNNING:
            {
                ui->state_val_label->setStyleSheet("QLabel { background-color : orange}");
                break;
            }
            case tState::EMERGENCY:
            {
                ui->state_val_label->setStyleSheet("QLabel { background-color : red}");
                break;
            }
        }
    }else{
        ui->state_val_label->setStyleSheet("QLabel { background-color : white}");
        ui->state_val_label->setText(QString::fromStdString("-"));
    }
    if(speed != nullptr){
        ui->speed_val_label->setText(QString::number(speed->combinedSpeed, 'f', 2));
    }else{
        ui->speed_val_label->setText(QString::fromStdString("-"));
    }

    if(odo != nullptr) {
        ui->orientation_val_label->setText(QString::number(odo->orientation, 'f', 1));
        ui->position_x_val_label->setText(QString::number(odo->pos.x, 'f', 1));
        ui->position_y_val_label->setText(QString::number(odo->pos.y, 'f', 1));
    }else{
        ui->orientation_val_label->setText(QString::fromStdString("-"));
        ui->position_x_val_label->setText(QString::fromStdString("-"));
        ui->position_y_val_label->setText(QString::fromStdString("-"));
    }

    //update map filename
    if(fileNameMap != nullptr){
        ui->map_xml_val_label->setText(fileNameMap);
    }else{
        ui->map_xml_val_label->setText("-");
    }
    
    //update car config filename
    if(fileNameCarConfig != nullptr){
        ui->car_config_val_label->setText(fileNameCarConfig);
    }else{
        ui->car_config_val_label->setText("-");
    }

    //update car x and y
    if(fileNameCarConfig != nullptr){
        ui->car_x_val_edit->setText(QString::number(this->car_init_x, 'f', 2));
        ui->car_y_val_edit->setText(QString::number(this->car_init_y, 'f', 2));
        ui->car_orientation_val_edit->setText(QString::number(this->car_init_orientation, 'f', 2));
    }else{
    	if(mapreceived || carconfreceived || ad_running || rc_running) {
            QSettings settings;
            QSettings carSettings(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString(), QSettings::IniFormat);
            this->car_height = carSettings.value("car/length", 400).toInt();
            this->car_width = carSettings.value("car/width", 240).toInt();
            this->car_init_x = carSettings.value("odoinit/posx", 200.0).toFloat();
            this->car_init_y = carSettings.value("odoinit/posy", 200.0).toFloat();
            this->car_init_orientation = carSettings.value("odoinit/orientation", 0.1).toFloat();
        
            ui->car_config_val_label->setText(settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString());
            ui->car_x_val_edit->setText(QString::number(this->car_init_x, 'f', 2));
            ui->car_y_val_edit->setText(QString::number(this->car_init_y, 'f', 2));
            ui->car_orientation_val_edit->setText(QString::number(this->car_init_orientation, 'f', 2));
        } else {
            ui->car_x_val_edit->setText(QString::fromStdString("-"));
            ui->car_y_val_edit->setText(QString::fromStdString("-"));
            ui->car_orientation_val_edit->setText(QString::fromStdString("-"));
        }
    }

    //manage control tab buttons
    ui->loglevel_combo->setEnabled(!mapreceived);
    ui->map_load_button->setEnabled(initialization);
    ui->car_config_load_button->setEnabled(mapreceived);
    ui->abort_button->setEnabled(carconfreceived || ad_running || rc_running);
    ui->car_config_push_button->setEnabled(carconfselected || mapreceived);
    ui->navi_route_push_button->setEnabled(carconfreceived);
    ui->start_rc_button->setEnabled(ready);
    ui->start_ad_button->setEnabled(routeinforreceived);
    ui->running_stop_button->setEnabled(ad_running || rc_running);
    if(emergency){
        ui->map_load_button->setEnabled(false);
        ui->car_config_load_button->setEnabled(false);
        ui->car_config_push_button->setEnabled(false);
        ui->navi_route_push_button->setEnabled(false);
        ui->start_ad_button->setEnabled(false);
        ui->start_rc_button->setEnabled(false);
        ui->running_stop_button->setEnabled(false);
        ui->abort_button->setEnabled(false);
    }
}

void MainWindow::resetControlTabVals() {
    initialization = false;
    mapreceived = false;
    carconfselected = false;
    carconfreceived = false;
    ready = false;
    rc_running = false;
    routeinforreceived = false;
    ad_running = false;
    stopClick = false;
    emergency = false;
}

void MainWindow::resetMemberVariables(){
    manager.clear();
    odo = nullptr;
    speed = nullptr;
    coords = nullptr;
    detectedLineArray = nullptr;
    nearfieldgridmap = nullptr;

    state = tState::INITIALIZATION;
    fileNameMap = nullptr;
    fileNameCarConfig = nullptr;
}

tCarConfigStruct MainWindow::prepareCarConfigStruct() {
    QSettings settings;
    QSettings carSettings(fileNameCarConfig.isNull() ? settings.value("car/settings", "/home/uniautonom/smds-uniautonom-remotecontrol-src/global/carconfig/default.ini").toString() : fileNameCarConfig, QSettings::IniFormat);

    tUInt32 length = carSettings.value("car/length", 400).toInt();
    car_height = length;
    tUInt32 width = carSettings.value("car/width", 240).toInt();
    car_width = width;
    cv::Point2f initpos = cv::Point2f(ui->car_x_val_edit->text().toFloat(), ui->car_y_val_edit->text().toFloat());
    car_init_x = initpos.x;
    car_init_y = initpos.y;
    tFloat32 initorientation = ui->car_orientation_val_edit->text().toFloat();
    car_init_orientation = initorientation;
    cv::Point2f traLeftNear = cv::Point2f(carSettings.value("carview/leftnearx", -300).toFloat(), carSettings.value("carview/leftneary", 300).toFloat());
    cv::Point2f traLeftFar = cv::Point2f(carSettings.value("carview/leftfarx", -300).toFloat(), carSettings.value("carview/leftfary", 600).toFloat());
    cv::Point2f traRightFar = cv::Point2f(carSettings.value("carview/rightfarx", 300).toFloat(), carSettings.value("carview/rightfary", 600).toFloat());
    cv::Point2f traRightNear = cv::Point2f(carSettings.value("carview/rightnearx", 300).toFloat(), carSettings.value("carview/rightneary", 300).toFloat());

    tCarConfigStruct carConfigStruct = tCarConfigStruct(length, width, initpos, initorientation, traLeftNear, traLeftFar, traRightFar, traRightNear);
    return carConfigStruct;
}

//TODO SEND SIGNAL VALUES TO TEST PATH
void MainWindow::sendtSignalValue() {
//    ADTFMediaSample sample;
//    tSignalValue command = tSignalValue(0);
//    sample.length = sizeof(command);
//
//    sample.data.reset(new char[sample.length]);
//    sample.pinName = "tSignalValue";
//    sample.mediaType = "tSignalValue";
//    sample.streamTime = 0;
//    memcpy(sample.data.get(), &command, sample.length);
//    this->networkClient->send(sample);
}
