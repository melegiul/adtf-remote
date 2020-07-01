#ifndef __RESIZEGRAPHICSVIEW_H
#define __RESIZEGRAPHICSVIEW_H

#include <QGraphicsView>

class QResizeEvent;

class ResizeGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ResizeGraphicsView(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void resized();
};

#endif // __RESIZEGRAPHICSVIEW_H
