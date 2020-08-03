//
// Created by uniautonom on 22.09.19.
//

#include "LaneItem.h"

LaneItem::LaneItem(Lane *lane, QGraphicsItem *parent) : QGraphicsPolygonItem(parent), lane(lane) {
    // ensure the correct order of left and right line to correctly set up the lane area
    QPolygon poly;
    auto &manager = ContentManager::getInstance();
    int left_id = lane->getLeft();
    int right_id = lane->getRight();
    auto left = manager.line_map.find(left_id);
    auto right = manager.line_map.find(right_id);
    if (left != manager.line_map.end() && right != manager.line_map.end()) {
        for (auto pNode: left->second->path) {
            poly.append(QPoint(pNode->x, pNode->y));
        }
        auto left_start = left->second->path.front();
        auto right_start = right->second->path.front();
        auto right_end = right->second->path.back();
        double d_end = std::sqrt(std::pow(left_start->x - right_end->x, 2) + std::pow(left_start->y - right_end->y, 2));
        double d_start = std::sqrt(
                std::pow(left_start->x - right_start->x, 2) + std::pow(left_start->y - right_start->y, 2));
        if (d_end < d_start) {
            for (auto el2: right->second->path) {
                poly.append(QPoint(el2->x, el2->y));
            }
        } else {
            for (auto i = right->second->path.rbegin();
                 i != right->second->path.rend(); ++i) {
                auto el2 = *i;
                poly.append(QPoint(el2->x, el2->y));
            }
        }
        setPolygon(poly);

        colorRegular();

        QString tip("Lane: ");
        tip.append(std::to_string(lane->getId()).c_str());
        tip.append("\nRight Line: ");
        tip.append(std::to_string(lane->getRight()).c_str());
        tip.append("\nLeft Line: ");
        tip.append(std::to_string(lane->getLeft()).c_str());
        setToolTip(tip);
    }
}

void LaneItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    QAction *addNavMarkingAction = menu.addAction("Add Navigation Target");
    QAction *selectedAction = menu.exec(event->screenPos());

    Node *pos = new Node(event->scenePos().x(), event->scenePos().y());
    if (selectedAction == addNavMarkingAction) {
        //this line registers the datatype shared pointer in the Qt framework
        qRegisterMetaType<std::shared_ptr<NavigationMarker>>();
        std::shared_ptr<NavigationMarker> navMarker = std::make_shared<NavigationMarker>(pos, eNavType::Final, lane);
        ItemSignalController::getInstance().sendAddNavigationMarkerClicked(navMarker);
    }
}

//At some points on the map SpecialMarkingItems, which are not parking slots, are on top of LaneItems. This function delegates the call to the LaneItem beneath
void LaneItem::propagateMouseEventToLaneItem(QGraphicsSceneContextMenuEvent *event) {
    Node *pos = new Node(event->scenePos().x(), event->scenePos().y());
    qRegisterMetaType<std::shared_ptr<NavigationMarker>>();
    std::shared_ptr<NavigationMarker> navMarker = std::make_shared<NavigationMarker>( pos, eNavType::Final, lane);
    ItemSignalController::getInstance().sendAddNavigationMarkerClicked(navMarker);
}

bool LaneItem::isEqual(Lane *lane0) {
    return this->lane == lane0;
}

void LaneItem::colorRegular() {
    setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
    setZValue(0);
}

void LaneItem::colorSelected() {
    setPen(QPen(Qt::darkRed, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setBrush(QBrush(Qt::darkRed, Qt::SolidPattern));
    setZValue(0);
}

void LaneItem::colorCarOnTop() {
    setPen(QPen(QColor(70, 70, 70), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setBrush(QBrush(QColor(70, 70, 70), Qt::SolidPattern));
    setZValue(5);
}


