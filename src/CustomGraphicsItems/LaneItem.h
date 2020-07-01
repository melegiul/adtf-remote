//
// Created by uniautonom on 22.09.19.
//

#ifndef AADC_USER_LANEITEM_H
#define AADC_USER_LANEITEM_H

#include <Map/Lane.hpp>
#include <QtGui/QGraphicsPolygonItem>
#include <Map/ContentManager.hpp>
#include <QtGui/QPen>
#include <QtGui/QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include "Shared_Ptr_p.h"
#include "../ItemSignalController.h"



class LaneItem : public QGraphicsPolygonItem {
public:
    LaneItem(Lane *lane, QGraphicsItem *parent = 0);

    bool isEqual(Lane *lane);

    enum {
        Type = UserType + 4
    };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }
    Lane *lane;

    void colorRegular();
    void colorSelected();
    void colorCarOnTop();
    void propagateMouseEventToLaneItem(QGraphicsSceneContextMenuEvent *event);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

#endif //AADC_USER_LANEITEM_H