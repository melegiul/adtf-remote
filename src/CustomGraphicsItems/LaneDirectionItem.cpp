//
// Created by uniautonom on 01.10.19.
//

#include <cmath>

#include "Map/Line.hpp"
#include "Map/Node.hpp"
#include "LaneDirectionItem.h"

LaneDirectionItem::LaneDirectionItem(Line *left, Line *right, int size, QGraphicsItem *parent) : QGraphicsPixmapItem(
        parent), left(left), right(right), size(size) {
    QPixmap arrow(":/arrow.png");
    arrow = arrow.scaled(size, size);

    auto left_start = left->path.front();
    auto right_start = right->path.front();
    auto right_end = right->path.back();
    // The right line can go in the opposite direction. In this case, the last point needs to be used.
    double d_end = std::sqrt(std::pow(left_start->x - right_end->x, 2) + std::pow(left_start->y - right_end->y, 2));
    double d_start = std::sqrt(
            std::pow(left_start->x - right_start->x, 2) + std::pow(left_start->y - right_start->y, 2));
    // find angle of lane
    QLineF l;
    l.setP1(QPoint(left_start->x, left_start->y));
    int x_new, y_new;
    setPixmap(arrow);
    if (d_end < d_start) {
        l.setP2(QPoint(right_end->x, right_end->y));
        x_new = (left_start->x + right_end->x) / 2;
        y_new = (left_start->y + right_end->y) / 2;

    } else {
        l.setP2(QPoint(right_start->x, right_start->y));
        x_new = (left_start->x + right_start->x) / 2;
        y_new = (left_start->y + right_start->y) / 2;

    }
    auto angle = l.angle();
    setPos(x_new - size / 2., y_new - size / 2.);
    setTransformOriginPoint(arrow.rect().center());
    setRotation(-angle);


}
