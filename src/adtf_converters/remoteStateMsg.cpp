#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "remoteStateMsg.h"
#include "mediaDesciptionSingleton.h"

using namespace adtf;

static const std::string mediaType = "tRemoteStateMsg";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::unique_ptr<tRemoteStateMsg> adtf_converter::from_network::remoteStateMsg(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDesciptionSingleton::getInstance().getMediaDescription().c_str(), IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    //TODO adapt to helper_structs.h
    assert(sample.mediaType == mediaType);
    pCoder->Begin(sample.data.get(), sample.length);

    tState state;

    pCoder->Get("state", &state);
    std::unique_ptr<tRemoteStateMsg> rmtStateMsg = std::make_unique<tRemoteStateMsg>(state);

    pCoder->End();
    return rmtStateMsg;
}
