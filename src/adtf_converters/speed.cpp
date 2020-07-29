#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "speed.h"

using namespace adtf;

static const std::string mediaType = "tSpeed";
static const char *mediaDescription = R"(
<?xml version="1.0" encoding="iso-8859-1" standalone="no"?>
<adtf:ddl xmlns:adtf="adtf">
  <structs>
    <struct alignment="1" name="tSpeed" version="1">
      <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="combinedSpeed" type="tFloat32" />
      <element alignment="1" arraysize="1" byteorder="LE" bytepos="4" name="leftSpeed" type="tFloat32" />
      <element alignment="1" arraysize="1" byteorder="LE" bytepos="8" name="rightSpeed" type="tFloat32" />
      <element alignment="1" arraysize="1" byteorder="LE" bytepos="12" name="timestamp" type="tUInt32" />
    </struct>
  </structs>
</adtf:ddl>
)";

static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tSpeed adtf_converter::from_network::speed(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tSpeed message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("combinedSpeed", &message.combinedSpeed);
    pCoder->Get("leftSpeed", &message.leftSpeed);
    pCoder->Get("rightSpeed", &message.rightSpeed);
    pCoder->Get("timestamp", &message.timestamp);

    pCoder->End();
    return message;
}
