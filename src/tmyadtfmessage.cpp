#include <iostream>
#include <cstring>
#include <QtDebug>

#include <adtf_plugin_sdk.h>

#include "tmyadtfmessage.h"

using namespace adtf;

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


void tMyADTFMessage::fromNetwork(void *data, size_t length, uint64_t time) {
    tMyADTFMessage parsed;

    cObjectPtr<cMediaType> mediaType = new cMediaType(0,0,0, "tMyADTFMessage", mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);

    cObjectPtr<IMediaSample> pSample = new cMediaSample();
    int c = pSample->Update(time, data, length, 0);

    cObjectPtr<IMediaCoder> pCoder;
//    cObjectPtr<cMediaCoder> pCoder;
//    pCoder->Create("tMyADTFMessage", mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
//    pCoder->Begin() + End()
    int a = mediaType->Lock(pSample, &pCoder);

    int d = pCoder->Get("sHeaderStruct.ui32HeaderVal", &parsed.sHeaderStruct.ui32HeaderVal);
    pCoder->Get("sHeaderStruct.f64HeaderVal", &parsed.sHeaderStruct.f64HeaderVal);
    int e = pCoder->Get("sSimpleStruct.ui8Val", &parsed.sSimpleStruct.ui8Val);
    pCoder->Get("sSimpleStruct.ui16Val", &parsed.sSimpleStruct.ui16Val);
    pCoder->Get("sSimpleStruct.ui32Val", &parsed.sSimpleStruct.ui32Val);
    pCoder->Get("sSimpleStruct.i32Val", &parsed.sSimpleStruct.i32Val);
    pCoder->Get("sSimpleStruct.i64Val", &parsed.sSimpleStruct.i64Val);
    pCoder->Get("sSimpleStruct.f64Val", &parsed.sSimpleStruct.f64Val);
    pCoder->Get("sSimpleStruct.f32Val", &parsed.sSimpleStruct.f32Val);

    mediaType->Unlock(pCoder);
}
