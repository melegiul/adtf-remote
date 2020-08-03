#include <adtf_plugin_sdk.h>
#include <iostream>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "detectedline.h"
#include "mediaDesciptionSingleton.h"

using namespace adtf;

static const std::string mediaType = "tDetectedLineArray";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::vector<tDetectedLine> adtf_converter::from_network::detectedLineArray(const ADTFMediaSample &sample) {
        if (!pCoderInitialized) {
            pCoder->Create(mediaType.data(), mediaDesciptionSingleton::getInstance().getMediaDescription().c_str(), IMediaDescription::MDF_DDL_DEFAULT_VERSION);
            pCoderInitialized = true;
        }
        assert(sample.mediaType == mediaType);
        pCoder->Begin(sample.data.get(), sample.length);
        std::vector<tDetectedLine> lines;
        tInt32 arraySize;
        pCoder->Get("arraySize", &arraySize);
        lines.resize(arraySize);
        if (arraySize > 0)
            pCoder->Get("detectedLines", lines.data());
        pCoder->End();
        return lines;
}

std::unique_ptr<tDetectedLineArray> adtf_converter::from_network::detectedLine(const std::vector<tDetectedLine> &lines) {
    std::unique_ptr<tDetectedLineArray> tDetLineArray = std::make_unique<tDetectedLineArray>(lines.size());
    size_t i = 0;
    for (const tDetectedLine &line : lines) {
        tDetLineArray->detectedLines[i].fromPos.x = line.fromPos.x;
        tDetLineArray->detectedLines[i].fromPos.y = line.fromPos.y;
        tDetLineArray->detectedLines[i].toPos.x = line.toPos.x;
        tDetLineArray->detectedLines[i].toPos.y = line.toPos.y;
        ++i;
    }

    return tDetLineArray;
}
