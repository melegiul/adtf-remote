//
// Created by uniautonom on 01.10.19.
//

#include <cmath>
#include <QtCore/qmath.h>
#include <QtGui/QPen>
#include <iostream>

#include "Map/Node.hpp"
#include "VisibleLineItem.h"

VisibleLineItem::VisibleLineItem(LineMarking *marking, Line *line, int offset, int line_width, QGraphicsItem *parent)
        : QGraphicsPathItem(parent), marking(marking), line(line), offset(offset), line_width(line_width) {
    QPen pen = QPen(QColor(marking->r, marking->g, marking->b, 255));
    pen.setWidth(line_width);
    pen.setCapStyle(Qt::FlatCap);
    if (marking->gap_length > 0) {
        QVector<qreal> dashes;
        dashes << marking->stroke_length / pen.width() << marking->gap_length / pen.width();
        pen.setDashPattern(dashes);
    }

    // determine actual line path with the given offset
    QPolygon polyline;
    if (offset == 0) {
        for (auto n: line->path) {
            polyline.append(QPoint(n->x, n->y));
        }
    } else {
        QLineF line_prev, line_next;
        QPointF prev, current, next;
        bool first_set = false, second_set = false;
        for (auto n: line->path) {
            if (!first_set) {
                prev.setX(n->x);
                prev.setY(n->y);
                first_set = true;
                continue;
            }
            if (!second_set) {
                current.setX(n->x);
                current.setY(n->y);
                second_set = true;
                line_prev.setP1(prev);
                line_prev.setP2(current);
                auto angle = ((-line_prev.angle()) * M_PI / 180) + M_PI / 2;
                auto x = prev.x() + offset * qCos(angle);
                auto y = prev.y() + offset * qSin(angle);
                polyline.append(QPoint((int) std::round(x), (int) std::round(y)));
                continue;
            }
            next.setX(n->x);
            next.setY(n->y);
            line_next.setP1(current);
            line_next.setP2(next);
            auto angle_prev = ((-line_prev.angle()) * M_PI / 180) + M_PI / 2;
            auto angle_next = ((-line_next.angle()) * M_PI / 180) + M_PI / 2;
            auto avg_angle = (angle_prev + angle_next) / 2;
            auto x = current.x() + offset * qCos(avg_angle);
            auto y = current.y() + offset * qSin(avg_angle);
            polyline.append(QPoint((int) std::round(x), (int) std::round(y)));
            line_prev = line_next;
            prev = current;
            current = next;
        }
        auto angle = ((-line_prev.angle()) * M_PI / 180) + M_PI / 2;
        auto x = current.x() + offset * qCos(angle);
        auto y = current.y() + offset * qSin(angle);
        polyline.append(QPoint((int) std::round(x), (int) std::round(y)));
    }
    auto qpp = QPainterPath();
    qpp.addPolygon(polyline);
    setPath(qpp);
    setPen(pen);
    setZValue(marking->priority);
}
