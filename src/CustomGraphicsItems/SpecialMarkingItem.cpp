//
// Created by uniautonom on 01.10.19.
//

#include "SpecialMarkingItem.h"

SpecialMarkingItem::SpecialMarkingItem(SpecialMarking *m, QPolygon outline) : QGraphicsPolygonItem(outline), m(m) {
    QString tip("ID: ");
    tip.append(std::to_string(m->getId()).c_str());
    tip.append("\nType: ");
    tip.append(enumStrings<eSpecialMarking >::data[m->type]);
    for (int i: m->adjacentLane) {
        tip.append("\nAdjacent Lane: ");
        tip.append(std::to_string(i).c_str());
    }

    QColor color;
    if (m->r == -1 || m->g == -1 || m->b == -1) {
        color = QColor(Qt::black);
    } else {
        color = QColor(m->r, m->g, m->b);
    }
    QPen pen = QPen(color);
    setPen(pen);

    if (m->type == eSpecialMarking::ParkingDisabled
        || m->type == eSpecialMarking::ParkingElectric
        || m->type == eSpecialMarking::ParkingFamily
        || m->type == eSpecialMarking::ParkingRegular
        || m->type == eSpecialMarking::ParkingWomen) {
        setPen(QPen(Qt::black));

        QPixmap label(":/parking.png");
        const QSize b_rect = boundingRect().size().toSize() / 2;
        label = label.scaled(b_rect, Qt::KeepAspectRatio);
        auto *item = new QGraphicsPixmapItem(label, this);

        item->setPos(boundingRect().center().x() - label.size().width() / 2.,
                     boundingRect().center().y() - label.size().height() / 2.);
        item->setToolTip(tip);
    }

    if (m->type == eSpecialMarking::crossWalk) {
        if (outline.size() == 4) {
            QImage zebra(":/zebra.png");
            //Determine the direction of the crosswalk
            QPoint first = outline.value(0);
            QPoint second = outline.value(1);
            QPoint third = outline.value(2);
            QLineF width(first, second);
            QLineF height(second, third);
            zebra = zebra.scaled(width.length(), height.length());
            QTransform transform;
            transform = transform.rotate(width.angle());
            QPixmap::fromImage(zebra);

            QBrush brush;
            brush.setTransform(transform);
            brush.setTexture(QPixmap::fromImage(zebra));
            setBrush(brush);
        }
    } else {
        setBrush(QBrush(color));
    }
    setToolTip(tip);
}

SpecialMarking *SpecialMarkingItem::getM() const {
    return m;
}

void SpecialMarkingItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    QAction *addNavMarkingAction = menu.addAction("Add Navigation Target");
    QAction *selectedAction = menu.exec(event->screenPos());

    if (selectedAction == addNavMarkingAction) {
        //if the special marking is a type of parking lot it serves as navigation target
        if (m->isParkingType()) {
            Node *pos = new Node(event->scenePos().x(), event->scenePos().y());
            //this line registers the datatype shared pointer in the Qt framework
            qRegisterMetaType<std::shared_ptr<NavigationMarker>>();
            std::shared_ptr<NavigationMarker> navMarker = std::make_shared<NavigationMarker>(pos, eNavType::Final,
                                                                                             m);
            ItemSignalController::getInstance().sendAddNavigationMarkerClicked(navMarker);
        } else {
            //is the special marking not a parking lot the corresponding lane serves as navigation target
            QList<QGraphicsItem *> list = scene()->items(event->scenePos());
            for (int i = 0; i < list.size(); i++) {
                LaneItem *laneItem = dynamic_cast<LaneItem *>(list.at(i));
                if (laneItem != nullptr) {
                    laneItem->propagateMouseEventToLaneItem(event);
                    break;
                }
            }
        }
    }
}
