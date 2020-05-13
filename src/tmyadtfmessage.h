#include <cstdint>

typedef int16_t tPixelFormat;

#pragma pack(push,1)
typedef struct
{
    uint32_t ui32HeaderVal;
    double f64HeaderVal;
} tHeaderStruct;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    uint8_t ui8Val;
    uint16_t ui16Val;
    uint32_t ui32Val;
    int32_t i32Val;
    int64_t i64Val;
    double f64Val;
    float f32Val;
} tSimpleStruct;
#pragma pack(pop)

#pragma pack(push,1)
struct tMyADTFMessage
{
    tHeaderStruct sHeaderStruct;
    tSimpleStruct sSimpleStruct;

public:
    static void fromNetwork(void *data, size_t length, uint64_t time);
};
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    tHeaderStruct sHeaderStruct;
    tSimpleStruct sSimpleStruct;
} tNestedStruct;
#pragma pack(pop)
