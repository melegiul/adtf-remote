//
// Created by uniautonom on 28.02.20.
//

#ifndef AADC_USER_NEARFIELDMAPITEM_H
#define AADC_USER_NEARFIELDMAPITEM_H

#include <QGraphicsItemGroup>
#include "../../../NearfieldGrid/MapTreeNode.h"
#include "../../../NearfieldGrid/MapConstants.h"

class NearfieldMapItem: public  QGraphicsItemGroup{
public:
    NearfieldMapItem(MapTreeNode* root,QGraphicsItem *parent = 0);
private:
    void setupMap(MapTreeNode *root, int width = GRIDMAP_WIDTH, int height = GRIDMAP_HEIGHT, int xMid = GRIDMAP_WIDTH/2, int yMid =GRIDMAP_HEIGHT/2);
};


#endif //AADC_USER_NEARFIELDMAPITEM_H
