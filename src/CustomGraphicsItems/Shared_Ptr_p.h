//
// Created by uniautonom on 30.03.20.
//

#ifndef C___PROJEKT_SHARED_PTR_P_H
#define C___PROJEKT_SHARED_PTR_P_H

#include <memory>
#include "Map/NavigationMarker.hpp"

//shared pointer datatype needs to be declared before it can be used in Qt
//Since several classes need this macro a private header is used

Q_DECLARE_METATYPE(std::shared_ptr<NavigationMarker>)

#endif //C___PROJEKT_SHARED_PTR_P_H
