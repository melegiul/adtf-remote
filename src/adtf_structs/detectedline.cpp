#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "trapezoid.h"
#include "user_structs.h"
#include <QtDebug>

using namespace adtf;

static const std::string mediaType = "tDetectedLine";
static const char *mediaDescription = R"(
                                  <struct alignment="1" name="tDetectedLine" version="1">
                                      <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="arraySize" type="tInt32"/>
                                      <element alignment="1" arraysize="arraySize" byteorder="LE" bytepos="5" name="detectedLine" type="tChar" numbits="256"/>
                                  </struct>
                                   )";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


std::vector<DetectedLine> fromNetworkDetectedLine(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    std::vector<DetectedLine> lines;
    pCoder->Begin(sample.data.get(), sample.length);

    int32_t arraySize;
    pCoder->Get("arraySize", &arraySize);
    lines.resize(arraySize);

    int a = pCoder->Get("detectedLine", lines.data());
    std::memcpy(lines.data(), sample.data.get() + 4, sizeof(DetectedLine) * arraySize);
    qDebug() << "pcoderget:" << a;

    pCoder->End();
    return lines;
}
