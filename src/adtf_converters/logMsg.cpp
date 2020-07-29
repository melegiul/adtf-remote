#include <adtf_plugin_sdk.h>

#include "../adtfmediasample.h"
#include "helper_structs.h"
#include "logMsg.h"

using namespace adtf;

static const std::string mediaType = "tLogMsg";
static const char *mediaDescription = R"(
<?xml version="1.0" encoding="iso-8859-1" standalone="no"?>
<adtf:ddl xmlns:adtf="adtf">
    <enums>
        <enum name="tFilterLogType" type="tUInt32">
            <element name="NONE" value="0" />
            <element name="DEBUG" value="100" />
            <element name="INFO" value="110" />
            <element name="WARNING" value="120" />
            <element name="ERROR" value="130" />
            <element name="CRITICAL" value="140" />
            <element name="ACK" value="150" />
        </enum>
        <enum name="tUniaFilter" type="tUInt32">
            <element name="STATEMACHINE" value="100"/>
            <element name="TRAJECTORYPLANNER" value="110" />
            <element name="LANEDETECTIONFUSION" value="120" />
        </enum>
        <enum name="tLogContext" type="tUInt32">
            <element name="NONE" value="0" />
            <element name="MAP" value="100"/>
            <element name="AC" value="110"/>
            <element name="RI" value="120"/>
            <element name="INITIALIZATION" value="200" />
            <element name="READY" value="210" />
            <element name="AD_RUNNING" value="220" />
            <element name="RC_RUNNING" value="230" />
            <element name="EMERGENCY" value="240" />
        </enum>
    </enums>
    <structs>
        <struct alignment="1" name="tLogMsg" version="1">
           <element alignment="1" arraysize="1" byteorder="LE" bytepos="0" name="timestamp" type="tUInt64" />
           <element alignment="1" arraysize="1" byteorder="LE" bytepos="8" name="logtype" type="tFilterLogType" />
           <element alignment="1" arraysize="1" byteorder="LE" bytepos="12" name="filter" type="tUniaFilter" />
           <element alignment="1" arraysize="1" byteorder="LE" bytepos="16" name="logcontext" type="tLogContext" />
           <element alignment="1" arraysize="1" byteorder="LE" bytepos="20" name="payloadLength" type="tUInt32" />
           <element alignment="1" arraysize="payloadLength" byteorder="LE" bytepos="24" name="ui8Data" type="tUInt8" />
        </struct>
    </structs>
</adtf:ddl>
)";
static cObjectPtr<cMediaCoder> pCoder = new cMediaCoder();
static bool pCoderInitialized = false;

std::unique_ptr<tLogMsg> adtf_converter::from_network::logMsg(const ADTFMediaSample &sample) {
    if (!pCoderInitialized) {
        pCoder->Create(mediaType.data(), mediaDescription, IMediaDescription::MDF_DDL_DEFAULT_VERSION);
        pCoderInitialized = true;
    }

    //TODO adapt to helper_structs.h
    assert(sample.mediaType == mediaType);
    uint64_t timestamp;
    tFilterLogType logtype;
    tUniaFilter filter;
    tLogContext logcontext;
    uint32_t payloadLength;
    pCoder->Begin(sample.data.get(), sample.length);

    pCoder->Get("timestamp", &timestamp);
    pCoder->Get("logtype", &logtype);
    pCoder->Get("filter", &filter);
    pCoder->Get("logcontext", &logcontext);
    pCoder->Get("payloadLength", &payloadLength);
    std::unique_ptr<tLogMsg> logMsg = std::make_unique<tLogMsg>(timestamp, logtype, filter, logcontext, payloadLength);
    pCoder->Get("ui8Data", logMsg->ui8Data);

    pCoder->End();
    return logMsg;
}