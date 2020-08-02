#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "trapezoid.h"
#include "helper_structs.h"
#include "mediaDesciptionSingleton.h"

using namespace adtf;

static const std::string mediaType = "tTrapezoid";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tTrapezoid adtf_converter::from_network::trapezoid(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDesciptionSingleton::getInstance().getMediaDescription().c_str(), IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tTrapezoid message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("posA", &message.pos_A);
    pCoder->Get("posB", &message.pos_B);
    pCoder->Get("posC", &message.pos_C);
    pCoder->Get("posD", &message.pos_D);

    pCoder->End();
    return message;
}
