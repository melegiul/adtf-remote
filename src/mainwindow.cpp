#include <vector>
#include <memory>
#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QSettings>

#include <QGraphicsItem>

#include "mainwindow.h"
#include "dialog_preferences.h"
#include "networkclient.h"
#include "adtf_converters/carodometry.h"
#include "adtf_converters/trapezoid.h"
#include "adtf_converters/detectedline.h"
#include "adtf_converters/nearfieldgridmap.h"
#include "adtf_converters/remoteStateMsg.h"

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
    std::map<tState, std::string> tStateMap = {
        {tState::NONE, "None"},
        {tState::INITIALIZATION, "Initialization"},
        {tState::READY, "Ready"},
        {tState::AD_RUNNING, "AD_Running"},
        {tState::RC_RUNNING, "RC_Running"},
        {tState::EMERGENCY, "Emergency"}
    };
    QSettings settings;
    this->car_height = settings.value("car/length", 400).toInt();
    this->car_width = settings.value("car/width", 240).toInt();
    int ui_background = settings.value("ui/background", 180).toInt();

    ui->setupUi(this);

    connect(ui->map_view, &ResizeGraphicsView::resized, ui->measure_bar, &BottomLeftGraphicsView::resize);

    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::openPreferences);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::connectNetwork);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnectNetwork);

    connect(this->networkClient, &NetworkClient::connected, this, &MainWindow::networkConnected);
    connect(this->networkClient, &NetworkClient::disconnected, this, &MainWindow::networkDisconnected);
    connect(this->networkClient, &NetworkClient::received, this, &MainWindow::networkReceived);
    connect(this->networkClient, &NetworkClient::errored, this, &MainWindow::networkErrored);

    // control tab related code
    // connection update with connection menu trigger
    connect(this, SIGNAL(carUpdated()), this, SLOT(updateCar()));
    // odometry update with updateCar trigger

    connect(this, SIGNAL(guiUpdated()), this, SLOT(updateControlTab()));
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

void MainWindow::openMapXML() {
    QString fileName = QFileDialog::getOpenFileName(this, "Choose map XML");
    qDebug() << "XML map file" << fileName << "selected for opening";
    if (fileName.isNull()) return;

    ADTFMediaSample sample;
    std::ifstream is (fileName.toUtf8(), std::ifstream::binary | std::ifstream::ate);
    if (!is) {
        QMessageBox::warning(this, "Cannot open file", "Cannot open file: " + fileName);
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

    ContentManager &contentManager = ContentManager::getInstance();
    contentManager.clear();
    contentManager.importFromXml(fileName.toUtf8());

    this->updateMap();
    NavigationHelper::makeMapGraph();
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

    //FIXME Just for development
    initialization = true;
    emit(guiUpdated());

}

void MainWindow::disconnectNetwork()
{
    this->networkClient->disconnectNetwork();
}

void MainWindow::networkConnected() {
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->statusbar->showMessage("Connected to " + this->networkClient->getPeer());
    ui->connection_val_label->setText(QString::fromStdString("connected"));
    ui->connection_val_label->setStyleSheet("QLabel { background-color : green}");
    initialization = true;
    emit(guiUpdated());
}

void MainWindow::networkDisconnected() {
    ui->actionDisconnect->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->connection_val_label->setText(QString::fromStdString("not connected"));
    ui->connection_val_label->setStyleSheet("QLabel { background-color : red}");
    resetControlTabBools();
    emit(guiUpdated());
}

void MainWindow::networkReceived(ADTFMediaSample sample)
{
    if (sample.pinName == "CarOdometry" && sample.mediaType == "tCarOdometry") {
        tCarOdometry odometry = adtf_converter::from_network::carOdometry(sample);
        this->setCarOdometry(odometry);
    } else if (sample.pinName == "Trapezoid" && sample.mediaType == "tTrapezoid") {
        tTrapezoid trapezoid = adtf_converter::from_network::trapezoid(sample);
        this->setTrapezoidCoords(trapezoid);
    } else if (sample.pinName == "DetectedLine_POD" && sample.mediaType == "tDetectedLine_POD") {
        std::vector<DetectedLine_POD> lines = adtf_converter::from_network::detectedLine(sample);
        std::unique_ptr<tDetectedLine> tDetLines = adtf_converter::from_network::detectedLinePOD(lines);
        this->setDetectedLine(std::move(tDetLines));
    } else if (sample.pinName == "NearfieldGridmap" && sample.mediaType == "tNearfieldGridMapArray") {
        std::unique_ptr<tNearfieldGridMapArray> nearfieldGrid = adtf_converter::from_network::nearfieldGridmap(sample);
        this->setNearfieldgridmap(*nearfieldGrid);
    } else if (sample.pinName == "state" && sample.mediaType == "tRemoteStateMsg") {
        std::unique_ptr<tRemoteStateMsg> statemsg = adtf_converter::from_network::remoteStateMsg(sample);
        this->setCarState(*statemsg);
    } else if (sample.pinName == "log" && sample.mediaType == "tLogMsg") {

    }

    //FIXME concrete pinNames and Datatypes to be set
    //TODO add new data Pins here
}

void MainWindow::networkErrored(QString errorMsg) {
    ui->statusbar->showMessage(errorMsg);
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
    else if (item->text(0) == "Nearfieldgridmap") nearfield_map_filter->setVisible(item->checkState(column));
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
    if (nearfield_map_filter != nullptr){ //wallerni 25.2.20
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
    navMarker_filter = new TreeNodeItem();

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
    scene->addItem(navMarker_filter);
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
    //setEditingMode(ui->editor_checkbox->checkState());
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

    setupNearfieldGridMap();
}

//Setup the trapezoid by creating a QGraphicsPolygonItem and set properties
void MainWindow::setupTrapezoid() {
    if (this->coords == nullptr)
        return;

    if (trapezoid_filter != nullptr) {
        scene->removeItem(trapezoid_filter);
        delete trapezoid_filter;
    }

    trapezoid_filter = new QGraphicsPolygonItem(QPolygonF(QVector<QPointF>() << QPointF((int) coords->pos_A.x, (int) coords->pos_A.y)
                                                            << QPointF((int) coords->pos_B.x, (int) coords->pos_B.y)
                                                            << QPointF((int) coords->pos_C.x, (int) coords->pos_C.y)
                                                            << QPointF((int) coords->pos_D.x, (int) coords->pos_D.y)));

    trapezoid_filter->setPen(QPen(Qt::green, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    trapezoid_filter->setZValue(1000);
    scene->addItem(trapezoid_filter);
}
//Setup the detected Lines and add it to the scene
void MainWindow::setupDetectedLine() {
    if (detectedLine == nullptr)
        return;

    if (detected_line_filter != nullptr) {
        scene->removeItem(detected_line_filter);
        delete detected_line_filter;
    }

    if (detectedLine->arraySize >= 0 && show_detected_line) {

        for (int i = 0; i < detectedLine->arraySize; i++) {

            QLineF line = QLineF((qreal) detectedLine->detectedLine[i].fromPos.x, (qreal) detectedLine->detectedLine[i].fromPos.y,
                                 (qreal) detectedLine->detectedLine[i].toPos.x, (qreal) detectedLine->detectedLine[i].toPos.y);


            QGraphicsLineItem *lineItem = new QGraphicsLineItem(line, detected_line_filter);

            lineItem->setPen(QPen(Qt::red, 40, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            lineItem->setZValue(1);
            scene->addItem(lineItem);
        }
    }
}


void MainWindow::updateCar() {
    if (car_filter == nullptr) setupCar();
    if (odo == nullptr) return;
    auto angle = odo->orientation * 180 / M_PI;
    if (angle < -0.5) angle += 360;

    //update car status messages
    ui->speed_val_label->setText(QString::fromStdString("to be done"));
    ui->orientation_val_label->setText(QString::fromStdString(std::to_string((int) odo->orientation)));
    ui->position_x_val_label->setText(QString::fromStdString(std::to_string((int) odo->pos.x)));
    ui->position_y_val_label->setText(QString::fromStdString(std::to_string((int) odo->pos.y)));

    car_filter->setPos(odo->pos.x - car_width / 2., odo->pos.y - car_height / 2.);
    car_filter->setRotation(-angle);

    if (show_active_lanes) colorLanesOfInterest();

    focusOnCar();
}

//Update the position of the trapezoid
void MainWindow::updateTrapezoid() {
    if (trapezoid_filter == nullptr) setupTrapezoid();
    if (coords == nullptr) return;

    trapezoid_filter->setPolygon(QPolygonF(QVector<QPointF>() << QPointF((int) coords->pos_A.x, (int) coords->pos_A.y)
                                                              << QPointF((int) coords->pos_B.x, (int) coords->pos_B.y)
                                                              << QPointF((int) coords->pos_C.x, (int) coords->pos_C.y)
                                                              << QPointF((int) coords->pos_D.x, (int) coords->pos_D.y)));
}
//Update detected Lines. First remove all Lines then add the new detected lines
void MainWindow::updateDetectedLine() {
    if (detected_line_filter == nullptr && show_detected_line) setupDetectedLine();
    if (detectedLine == nullptr) return;


    if (detectedLine->arraySize >= 0) {

            foreach (QGraphicsItem *item, scene->items()) {
                QGraphicsLineItem *lineItem = dynamic_cast<QGraphicsLineItem*>(item);
                if (lineItem != nullptr && lineItem->parentItem() == detected_line_filter) scene->removeItem(lineItem);
            }

        if (show_detected_line) {
            for (int i = 0; i < detectedLine->arraySize; i++) {

                QLineF line = QLineF((qreal) detectedLine->detectedLine[i].fromPos.x,
                                     (qreal) detectedLine->detectedLine[i].fromPos.y,
                                     (qreal) detectedLine->detectedLine[i].toPos.x,
                                     (qreal) detectedLine->detectedLine[i].toPos.y);


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


void MainWindow::setCarOdometry(tCarOdometry &odo) {
    // ensures that carUpdate is called within Widget (only Widget is allowed to update the map)
    this->odo = &odo;
    emit(carUpdated());
}


void MainWindow::setTrapezoidCoords(tTrapezoid &coords) {
    this->coords = &coords;
    emit(trapezoidUpdated());
}

void MainWindow::setDetectedLine(std::shared_ptr<tDetectedLine> detectedLine) {
    this->detectedLine = detectedLine;
    emit(detectedLineUpdated());
}

void MainWindow::setNearfieldgridmap(tNearfieldGridMapArray &root) {
    this->nearfieldgridmap = MapTreeNode::generateFromArray(&root);
    if(this->nearfieldgridmap == nullptr) {
        //printw("Nullptr\n");
        this->nearfieldgridmap = new MapTreeNode(1);
    }
    emit(nearfieldGridMapUpdated());
}

void MainWindow::setCarState(tRemoteStateMsg &statemsg) {
    state = statemsg.filterState;
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
    nearfield_map_filter = new NearfieldMapItem(nearfieldgridmap,car_filter);
    nearfield_map_filter->setRotation(nearfield_map_filter->rotation() + 180);//car starts with front down
    nearfield_map_filter->setOpacity(0.7);
    nearfield_map_filter->setPos(car_width/2 + GRIDMAP_WIDTH/2,car_height/2 + GRIDMAP_HEIGHT/2 + HEIGHT_DIFF_GRIDMAPMID_CARMID);
    nearfield_map_filter->setTransformOriginPoint(car_filter->boundingRect().center());
    nearfield_map_filter->setFlag(QGraphicsItem::GraphicsItemFlag::ItemStacksBehindParent,true);
}

void MainWindow::handleMapPushClick(){
    ui->statusbar->showMessage("Handle Map Push Click!");

    //open dialog
    fileName = QFileDialog::getOpenFileName(this, "Choose map XML");
    qDebug() << "XML map file" << fileName << "selected for opening";
    if (fileName.isNull()) return;

    //TODO go on here
//    //pack and send map
//    ADTFMediaSample sample;
//    std::ifstream is (fileName.toUtf8(), std::ifstream::binary | std::ifstream::ate);
//    if (!is) {
//        QMessageBox::warning(this, "Cannot open file", "Cannot open file: " + fileName);
//        return;
//    }
//    sample.length = is.tellg();
//    is.seekg(0);
//
//    sample.data.reset(new char[sample.length]);
//    sample.pinName = "MapXML";
//    sample.mediaType = "";
//    sample.streamTime = 0;
//    is.read(sample.data.get(), sample.length);
//    this->networkClient->send(sample);
//
//    ContentManager &contentManager = ContentManager::getInstance();
//    contentManager.clear();
//    contentManager.importFromXml(fileName.toUtf8());
//
//    this->updateMap();
//    NavigationHelper::makeMapGraph();



    mapreceived = true;
    emit(guiUpdated());
}

void MainWindow::handleCarConfigLoadClick(){
    ui->statusbar->showMessage("Handle Car Config Load Click!");

    //open dialog

    //send map via

    carconfselected = true;
    emit(guiUpdated());
}

void MainWindow::handleCarConfigPushClick(){
    ui->statusbar->showMessage("Handle Car Config Push Click!");

    //open dialog

    //send map via

    carconfreceived = true;
    ready = true;
    emit(guiUpdated());
}

void MainWindow::handleRouteInfoPushClick(){
    ui->statusbar->showMessage("Handle Route Info Push Click!");

    //open dialog

    //send map via

    routeinforreceived = true;
    emit(guiUpdated());
}

void MainWindow::handleStartADClick(){
    ui->statusbar->showMessage("Handle Start AD Click!");

    //open dialog

    //send map via


    ad_running = true;
    emit(guiUpdated());
}

void MainWindow::handleStartRCClick(){
    ui->statusbar->showMessage("Handle Start RC Click!");

    //open dialog

    //send map via


    rc_running = true;
    emit(guiUpdated());
}

void MainWindow::handleStopClick(){
    ui->statusbar->showMessage("Handle Stop Click!");

    //open dialog

    //send map via


    ad_running = false;
    rc_running = false;

    emit(guiUpdated());
}

void MainWindow::handleAbortClick(){
    ui->statusbar->showMessage("Handle Abort Click!");

    //open dialog

    //send map via

    resetControlTabBools();
    initialization = true;
    emit(guiUpdated());
}

void MainWindow::updateControlTab() {
    //update car state
    if(state != tState::NONE){
        ui->state_val_label->setText(QString::fromStdString(tStateMap[state]));
    }

    //update map filename
    if(fileName != nullptr){
        ui->map_xml_val_label->setText(fileName);
    }

    //update car config filename
    if(carConfig != nullptr){
        ui->car_config_val_label->setText(carConfig);
    }

    //update car x and y
    if(carConfig != nullptr){
        //TODO load information for Car config object

        ui->car_x_val_edit->setText(QString::fromStdString("13"));
        ui->car_x_val_edit->setText(QString::fromStdString("37"));
    }


    //manage control tab buttons
    ui->map_load_button->setEnabled(initialization);
    ui->car_config_load_button->setEnabled(mapreceived);
    ui->abort_button->setEnabled(mapreceived);
    ui->car_config_push_button->setEnabled(carconfselected);
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

void MainWindow::resetControlTabBools() {
    initialization = false;
    mapreceived = false;
    carconfselected = false;
    carconfreceived = false;
    ready = false;
    rc_running = false;
    routeinforreceived = false;
    ad_running = false;
    emergency = false;
}

