//
// Created by uniautonom on 20.09.19.
//

#ifndef AADC_USER_ITEMSIGNALCONTROLLER_H
#define AADC_USER_ITEMSIGNALCONTROLLER_H


#include <QObject>
#include <Map/StreetSign.hpp>
#include <Map/Lane.hpp>
#include <Map/NavigationMarker.hpp>
#include "CustomGraphicsItems/NavigationMarkerItem.h"
#include <memory>

class NavigationMarkerItem;

class ItemSignalController : public QObject {
Q_OBJECT
private:
    ItemSignalController() {};
public:
    static ItemSignalController &getInstance() {
        static ItemSignalController instance;
        return instance;
    }

    ItemSignalController(ItemSignalController const &) = delete;

    void operator=(ItemSignalController const &) = delete;

public:
    void sendSignClicked(StreetSign *s, Lane *l);

    void sendAddSignClicked(StreetSign *s, Lane *l);

    void sendAddNavigationMarkerClicked(std::shared_ptr<NavigationMarker> &navMarker);

    void sendMarkerMoved(NavigationMarkerItem * navMarkerItem);

    void sendUpdateMarkerTypesInMarkerList();

    void sendDeleteMarker();

    void sendUpdateMap();



signals:
    void signClicked(StreetSign *s, Lane *l);

    void addSignClicked(StreetSign *s, Lane *l);

    void updateMap();

    void addNavigationMarkerClicked(std::shared_ptr<NavigationMarker> &navMarker);

    void markerMoved(NavigationMarkerItem *navMarkerItem);

    void markerUpdateRequired();

    void deleteMarker();
};

#endif //AADC_USER_ITEMSIGNALCONTROLLER_H
