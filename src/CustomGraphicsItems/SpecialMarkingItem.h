//
// Created by uniautonom on 01.10.19.
//

#ifndef AADC_USER_SPECIALMARKINGITEM_H
#define AADC_USER_SPECIALMARKINGITEM_H


#include <QtGui/QGraphicsPolygonItem>
#include <QtGui/QMenu>
#include <QPixmap>
#include <QtGui/QPen>
#include <QGraphicsSceneContextMenuEvent>
#include "../ItemSignalController.h"
#include "Map/SpecialMarking.hpp"
#include "Shared_Ptr_p.h"


class SpecialMarkingItem : public QGraphicsPolygonItem {
public:
    explicit SpecialMarkingItem(SpecialMarking *m, QPolygon outline);

    enum { Type = UserType + 5 };

    int type() const override
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }


private:
    SpecialMarking *m;
public:
    SpecialMarking *getM() const;

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

#endif //AADC_USER_SPECIALMARKINGITEM_H
