//
// Created by uniautonom on 05.09.19.
//


#ifndef AADC_USER_GRAPHICS_VIEW_ZOOM_H
#define AADC_USER_GRAPHICS_VIEW_ZOOM_H

#include <QObject>
#include "fromQT/ui_PAF.h"
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>

class GraphicsViewZoom : public QObject {
Q_OBJECT
public:
    GraphicsViewZoom(QGraphicsView *view, QGraphicsView *rose, QGraphicsView *measure);

    void gentleZoom(double factor);

    void gentleRotate(double factor);

    void setModifiers(Qt::KeyboardModifiers &zoom, Qt::KeyboardModifiers &rotate);

    void setZoomFactorBase(double value);

    void adjustMeasure(qreal scale = 1);

    void setEnabled(bool enabled);

private:
    bool enabled;
    QGraphicsView *view;
    QGraphicsView *rose;
    QGraphicsView *measure;
    QGraphicsLineItem *measure_bar;
    QGraphicsSimpleTextItem *measure_text;
    QGraphicsRectItem *measure_background;

    Qt::KeyboardModifiers modifiers_zoom;
    Qt::KeyboardModifiers modifiers_rotate;
    double zoom_factor_base;
    double rotate_factor_base;

    char measure_units[10][7] = {" mm", " cm", "0 cm", " m", "0 m", "00 m", " km", "0 km", "00 km", "000 km"};
    int measure_unit_idx = 2;
    int measure_value = 1;  // should be 1, 2, or 5

    QPointF target_scene_pos, target_viewport_pos;

    bool eventFilter(QObject *object, QEvent *event) override;

signals:

    void zoomed();

    void rotated(double factor);

    void mousePositionUpdated(QPointF position);
};


#endif //AADC_USER_GRAPHICS_VIEW_ZOOM_H
