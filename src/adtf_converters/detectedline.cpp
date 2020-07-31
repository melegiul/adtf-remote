#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "detectedline.h"

using namespace adtf;

static const std::string mediaType = "tDetectedLineArray";
static const char *mediaDescription = R"(
                                    <datatypes>
                                      <datatype description="Point2i from open cv" name="Point" size="64" />
                                    </datatypes>
                                      <structs>
                                            <struct alignment="1" name="tDetectedLine" version="1">
                                                <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="startPos" type="Point" />
                                                <element alignment="1" arraysize="1" byteorder="LE" bytepos="8" name="endPos" type="Point" />
                                            </struct>
                                                <struct alignment="1" name="tDetectedLineArray" version="1">
                                                <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="arraySize" type="tInt32" />
                                                <element alignment="1" arraysize="arraySize" byteorder="LE" bytepos="4" name="detectedLineArray" type="tDetectedLine" />
                                            </struct>
                                      </structs>
                                   )";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::vector<tDetectedLine> adtf_converter::from_network::detectedLineArray(const ADTFMediaSample &sample) {
        if (!pCoderInitialized) {
            pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
            pCoderInitialized = true;
        }

        assert(sample.mediaType == mediaType);
        pCoder->Begin(sample.data.get(), sample.length);

        std::vector<tDetectedLine> lines;
        int32_t arraySize;

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
