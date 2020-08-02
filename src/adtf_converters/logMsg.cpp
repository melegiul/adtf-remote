#include <adtf_plugin_sdk.h>
#include <iostream>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "logMsg.h"
#include "mediaDesciptionSingleton.h"

using namespace adtf;

static const std::string mediaType = "tLogMsg";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::unique_ptr<tLogMsg> adtf_converter::from_network::logMsg(const ADTFMediaSample &sample) {

    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDesciptionSingleton::getInstance().getMediaDescription().c_str(), IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    //TODO adapt to helper_structs.h
    assert(sample.mediaType == mediaType);
    uint64_t timestamp;
    tFilterLogType logtype;
    tUniaFilter filter;
    tLogContext logcontext;
    uint32_t payloadLength;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("timestamp", &timestamp);
    pCoder->Get("logtype", &logtype);
    pCoder->Get("filter", &filter);
    pCoder->Get("logcontext", &logcontext);
    pCoder->Get("payloadLength", &payloadLength);
    std::unique_ptr<tLogMsg> logMsg = std::make_unique<tLogMsg>(timestamp, logtype, filter, logcontext, payloadLength);
    pCoder->Get("ui8Data", logMsg->ui8Data);

    pCoder->End();
    return logMsg;
}