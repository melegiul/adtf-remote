#include "bottomleftgraphicsview.h"

BottomLeftGraphicsView::BottomLeftGraphicsView(QWidget *parent) : QGraphicsView(parent), parent(parent)
{
}

void BottomLeftGraphicsView::resize()
{
    this->move(0, this->parent->size().height() - this->size().height());
}
