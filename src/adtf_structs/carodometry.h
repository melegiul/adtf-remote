#include <cstdint>
#include <string>

struct ADTFMediaSample;

struct tCarOdometry
{
    float pos0;
    float pos1;
    float orientation;
    uint32_t timestamp;

public:
    static tCarOdometry fromNetwork(const ADTFMediaSample &sample);
    static ADTFMediaSample toNetwork(tCarOdometry message, std::string pinName, uint64_t streamTime);
};
