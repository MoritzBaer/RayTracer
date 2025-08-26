#pragma once

#include <inttypes.h>
#include "IntegerPacking.h"
#include "InterThreadQueue.h"
#include <random>

struct TraceTask;
template <typename ResultType>
struct TraceResult;

class RGBTracer
{
public:
    struct RGB
    {
        double R, G, B;

        inline RGB &operator+=(RGB const &other)
        {
            R += other.R;
            G += other.G;
            B += other.B;
            return *this;
        }
    };

    typedef RGB ResultType;

private:
    InterThreadQueue<TraceResult<RGB>> &resultEndpoint;
    bool &runningFlag;
    std::random_device rng;

public:
    inline constexpr static uint32_t ConvertToPackedInt(RGB const &rgb)
    {
        return A_VALUE(0xff) | R_VALUE(uint32_t(rgb.R * 255)) | G_VALUE(uint32_t(rgb.G * 255)) | B_VALUE(uint32_t(rgb.B * 255));
    }

    void Work(TraceTask const &);

    RGBTracer(InterThreadQueue<TraceResult<RGB>> &resultEndpoint, bool &runningFlag) : resultEndpoint(resultEndpoint), rng(), runningFlag(runningFlag) {}
};
