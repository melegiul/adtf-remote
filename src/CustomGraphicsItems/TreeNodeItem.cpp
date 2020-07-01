//
// Created by uniautonom on 04.10.19.
//

#include "TreeNodeItem.h"

QRectF TreeNodeItem::boundingRect() const {
    return {};
}

void TreeNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {}

TreeNodeItem::TreeNodeItem(QGraphicsItem *parent) : QGraphicsItem(parent){}
