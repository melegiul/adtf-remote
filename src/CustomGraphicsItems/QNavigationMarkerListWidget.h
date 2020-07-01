//
// Created by uniautonom on 17.03.20.
//

#ifndef C___PROJEKT_QNAVIGATIONMARKERLISTWIDGET_H
#define C___PROJEKT_QNAVIGATIONMARKERLISTWIDGET_H


#include <QListWidget>
#include <QMouseEvent>
#include <QApplication>
#include <Map/ContentManager.hpp>
#include "QNavigationMarkerListWidgetItem.h"
#include "GUIHelper/ItemSignalController.h"


class QNavigationMarkerListWidget : public QListWidget {

    Q_OBJECT

private:
    int rowOldItem;
    bool permissionForDragAndDrop = false;
    QPoint startPos;
    QNavigationMarkerListWidgetItem *navMarkerListWidgetItem = nullptr;
    NavigationMarkerItem *navMarkerItem = nullptr;
    QString text;

public:
    QNavigationMarkerListWidget(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event) override ;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void on_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
};

#endif //C___PROJEKT_QNAVIGATIONMARKERLISTWIDGET_H
