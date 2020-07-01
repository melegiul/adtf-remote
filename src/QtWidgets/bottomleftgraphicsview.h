#ifndef __BOTTOMLEFTGRAPHICSVIEW_H
#define __BOTTOMLEFTGRAPHICSVIEW_H

#include <QGraphicsView>

class BottomLeftGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit BottomLeftGraphicsView(QWidget *parent = nullptr);

public slots:
    void resize();

private:
    QWidget *parent;
};

#endif // __BOTTOMLEFTGRAPHICSVIEW_H
