#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "nearfieldgridmap.h"

using namespace adtf;

static const std::string mediaType = "tNearfieldGridMapArray";
static const char *mediaDescription = R"(
                                      <struct alignment="1" name="tNearfieldGridMapArray" version="1">
                                          <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="length" type="tUInt16" />
                                          <element alignment="1" arraysize="length" byteorder="LE" bytepos="2" name="occupancyList" type="tUInt8" />
                                      </struct>
                                   )";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::unique_ptr<tNearfieldGridMapArray> adtf_converter::from_network::nearfieldGridmap(const ADTFMediaSample &sample) {
        if (!pCoderInitialized) {
            pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
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
