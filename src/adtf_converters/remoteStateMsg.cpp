#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "user_structs.h"
#include "remoteStateMsg.h"

using namespace adtf;

static const std::string mediaType = "tRemoteStateMsg";
static const char *mediaDescription = R"(
                                      <struct alignment="1" name="tRemoteStateMsg" version="1">
                                        <element alignment="1" arraysize="1" byteorder="LE" bytepos="4" name="state" type="tFilterState" />
                                      </struct>
                                   )";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::unique_ptr<tRemoteStateMsg> adtf_converter::from_network::remoteStateMsg(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    pCoder->Begin(sample.data.get(), sample.length);

    tState state;

    pCoder->Get("state", &state);
    std::unique_ptr<tRemoteStateMsg> rmtStateMsg = std::make_unique<tRemoteStateMsg>(state);

    pCoder->End();
    return rmtStateMsg;
}