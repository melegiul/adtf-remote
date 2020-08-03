#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "speed.h"
#include "mediaDesciptionSingleton.h"

using namespace adtf;

static const std::string mediaType = "tSpeed";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tSpeed adtf_converter::from_network::speed(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDesciptionSingleton::getInstance().getMediaDescription().c_str(), IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tSpeed message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("combinedSpeed", &message.combinedSpeed);
    pCoder->Get("leftSpeed", &message.leftSpeed);
    pCoder->Get("rightSpeed", &message.rightSpeed);
    pCoder->Get("timestamp", &message.timestamp);

    pCoder->End();
    return message;
}
