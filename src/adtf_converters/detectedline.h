#include <vector>
#include <memory>

#include "helper_structs.h"

struct ADTFMediaSample;

namespace adtf_converter {
    namespace from_network {
        std::vector<tDetectedLine> detectedLineArray(const ADTFMediaSample &sample);
        std::unique_ptr<tDetectedLineArray> detectedLine(const std::vector<tDetectedLine> &lines);
    }
}
