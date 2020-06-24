#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "user_structs.h"

using namespace adtf;

static const std::string mediaType = "tDetectedLine_POD";
static const char *mediaDescription = R"(
                                      <structs>
                                          <struct alignment="1" name="Node_POD" version="1">
                                              <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="x" type="tInt32"/>
                                              <element alignment="1" arraysize="1" byteorder="LE" bytepos="4" name="y" type="tInt32"/>
                                          </struct>
                                          <struct alignment="1" name="DetectedLine_POD" version="1">
                                              <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="from" type="Node_POD"/>
                                              <element alignment="1" arraysize="1" byteorder="LE" bytepos="8" name="to" type="Node_POD"/>
                                          </struct>
                                          <struct alignment="1" name="tDetectedLine_POD" version="1">
                                              <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="arraySize" type="tInt32"/>
                                              <element alignment="1" arraysize="arraySize" byteorder="LE" bytepos="4" name="detectedLine" type="DetectedLine_POD"/>
                                          </struct>
                                      </structs>
                                   )";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::vector<DetectedLine_POD> fromNetworkDetectedLine(const ADTFMediaSample &sample) {
        if (!pCoderInitialized) {
            pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
            pCoderInitialized = true;
        }

        assert(sample.mediaType == mediaType);
        pCoder->Begin(sample.data.get(), sample.length);

        std::vector<DetectedLine_POD> lines;
        int32_t arraySize;

        pCoder->Get("arraySize", &arraySize);
        lines.resize(arraySize);
        if (arraySize > 0)
            pCoder->Get("detectedLine", lines.data());

        pCoder->End();
        return lines;
}

std::unique_ptr<tDetectedLine> convertToOldFormat(const std::vector<DetectedLine_POD> &lines) {
    std::unique_ptr<tDetectedLine> tDetLine = std::make_unique<tDetectedLine>(lines.size());
    size_t i = 0;
    for (const DetectedLine_POD &line : lines) {
        tDetLine->detectedLine[i].fromPos.x = line.fromPos.x;
        tDetLine->detectedLine[i].fromPos.y = line.fromPos.y;
        tDetLine->detectedLine[i].toPos.x = line.toPos.x;
        tDetLine->detectedLine[i].toPos.y = line.toPos.y;
        ++i;
    }

    return tDetLine;
}
