#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "user_structs.h"
#include "carodometry.h"

using namespace adtf;

static const std::string mediaType = "tCarOdometry";
static const char *mediaDescription = R"(
<?xml version="1.0" encoding="iso-8859-1" standalone="no"?>
<adtf:ddl xmlns:adtf="adtf">
    <datatypes>
        <datatype description="Point2f from open cv" name="Point2f" size="64" />
    </datatypes>
    <structs>
        <struct alignment="1" name="tCarOdometry" version="1">
            <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="pos" type="Point2f"/>
            <element alignment="1" arraysize="1" byteorder="LE" bytepos="8" name="orientation" type="tFloat32"/>
            <element alignment="1" arraysize="1" byteorder="LE" bytepos="12" name="timestamp" type="tUInt32"/>
        </struct>
    </structs>
</adtf:ddl>
)";

static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tCarOdometry adtf_converter::from_network::carOdometry(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tCarOdometry message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("pos", &message.pos);
    pCoder->Get("orientation", &message.orientation);
    pCoder->Get("timestamp", &message.timestamp);

    pCoder->End();
    return message;
}
