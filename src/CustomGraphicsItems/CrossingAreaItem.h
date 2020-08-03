//
// Created by uniautonom on 01.10.19.
//

#ifndef AADC_USER_CROSSINGAREAITEM_H
#define AADC_USER_CROSSINGAREAITEM_H


#include <QGraphicsPolygonItem>
#include <Map/Crossing.h>
#include <Map/MapArea.hpp>

class CrossingAreaItem : public QGraphicsPolygonItem {
public:
    CrossingAreaItem(Crossing *crossing, MapArea *area, QGraphicsItem *parent);
    enum { Type = UserType + 1 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
private:
    Crossing *crossing;
    MapArea *area;
};


#endif //AADC_USER_CROSSINGAREAITEM_H
