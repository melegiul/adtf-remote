//
// Created by uniautonom on 01.10.19.
//

#ifndef AADC_USER_INVISIBLELINEITEM_H
#define AADC_USER_INVISIBLELINEITEM_H


#include <QtGui/QGraphicsPathItem>
#include <Map/Line.hpp>

class InvisibleLineItem : public QGraphicsPathItem {
public:
    InvisibleLineItem(Line *line, int width, QGraphicsItem *parent);
    enum { Type = UserType + 2 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
private:
    Line *line;
    int width;
};


#endif //AADC_USER_INVISIBLELINEITEM_H
