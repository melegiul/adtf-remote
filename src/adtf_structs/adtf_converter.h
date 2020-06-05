#ifndef __ADTF_CONVERTER_H
#define __ADTF_CONVERTER_H


namespace adtf_converter {
    tCarOdometry2 fromNetwork(const ADTFMediaSample &sample);
    ADTFMediaSample toNetwork(tCarOdometry2 message, std::string pinName, uint64_t streamTime);
}

#endif // __ADTF_CONVERTER_H
