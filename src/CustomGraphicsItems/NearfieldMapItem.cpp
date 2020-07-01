//
// Created by uniautonom on 28.02.20.
//

#include <QtGui/QLayout>
#include "NearfieldMapItem.h"
#include "NearfieldCellItem.h"

NearfieldMapItem::NearfieldMapItem(MapTreeNode *root,QGraphicsItem *parent):QGraphicsItemGroup(parent){
    setupMap(root);
}
void NearfieldMapItem::setupMap(MapTreeNode *root, int width, int height, int xMid, int yMid) {
    if(root->getOccupancy() == 0)
    {

        int w2= width/2;
        int h2= height/2;
        if(root->NorthEastChild != nullptr)
            setupMap(root->NorthEastChild,w2,h2,xMid+w2/2,yMid-h2/2);
        if(root->NorthWestChild != nullptr)
            setupMap(root->NorthWestChild,w2,h2,xMid-w2/2,yMid-h2/2);
        if(root->SouthEastChild != nullptr)
            setupMap(root->SouthEastChild,w2,h2,xMid+w2/2,yMid+h2/2);
        if(root->SouthWestChild != nullptr)
            setupMap(root->SouthWestChild,w2,h2,xMid-w2/2,yMid+h2/2);
    }
    else
    {
        NearfieldCellItem* cell = new NearfieldCellItem(width,height,QPoint(xMid,yMid),root->getOccupancy(), this);
        addToGroup(cell);
        cell->setPos(xMid-width/2,yMid-height/2);
    }
}