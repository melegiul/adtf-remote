#include <memory>

struct ADTFMediaSample;


namespace adtf_converter {
    namespace from_network {
        std::unique_ptr<tLogMsg> logMsg(const ADTFMediaSample &sample);
    }
}


