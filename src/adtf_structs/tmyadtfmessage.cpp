#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "tmyadtfmessage.h"

using namespace adtf;

static const std::string mediaType = "tMyADTFMessage";
static const char *mediaDescription = R"(
                                <structs>
                                   <struct name="tSimpleStruct" version="1" alignment="1">
                                      <element name="ui8Val" type="tUInt8" bytepos="0" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="ui16Val" type="tUInt16" bytepos="1" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="ui32Val" type="tUInt32" bytepos="3" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="i32Val" type="tInt32" bytepos="7" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="i64Val" type="tInt64" bytepos="11" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="f64Val" type="tFloat64" bytepos="19" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="f32Val" type="tFloat32" bytepos="27" alignment="1" arraysize="1" byteorder="LE"/>
                                   </struct>
                                   <struct name="tHeaderStruct" version="1">
                                      <element name="ui32HeaderVal" type="tUInt32" bytepos="0" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="f64HeaderVal" type="tFloat64" bytepos="4" alignment="1" arraysize="1" byteorder="LE"/>
                                   </struct>
                                   <struct name="tMyADTFMessage" version="1" alignment="1">
                                      <element name="sHeaderStruct" type="tHeaderStruct" bytepos="0" alignment="1" arraysize="1" byteorder="LE"/>
                                      <element name="sSimpleStruct" type="tSimpleStruct" bytepos="12" alignment="1" arraysize="1" byteorder="LE"/>
                                   </struct>
                                </structs>
                                   )";
static const uint length = 43;
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;


tMyADTFMessage adtf_converter::from_network::myADTFMessage(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    assert(sample.mediaType == mediaType);
    tMyADTFMessage message;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("sHeaderStruct.ui32HeaderVal", &message.sHeaderStruct.ui32HeaderVal);
    pCoder->Get("sHeaderStruct.f64HeaderVal", &message.sHeaderStruct.f64HeaderVal);
    pCoder->Get("sSimpleStruct.ui8Val", &message.sSimpleStruct.ui8Val);
    pCoder->Get("sSimpleStruct.ui16Val", &message.sSimpleStruct.ui16Val);
    pCoder->Get("sSimpleStruct.ui32Val", &message.sSimpleStruct.ui32Val);
    pCoder->Get("sSimpleStruct.i32Val", &message.sSimpleStruct.i32Val);
    pCoder->Get("sSimpleStruct.i64Val", &message.sSimpleStruct.i64Val);
    pCoder->Get("sSimpleStruct.f64Val", &message.sSimpleStruct.f64Val);
    pCoder->Get("sSimpleStruct.f32Val", &message.sSimpleStruct.f32Val);

    pCoder->End();
    return message;
}

ADTFMediaSample adtf_converter::to_network::myADTFMessage(tMyADTFMessage message, std::string pinName, uint64_t streamTime) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    ADTFMediaSample sample;
    sample.mediaType = mediaType;
    sample.length = length;
    sample.pinName = pinName;
    sample.streamTime = streamTime;
    sample.data = std::shared_ptr<char[]>(new char[length]);
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Set("sHeaderStruct.ui32HeaderVal", &message.sHeaderStruct.ui32HeaderVal);
    pCoder->Set("sHeaderStruct.f64HeaderVal", &message.sHeaderStruct.f64HeaderVal);
    pCoder->Set("sSimpleStruct.ui8Val", &message.sSimpleStruct.ui8Val);
    pCoder->Set("sSimpleStruct.ui16Val", &message.sSimpleStruct.ui16Val);
    pCoder->Set("sSimpleStruct.ui32Val", &message.sSimpleStruct.ui32Val);
    pCoder->Set("sSimpleStruct.i32Val", &message.sSimpleStruct.i32Val);
    pCoder->Set("sSimpleStruct.i64Val", &message.sSimpleStruct.i64Val);
    pCoder->Set("sSimpleStruct.f64Val", &message.sSimpleStruct.f64Val);
    pCoder->Set("sSimpleStruct.f32Val", &message.sSimpleStruct.f32Val);

    pCoder->End();
    return sample;
}
