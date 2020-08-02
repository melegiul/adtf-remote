//
// Created by uniautonom on 01.08.20.
//

#ifndef UNIAUTONOM_REMOTEGUI_MEDIADESCIPTIONSINGLETON_H
#define UNIAUTONOM_REMOTEGUI_MEDIADESCIPTIONSINGLETON_H


#include <string>

class mediaDesciptionSingleton {
    private:

    mediaDesciptionSingleton() = default;

    std::string description;

    public:
        static mediaDesciptionSingleton &getInstance() {
            static mediaDesciptionSingleton instance;
            return instance;
        }

        mediaDesciptionSingleton(mediaDesciptionSingleton const &) = delete;

        void operator=(mediaDesciptionSingleton const &) = delete;

        void loadDescription(std::string file);

        std::string  getMediaDescription();

    };

#endif //UNIAUTONOM_REMOTEGUI_MEDIADESCIPTIONSINGLETON_H
