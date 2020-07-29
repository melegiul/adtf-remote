#include <vector>
#include <memory>

#include "helper_structs.h"

struct ADTFMediaSample;

namespace adtf_converter {
    namespace from_network {
        std::vector<DetectedLine_POD> detectedLine(const ADTFMediaSample &sample);
        std::unique_ptr<tDetectedLine> detectedLinePOD(const std::vector<DetectedLine_POD> &lines);
    }
}
