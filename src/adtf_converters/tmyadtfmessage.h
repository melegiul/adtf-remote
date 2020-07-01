#include <cstdint>
#include <string>

#include "../adtfmediasample.h"

struct tHeaderStruct
{
    uint32_t ui32HeaderVal;
    double f64HeaderVal;
};

struct tSimpleStruct
{
    uint8_t ui8Val;
    uint16_t ui16Val;
    uint32_t ui32Val;
    int32_t i32Val;
    int64_t i64Val;
    double f64Val;
    float f32Val;
};

struct tMyADTFMessage
{
    tHeaderStruct sHeaderStruct;
    tSimpleStruct sSimpleStruct;
};

namespace adtf_converter {
    namespace from_network {
        tMyADTFMessage myADTFMessage(const ADTFMediaSample &sample);
    }
    namespace to_network {
        ADTFMediaSample myADTFMessage(tMyADTFMessage message, std::string pinName, uint64_t streamTime);
    }
}
