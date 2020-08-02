//
// Created by uniautonom on 01.08.20.
//

#include <fstream>
#include <iostream>
#include "mediaDesciptionSingleton.h"

void mediaDesciptionSingleton::loadDescription(std::string file) {

    std::string line,text;
    std::ifstream in(file);
    while(std::getline(in, line))
    {
        text += line + "\n";
    }
    description = text;
}

std::string mediaDesciptionSingleton::getMediaDescription() {
        return description;
}
