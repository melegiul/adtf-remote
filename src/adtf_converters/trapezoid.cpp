#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "trapezoid.h"
#include "helper_structs.h"

using namespace adtf;

static const std::string mediaType = "tTrapezoid";
static const char *mediaDescription = R"(
<?xml version="1.0" encoding="iso-8859-1" standalone="no"?>
<adtf:ddl xmlns:adtf="adtf">
    <datatypes>
        <datatype description="Point2f from open cv" name="Point2f" size="64" />
    </datatypes>
    <structs>
        <struct alignment="1" name="tTrapezoid" version="1">
            <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="posA" type="Point2f"/>
            <element alignment="1" arraysize="1" byteorder="LE" bytepos="8" name="posB" type="Point2f"/>
            <element alignment="1" arraysize="1" byteorder="LE" bytepos="16" name="posC" type="Point2f"/>
            <element alignment="1" arraysize="1" byteorder="LE" bytepos="24" name="posD" type="Point2f"/>
        </struct>
    </structs>
</adtf:ddl>
)";

static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tTrapezoid adtf_converter::from_network::trapezoid(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tTrapezoid message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("posA", &message.pos_A);
    pCoder->Get("posB", &message.pos_B);
    pCoder->Get("posC", &message.pos_C);
    pCoder->Get("posD", &message.pos_D);

    pCoder->End();
    return message;
}
