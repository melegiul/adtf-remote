//
// Created by uniautonom on 20.09.19.
//

#include "ItemSignalController.h"


void ItemSignalController::sendSignClicked(StreetSign *s, Lane *l) {
    emit signClicked(s, l);
}

void ItemSignalController::sendAddSignClicked(StreetSign *s, Lane *l) {
    emit addSignClicked(s, l);
}

void ItemSignalController::sendUpdateMap() {
    emit updateMap();
}

void ItemSignalController::sendAddNavigationMarkerClicked(std::shared_ptr<NavigationMarker> &navMarker) {
    emit addNavigationMarkerClicked(navMarker);
}

void ItemSignalController::sendMarkerMoved(NavigationMarkerItem * navMarkerItem) {
    emit markerMoved(navMarkerItem);
}

void ItemSignalController::sendUpdateMarkerTypesInMarkerList() {
    emit markerUpdateRequired();
}

void ItemSignalController::sendDeleteMarker() {
    emit deleteMarker();
}


