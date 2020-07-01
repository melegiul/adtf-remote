//
// Created by uniautonom on 01.10.19.
//

#include "CrossingAreaItem.h"
#include <QBrush>
#include <QPen>

CrossingAreaItem::CrossingAreaItem(Crossing *crossing, MapArea *area, QGraphicsItem *parent) : QGraphicsPolygonItem(
        parent), crossing(crossing), area(area) {
    QColor color = QColor(255, 152, 20, 150);  // semi-transparent orange
    QPolygon outline;
    for (auto el2: area->outline) {
        outline.append(QPoint(el2->x, el2->y));
    }
    setPolygon(outline);

    QString tip("Crossing: ");
    tip.append(std::to_string(crossing->getId()).c_str());
    tip.append("\nArea: ");
    tip.append(std::to_string(area->get_id()).c_str());
    for (auto id: crossing->lane_ids) {
        tip.append("\nLane: ");
        tip.append(std::to_string(id).c_str());
    }

    setBrush(QBrush(color, Qt::SolidPattern));
    setPen(QPen(color));
    setToolTip(tip);
}
