//
// Created by uniautonom on 01.10.19.
//

#ifndef AADC_USER_VISIBLELINEITEM_H
#define AADC_USER_VISIBLELINEITEM_H


#include <QGraphicsPathItem>
#include <Map/LineMarking.hpp>
#include <Map/Line.hpp>

class VisibleLineItem : public QGraphicsPathItem {
public:
    VisibleLineItem(LineMarking *marking, Line *line, int offset, int line_width, QGraphicsItem *parent);
    enum { Type = UserType + 8 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
private:
    LineMarking *marking;
    Line *line;
    int offset;
    int line_width;
};


#endif //AADC_USER_VISIBLELINEITEM_H
