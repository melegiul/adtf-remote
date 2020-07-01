#include <memory>

struct ADTFMediaSample;
struct tNearfieldGridMapArray;

std::unique_ptr<tNearfieldGridMapArray> fromNetworkNearfieldGrid(const ADTFMediaSample &sample);
