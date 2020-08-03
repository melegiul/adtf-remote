//
// Created by uniautonom on 04.10.19.
//

#ifndef AADC_USER_TREENODEITEM_H
#define AADC_USER_TREENODEITEM_H


#include <QGraphicsItem>

class TreeNodeItem: public QGraphicsItem {
    // an empty item with no functionality to construct a tree
public:
    explicit TreeNodeItem(QGraphicsItem * parent=0);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    enum { Type = UserType + 7 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
};


#endif //AADC_USER_TREENODEITEM_H
