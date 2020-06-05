#include <cstdint>
#include <string>

struct ADTFMediaSample;

struct tCarOdometry2
{
    float posf[2];
    float orientation;
    uint32_t timestamp;

public:
    static tCarOdometry2 fromNetwork(const ADTFMediaSample &sample);
    static ADTFMediaSample toNetwork(tCarOdometry2 message, std::string pinName, uint64_t streamTime);
};
