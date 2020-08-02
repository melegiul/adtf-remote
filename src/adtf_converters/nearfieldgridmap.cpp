#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "nearfieldgridmap.h"
#include "mediaDesciptionSingleton.h"

using namespace adtf;

static const std::string mediaType = "tNearfieldGridMapArray";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::unique_ptr<tNearfieldGridMapArray> adtf_converter::from_network::nearfieldGridmap(const ADTFMediaSample &sample) {
        if (!pCoderInitialized) {
            pCoder->Create(mediaType.data(), mediaDesciptionSingleton::getInstance().getMediaDescription().c_str(), IMediaDescription::MDF_DDL_DEFAULT_VERSION);
            pCoderInitialized = true;
        }

        assert(sample.mediaType == mediaType);
        pCoder->Begin(sample.data.get(), sample.length);

        std::vector<tUInt8> lines;
        uint16_t arraySize;

        pCoder->Get("length", &arraySize);
        std::unique_ptr<tNearfieldGridMapArray> nearfieldGrid = std::make_unique<tNearfieldGridMapArray>(arraySize);
        pCoder->Get("occupancyList", nearfieldGrid->occupancyList);

        pCoder->End();
        return nearfieldGrid;
}
