#include <memory>

struct ADTFMediaSample;


namespace adtf_converter {
    namespace from_network {
        std::unique_ptr<tNearfieldGridMapArray> nearfieldGridmap(const ADTFMediaSample &sample);
    }
}
