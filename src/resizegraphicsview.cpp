#include "resizegraphicsview.h"

ResizeGraphicsView::ResizeGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
}

void ResizeGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    emit resized();
}
