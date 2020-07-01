//
// Created by uniautonom on 01.10.19.
//

#ifndef AADC_USER_LANEDIRECTIONITEM_H
#define AADC_USER_LANEDIRECTIONITEM_H


#include <QGraphicsPixmapItem>

class Line;

class LaneDirectionItem : public QGraphicsPixmapItem {
public:
    LaneDirectionItem(Line *left, Line *right, int size, QGraphicsItem *parent = 0);
    enum { Type = UserType + 3 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
private:
    Line *left, *right;
    int size;
};


#endif //AADC_USER_LANEDIRECTIONITEM_H
