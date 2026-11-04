
#ifndef PLAN_H
#define PLAN_H

#include <Arduino.h>

struct Plan {
    uint8_t leds;  // DR, D, DL, L, UL, U, UR, R
    bool C;
    bool VFD;
};

// DR to R bit positions
#define PLAN_DR  (1U << 0)
#define PLAN_D   (1U << 1)
#define PLAN_DL  (1U << 2)
#define PLAN_L   (1U << 3)
#define PLAN_UL  (1U << 4)
#define PLAN_U   (1U << 5)
#define PLAN_UR  (1U << 6)
#define PLAN_R   (1U << 7)

#endif
