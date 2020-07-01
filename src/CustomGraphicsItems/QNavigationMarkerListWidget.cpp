//
// Created by uniautonom on 17.03.20.
//

#include "QNavigationMarkerListWidget.h"

QNavigationMarkerListWidget::QNavigationMarkerListWidget(QWidget *parent):
    QListWidget(parent) {
}

void QNavigationMarkerListWidget::mousePressEvent(QMouseEvent *event){
    permissionForDragAndDrop = false;
    if(event->button() == Qt::LeftButton){
        navMarkerListWidgetItem = dynamic_cast<QNavigationMarkerListWidgetItem *>(itemAt(event->pos()));
        if(navMarkerListWidgetItem != nullptr) {
            setCurrentItem(navMarkerListWidgetItem);
            rowOldItem = currentRow();
            navMarkerItem = navMarkerListWidgetItem->getNavMarkerItem();
            text = navMarkerListWidgetItem->text();
            startPos = event->pos();
            unHighlight();
            navMarkerListWidgetItem->getNavMarkerItem()->highlight();
        }
    } else if(event->button() == Qt::RightButton){
        navMarkerListWidgetItem = dynamic_cast<QNavigationMarkerListWidgetItem *>(itemAt(event->pos()));
        if(navMarkerListWidgetItem != nullptr) {
            setCurrentItem(navMarkerListWidgetItem);
            ItemSignalController::getInstance().sendDeleteMarker();
        }
    }
    QListWidget::mousePressEvent(event);
}

void QNavigationMarkerListWidget::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons() & Qt::LeftButton){
        int distance = (event->pos() - startPos).manhattanLength(); //determines the minimal distance that cursor has to be moved before an action happens
        if (distance >= QApplication::startDragDistance()){
            permissionForDragAndDrop = true;
        }
        QNavigationMarkerListWidgetItem *item = dynamic_cast<QNavigationMarkerListWidgetItem *>(itemAt(event->pos()));;
        setCurrentItem(item);
    }
    QListWidget::mouseMoveEvent(event);
}

void QNavigationMarkerListWidget::mouseReleaseEvent(QMouseEvent *event){
        if(event->button() == Qt::LeftButton && permissionForDragAndDrop) {

            takeItem(rowOldItem);
            delete navMarkerListWidgetItem;
            navMarkerListWidgetItem = dynamic_cast<QNavigationMarkerListWidgetItem *>(itemAt(event->pos()));
            setCurrentItem(navMarkerListWidgetItem);
            int row = currentRow();

            // the last row in a ListWidget has the number -1
            if (row == -1) {
                row = count();
            }

            navMarkerListWidgetItem = new QNavigationMarkerListWidgetItem(text, nullptr, navMarkerItem);
            insertItem(row, navMarkerListWidgetItem);
            navMarkerListWidgetItem = nullptr;
            ContentManager::getInstance().swapNavigationMarker(rowOldItem, row);
            ItemSignalController::getInstance().sendUpdateMarkerTypesInMarkerList();


        }
    QListWidget::mousePressEvent(event);
}

void QNavigationMarkerListWidget::unHighlight() {
    for(int i = 0; i < this->count(); ++i)
    {
        QNavigationMarkerListWidgetItem* item = dynamic_cast<QNavigationMarkerListWidgetItem *>(this->item(i));
        if(item){
            item->getNavMarkerItem()->unHighlight();
        }
    }
}