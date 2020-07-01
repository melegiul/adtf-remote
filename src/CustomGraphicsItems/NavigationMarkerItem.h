//
// Created by uniautonom on 09.03.20.
//

#ifndef AADC_USER_NAVIGATIONMARKERITEM_H
#define AADC_USER_NAVIGATIONMARKERITEM_H


#include <Map/NavigationMarker.hpp>
#include <Map/Lane.hpp>
#include <QtGui/QGraphicsItem>
#include <memory>
#include <QtGui/QPen>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <iostream>
#include <QtGui/QBrush>
#include "LaneItem.h"
#include "SpecialMarkingItem.h"
#include "../ItemSignalController.h"

class NavigationMarkerItem : public QGraphicsEllipseItem {
public:
    NavigationMarkerItem(std::shared_ptr<NavigationMarker> navMarker, QGraphicsItem *parent = nullptr);

    ~NavigationMarkerItem();

    void setToolTipMarker();

    void setENavType(eNavType type);

    std::shared_ptr<NavigationMarker> getNavMarker();

    enum { Type = UserType + 9 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    void highlight();

    void unHighlight();

    bool isChanged() const;

    int getX() const;

    int getY() const;

private:
    std::shared_ptr<NavigationMarker> navMarker;
    int width = 512;
    int height = 512;
    bool changed = false;
    bool executeDrag = false;
    int x = 0;
    int y = 0;
    QPointF startPos;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif //AADC_USER_NAVIGATIONMARKERITEM_H
