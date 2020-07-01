//
// Created by uniautonom on 20.09.19.
//

#ifndef AADC_USER_STREETSIGNITEM_H
#define AADC_USER_STREETSIGNITEM_H

#include <QGraphicsObject>
#include <QtGui/QGraphicsEllipseItem>
#include <Map/StreetSign.hpp>
#include <Map/Lane.hpp>

class StreetSignItem : public QGraphicsPixmapItem {


public:
    StreetSignItem(StreetSign *sign, Lane *lane, int max_size, QGraphicsItem *parent = nullptr);

    bool isEqual(StreetSign *sign);

    void enableEditing();

    void disableEditing();
    enum { Type = UserType + 6 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
private:
    StreetSign *sign;
    Lane *lane;
    int max_size;
    int width;
    int height;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;


signals:

    void signClicked(StreetSign *s, Lane *l);

};


#endif //AADC_USER_STREETSIGNITEM_H
