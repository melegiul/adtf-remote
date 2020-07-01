//
// Created by uniautonom on 20.02.20.
//

#ifndef AADC_USER_NEARFIELDGRIDITEM_H
#define AADC_USER_NEARFIELDGRIDITEM_H

#include <QGraphicsItemGroup>
#include "TreeNodeItem.h"
#include "../../../NearfieldGrid/MapConstants.h"

class NearfieldGridItem : public QGraphicsItemGroup {
public:
    NearfieldGridItem(QGraphicsItem *parent = 0, int grid_height = GRIDMAP_HEIGHT, int grid_width = GRIDMAP_WIDTH, int cell_height = GRIDCELL_HEIGHT, int cell_width = GRIDCELL_WIDTH);
private:
    int grid_height, grid_width, cell_height, cell_width; //in pixel
    void setupGrid();
};

#endif //AADC_USER_NEARFIELDGRIDITEM_H
