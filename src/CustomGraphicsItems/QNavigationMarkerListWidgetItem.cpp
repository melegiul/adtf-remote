//
// Created by uniautonom on 16.03.20.
//

#include "QNavigationMarkerListWidgetItem.h"

QNavigationMarkerListWidgetItem::QNavigationMarkerListWidgetItem(const QString &text,
                                                                 QListWidget *parent, NavigationMarkerItem *navMarkerItem) :
        QListWidgetItem(parent), navMarkerItem(navMarkerItem) {
    this->setText(text);
}

void QNavigationMarkerListWidgetItem::setENavType(eNavType type) {
    if(type == Intermediate) {
        navMarkerItem->setENavType(Intermediate);
    } else if(type == Final){
        navMarkerItem->setENavType(Final);
    }
}

QNavigationMarkerListWidgetItem::~QNavigationMarkerListWidgetItem() {
}

NavigationMarkerItem *QNavigationMarkerListWidgetItem::getNavMarkerItem() {
    return navMarkerItem;
}

