#include <vector>

#include "user_structs.h"

struct ADTFMediaSample;


std::vector<DetectedLine> fromNetworkDetectedLine(const ADTFMediaSample &sample);
