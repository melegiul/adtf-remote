//
// Created by uniautonom on 16.03.20.
//

#ifndef C___PROJEKT_QNAVIGATIONMARKERLISTWIDGETITEM_H
#define C___PROJEKT_QNAVIGATIONMARKERLISTWIDGETITEM_H


#include <QListWidgetItem>
#include "NavigationMarkerItem.h"

class QNavigationMarkerListWidgetItem : public QListWidgetItem {
private:
    NavigationMarkerItem *navMarkerItem;

public:
    QNavigationMarkerListWidgetItem(const QString &text, QListWidget *parent,
                                    NavigationMarkerItem *navMarkerItem);

    ~QNavigationMarkerListWidgetItem() override;

    enum {
        Type = UserType + 10
    };

    int type() const {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    void setENavType(eNavType type);
    NavigationMarkerItem *getNavMarkerItem();
};

#endif //C___PROJEKT_QNAVIGATIONMARKERLISTWIDGETITEM_H
