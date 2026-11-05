
#ifndef PLAN_H
#define PLAN_H

#include <Arduino.h>

struct Plan {
    uint8_t leds;  // DR, D, DL, L, UL, U, UR, R
    bool C;
    bool VFD;
};

inline Plan operator|(const Plan& a, const Plan& b)
{
    Plan result;

    result.leds = a.leds | b.leds;
    result.C = a.C || b.C;
    result.VFD = a.VFD || b.VFD;

    return result;
}

inline Plan operator&(const Plan& a, const Plan& b)
{
    Plan result;

    result.leds = a.leds & b.leds;
    result.C = a.C && b.C;
    result.VFD = a.VFD && b.VFD;

    return result;
}

inline Plan operator-(const Plan& a, const Plan& b)
{
    Plan result;

    result.leds = a.leds & ~b.leds;
    result.C = a.C && !b.C;
    result.VFD = a.VFD && !b.VFD;

    return result;
}

// DR to R bit positions
#define DR  (1U << 0)
#define D   (1U << 1)
#define DL  (1U << 2)
#define L   (1U << 3)
#define UL  (1U << 4)
#define U   (1U << 5)
#define UR  (1U << 6)
#define R   (1U << 7)
#define ALL   0xFF
#define NONE   0x00

#endif
