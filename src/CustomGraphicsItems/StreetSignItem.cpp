//
// Created by uniautonom on 20.09.19.
//

#include <iostream>
#include "StreetSignItem.h"
#include "../ItemSignalController.h"
#include <QPainter>
#include "QGraphicsScene"
#include <QGraphicsView>

StreetSignItem::StreetSignItem(StreetSign *sign, Lane *lane, int max_size, QGraphicsItem *parent) :
        QGraphicsPixmapItem(parent), sign(sign), lane(lane), max_size(max_size) {
    QString filename = ":/";
    filename.append(enumStrings<eStreetSigns>::data[sign->sign]);
    filename.append(".png");
    QPixmap sign_img(filename);
    sign_img = sign_img.scaled(max_size, max_size, Qt::KeepAspectRatio);
    width = sign_img.width();
    height = sign_img.height();
    setPixmap(sign_img);

    QString tip("Sign at Lane: ");
    tip.append(std::to_string(lane->getId()).c_str());
    tip.append("\nType: ");
    tip.append(enumStrings<eStreetSigns>::data[sign->sign]);
    setToolTip(tip);

    setPos(sign->location->x - width / 2., sign->location->y - height / 2.);
}

bool StreetSignItem::isEqual(StreetSign *sign) {
    return sign == this->sign;
}

void StreetSignItem::enableEditing() {

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void StreetSignItem::disableEditing() {

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
}

void StreetSignItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    ItemSignalController::getInstance().sendSignClicked(sign, lane);
    QGraphicsItem::mousePressEvent(event);
}

QVariant StreetSignItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {

    if (change == ItemPositionChange && scene()) {
        sign->location->x = (int) std::round(pos().x() + width / 2.);
        sign->location->y = (int) std::round(pos().y() + height / 2.);
        ItemSignalController::getInstance().sendSignClicked(sign, lane);
    }
    return QGraphicsItem::itemChange(change, value);
}




