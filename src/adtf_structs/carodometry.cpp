#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "carodometry.h"

using namespace adtf;

static const std::string mediaType = "tCarOdometry";
static const char *mediaDescription = R"(
                                  <struct alignment="1" name="tCarOdometry" version="1">
                                      <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="pos0" type="tFloat32"/>
                                      <element alignment="1" arraysize="1" byteorder="LE" bytepos="4" name="pos1" type="tFloat32"/>
                                      <element alignment="1" arraysize="1" byteorder="LE" bytepos="8" name="orientation" type="tFloat32"/>
                                      <element alignment="1" arraysize="1" byteorder="LE" bytepos="12" name="timestamp" type="tUInt32"/>
                                  </struct>
                                   )";
static const uint length = 16;
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tCarOdometry tCarOdometry::fromNetwork(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tCarOdometry message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("pos0", &message.pos0);
    pCoder->Get("pos1", &message.pos1);
    pCoder->Get("orientation", &message.orientation);
    pCoder->Get("timestamp", &message.timestamp);

    pCoder->End();
    return message;
}

ADTFMediaSample tCarOdometry::toNetwork(tCarOdometry message, std::string pinName, uint64_t streamTime) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    ADTFMediaSample sample;
    sample.mediaType = mediaType;
    sample.length = length;
    sample.pinName = pinName;
    sample.streamTime = streamTime;
    sample.data = std::shared_ptr<char[]>(new char[length]);
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Set("pos0", &message.pos0);
    pCoder->Set("pos1", &message.pos1);
    pCoder->Set("orientation", &message.orientation);
    pCoder->Set("timestamp", &message.timestamp);

    pCoder->End();
    return sample;
}
