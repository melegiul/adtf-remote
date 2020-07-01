//
// Created by uniautonom on 26.02.20.
//

#ifndef AADC_USER_NEARFIELDCELLITEM_H
#define AADC_USER_NEARFIELDCELLITEM_H


#include <QtGui/QGraphicsPolygonItem>

class NearfieldCellItem:public QGraphicsPolygonItem {
public:
    NearfieldCellItem(int length, int height, QPoint mid, int occupancy, QGraphicsItem* parent = 0);
private:
    int length,height,occupancy;
    QPoint mid;
    QColor color;

    void setupColor();
};


#endif //AADC_USER_NEARFIELDCELLITEM_H
