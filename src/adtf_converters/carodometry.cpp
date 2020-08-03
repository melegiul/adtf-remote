#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "carodometry.h"
#include "mediaDesciptionSingleton.h"

using namespace adtf;

static const std::string mediaType = "tCarOdometry";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tCarOdometry adtf_converter::from_network::carOdometry(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDesciptionSingleton::getInstance().getMediaDescription().c_str(), IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tCarOdometry message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("pos", &message.pos);
    pCoder->Get("orientation", &message.orientation);
    pCoder->Get("timestamp", &message.timestamp);

    pCoder->End();
    return message;
}
