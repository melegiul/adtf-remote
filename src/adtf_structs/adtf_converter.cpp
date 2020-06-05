#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "adtf_converter.h"

using namespace adtf;

static const std::string mediaType = "tDetectedLine";
static const char *mediaDescription = R"(
                                      <struct alignment="1" name="tDetectedLine" version="1">
                                          <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="arraySize" type="int"/>
                                          <element alignment="1" arraysize="arraySize" byteorder="LE" bytepos="5" name="detectedLine" type="DetectedLine"/>
                                      </struct>
                                   )";
static const uint length = 16;
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tCarOdometry2 tCarOdometry2::fromNetwork(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tCarOdometry2 message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("pos0", &message.posf[0]);
    pCoder->Get("pos1", &message.posf[1]);
    pCoder->Get("orientation", &message.orientation);
    pCoder->Get("timestamp", &message.timestamp);

    pCoder->End();
    return message;
}

ADTFMediaSample tCarOdometry2::toNetwork(tCarOdometry2 message, std::string pinName, uint64_t streamTime) {
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

    pCoder->Set("pos0", &message.posf[0]);
    pCoder->Set("pos1", &message.posf[1]);
    pCoder->Set("orientation", &message.orientation);
    pCoder->Set("timestamp", &message.timestamp);

    pCoder->End();
    return sample;
}
