#ifndef __ADTFMEDIASAMPLE_H
#define __ADTFMEDIASAMPLE_H

#include <string>
#include <memory>
#include <cstdint>

struct ADTFMediaSample {
    std::string pinName;
    std::string mediaType;
    uint64_t streamTime;
    std::shared_ptr<char[]> data;
    uint length;
};

#endif // __ADTFMEDIASAMPLE_H
