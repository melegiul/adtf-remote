//
// Created by uniautonom on 20.02.20.
//

#include "NearfieldGridItem.h"

NearfieldGridItem::NearfieldGridItem(QGraphicsItem *parent, int grid_height, int grid_width, int cell_height, int cell_width): QGraphicsItemGroup(parent), grid_height(grid_height), grid_width(grid_width), cell_height(cell_height),cell_width(cell_width) {
    setupGrid();

    QString tip("Ultrasonic- and LiDAR-Results");
    setToolTip(tip);

}

void NearfieldGridItem::setupGrid() {


        for(double i = 0; i <= grid_height; i += cell_height){
            addToGroup(new QGraphicsLineItem(0,i,grid_width,i,this));
        }
    for(double i = 0; i <= grid_width; i += cell_width){
        addToGroup(new QGraphicsLineItem(i,0,i,grid_height,this));
    }
    setZValue(0);
}


