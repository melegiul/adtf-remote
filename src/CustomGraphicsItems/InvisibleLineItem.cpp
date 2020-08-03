//
// Created by uniautonom on 01.10.19.
//

#include <QtGui/QPen>

#include "Map/Line.hpp"
#include "Map/Node.hpp"
#include "InvisibleLineItem.h"

InvisibleLineItem::InvisibleLineItem(Line *line, int width, QGraphicsItem *parent) : QGraphicsPathItem(parent),
                                                                                     line(line), width(width) {
    QPolygon path;
    for (auto n: line->path) {
        path.append(QPoint(n->x, n->y));
    }
    QPen pen = QPen(QColor(0, 0, 0, 30));
    QVector<qreal> dashes;
    dashes << 1 << 2;
    pen.setDashPattern(dashes);
    pen.setWidth(width);
    auto qpp = QPainterPath();
    qpp.addPolygon(path);
    setPath(qpp);
    setPen(pen);
}
