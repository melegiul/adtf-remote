#include <vector>
#include <memory>

#include "user_structs.h"

struct ADTFMediaSample;

std::vector<DetectedLine_POD> fromNetworkDetectedLine(const ADTFMediaSample &sample);
std::unique_ptr<tDetectedLine> convertToOldFormat(const std::vector<DetectedLine_POD> &lines);
