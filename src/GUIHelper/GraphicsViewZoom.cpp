//
// Created by uniautonom on 05.09.19.
//

#include "GraphicsViewZoom.h"
#include "ItemSignalController.h"
#include <QMouseEvent>
#include <qmath.h>

GraphicsViewZoom::GraphicsViewZoom(QGraphicsView *view, QGraphicsView *rose, QGraphicsView *measure)
        : QObject(view), view(view), rose(rose), measure(measure) {
    view->viewport()->installEventFilter(this);
    view->setMouseTracking(true);

    QPixmap rose_img(":/compass.png");
    rose_img = rose_img.scaled(140, 140);
    auto rose_item = new QGraphicsPixmapItem(rose_img);
    rose_item->setTransformOriginPoint(rose_item->boundingRect().center());
    auto scene_rose = new QGraphicsScene();
    scene_rose->addItem(rose_item);
    rose->setScene(scene_rose);
    rose->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rose->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rose->setEnabled(false);

    measure_bar = new QGraphicsLineItem(0, 0, 100, 0);
    measure_bar->setPen(QPen(Qt::black, 10, Qt::SolidLine, Qt::FlatCap));
    measure_text = new QGraphicsSimpleTextItem();
    measure_text->setFont(QFont("Helvetica", 14));
    measure_background = new QGraphicsRectItem();
    measure_background->setPen(QPen(QColor(0, 0, 0, 0)));
    measure_background->setBrush(QBrush(QColor(255, 255, 255, 90)));
    measure_background->setZValue(-1);
    auto measure_scene = new QGraphicsScene();
    measure_scene->addItem(measure_bar);
    measure_scene->addItem(measure_text);
    measure_scene->addItem(measure_background);
    measure->setScene(measure_scene);
    measure->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    measure->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    measure->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    measure->setEnabled(false);

    modifiers_zoom = Qt::ControlModifier;
    modifiers_rotate = Qt::ShiftModifier;
    zoom_factor_base = 1.0015;
    rotate_factor_base = 120;
    adjustMeasure();
}

void GraphicsViewZoom::gentleZoom(double factor) {
    if(!enabled) return;
    bool too_close = measure_unit_idx == 0 && measure_value == 1 && factor > 1;
    bool too_far_away = measure_unit_idx == 9 && measure_value == 5 && factor < 1;
    if (!too_close && !too_far_away) {
        view->scale(factor, factor);
        adjustMeasure(factor);
        QPointF delta_viewport_pos = target_viewport_pos - QPointF(view->viewport()->width() / 2.0,
                                                                   view->viewport()->height() / 2.0);
        QPointF viewport_center = view->mapFromScene(target_scene_pos) - delta_viewport_pos;
        view->centerOn(view->mapToScene(viewport_center.toPoint()));

        emit zoomed();
    }
}

void GraphicsViewZoom::gentleRotate(double factor) {
    if(!enabled) return;
    view->rotate(factor);
    rose->rotate(factor);
    QPointF delta_viewport_pos = target_viewport_pos - QPointF(view->viewport()->width() / 2.0,
                                                               view->viewport()->height() / 2.0);
    QPointF viewport_center = view->mapFromScene(target_scene_pos) - delta_viewport_pos;
    view->centerOn(view->mapToScene(viewport_center.toPoint()));

    emit rotated(factor);
}

void GraphicsViewZoom::setModifiers(Qt::KeyboardModifiers &zoom, Qt::KeyboardModifiers &rotate) {
    modifiers_zoom = zoom;
    modifiers_rotate = rotate;
}

void GraphicsViewZoom::setZoomFactorBase(double value) {
    zoom_factor_base = value;
}

bool GraphicsViewZoom::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouse_event = dynamic_cast<QMouseEvent *>(event);
        QPointF delta = target_viewport_pos - mouse_event->pos();
        if (qAbs(delta.x()) > 1 || qAbs(delta.y()) > 1) {
            target_viewport_pos = mouse_event->pos();
            target_scene_pos = view->mapToScene(mouse_event->pos());
            emit mousePositionUpdated(target_scene_pos);
        }
    } else if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheel_event = dynamic_cast<QWheelEvent *>(event);
        if (QApplication::keyboardModifiers() == modifiers_zoom) {
            if (wheel_event->orientation() == Qt::Vertical) {
                double angle = wheel_event->delta();
                double factor = qPow(zoom_factor_base, angle);

                gentleZoom(factor);
                return true;
            }
        } else if (QApplication::keyboardModifiers() == modifiers_rotate) {
            if (wheel_event->orientation() == Qt::Vertical) {
                double angle = wheel_event->delta();
                gentleRotate(angle / rotate_factor_base);
                return true;
            }
        }
        return true;
    } else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        // Suppressed to disable scrolling with keys
        return true;
    } else if(event->type() == QEvent::MouseButtonDblClick){
        // Deselect sign when double-clicking elsewhere
        ItemSignalController::getInstance().sendSignClicked(nullptr, nullptr);
    }
    Q_UNUSED(object)
    return false;
}

void GraphicsViewZoom::adjustMeasure(qreal scale) {
    if (scale == 1) {
        // norm the bar to standard scale
        measure_bar->setLine(0, 0, 100, 0);
        measure_unit_idx = 2;
        measure_value = 1;
        QTransform trans = view->transform();
        scale = sqrt(trans.m11() * trans.m11() + trans.m12() * trans.m12());
    }

    qreal bar_length = measure_bar->line().length() * scale;

    // increase measure unit and bar length if bar gets too short
    while (bar_length < 0.5 * measure->width() && !(measure_unit_idx == 9 && measure_value == 5)) {
        if (measure_value == 1) {
            measure_value = 2;
            bar_length *= 2;
        } else if (measure_value == 2) {
            measure_value = 5;
            bar_length *= 2.5;
        } else if (measure_value == 5) {
            measure_value = 1;
            bar_length *= 2;
            measure_unit_idx++;
        }
    }
    // decrease measure unit and bar length if bar gets too long
    while (bar_length > 0.6 * measure->width() && !(measure_unit_idx == 0 && measure_value == 1)) {
        if (measure_value == 5) {
            measure_value = 2;
            bar_length /= 2.5;
        } else if (measure_value == 2) {
            measure_value = 1;
            bar_length /= 2;
        } else if (measure_value == 1) {
            measure_value = 5;
            bar_length /= 2;
            measure_unit_idx--;
        }
    }

    qreal padding_left = 20;
    qreal padding_bottom = 25;

    measure_bar->setLine(padding_left, padding_bottom, padding_left + bar_length, padding_bottom);

    QString text = QString::number(measure_value);
    text.append(measure_units[measure_unit_idx]);
    measure_text->setText(text);
    measure_text->setPos(padding_left + bar_length + padding_left, padding_bottom - measure_text->font().pointSize());

    QRectF background_rect = measure_bar->boundingRect();
    measure_text->boundingRect().width();
    // These numbers are not magic, but beautiful (add padding that looks good)
    background_rect.adjust(-10, -12, 10 + measure_text->boundingRect().width() + padding_left, 5);
    measure_background->setRect(background_rect);
}

void GraphicsViewZoom::setEnabled(bool en) {
    enabled = en;
}
