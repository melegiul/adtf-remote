//
// Created by uniautonom on 26.02.20.
//

#include "NearfieldCellItem.h"
#include "../../../NearfieldGrid/MapConstants.h"
#include <QBrush>
#include <QPen>

NearfieldCellItem::NearfieldCellItem(int length, int height, QPoint mid, int occupancy, QGraphicsItem *parent)
        :QGraphicsPolygonItem(parent),length(length),height(height),mid(mid),occupancy(occupancy){

    setupColor();

    QPolygon outline;
    outline << QPoint(0,0) << QPoint(length,0) << QPoint(length,height) << QPoint(0,height);
    /*
        outline << QPoint(mid.x()-length/2,mid.y()-height/2) <<
    QPoint(mid.x()+length/2,mid.y()-height/2) <<
    QPoint(mid.x()+length/2,mid.y()+height/2) <<
    QPoint(mid.x()-length/2,mid.y()+height/2);
*/
    setPolygon(outline);

    QString tip("Occupancy: ");
    tip.append(QString::number(occupancy));
    tip.append(" MidPoint: (");
    tip.append(QString::number(mid.x()));
    tip.append(",");
    tip.append(QString::number(mid.y()));
    tip.append(")");

    setBrush(QBrush(color, Qt::SolidPattern));
    setPen(QPen(color));
    setToolTip(tip);
}

void NearfieldCellItem::setupColor() {
    //continuous color gradient
    //occupancy max = 200 => 200 different Colors
    //low (255,255,255)=white to high (0,255,0)=green occupancy
    //color = QColor(MAX_OCCUPANCY-occupancy,255,MAX_OCCUPANCY-occupancy,transparency);

    //gradual gradient
    if(occupancy>80)
    color = QColor(MAX_OCCUPANCY-occupancy,255,MAX_OCCUPANCY-occupancy);
    else if(occupancy == 80)
        color =QColor(255-80,255,255-80);
    else if(occupancy>60)
        color =QColor(255-60,255,255-60);
    else if(occupancy>40)
        color=QColor(255-40,255,255-40);
    else
        color=QColor(255-20,255,255-20);
}
