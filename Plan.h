
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



// ==========================================
// PLANS DEFINITIONS
// ==========================================


// BASICS
const Plan ON = { ALL, true, true };
const Plan OFF = { 0, false, false };
const Plan CIRCLE = { NONE, true, false };
const Plan VFD = { NONE, false, true };

// 1 LED
const Plan LED_R = { R, false, false };
const Plan LED_UR = { UR, false, false };
const Plan LED_U = { U, false, false };
const Plan LED_UL = { UL, false, false };
const Plan LED_L = { L, false, false };
const Plan LED_DL = { DL, false, false };
const Plan LED_D = { D, false, false };
const Plan LED_DR = { DR, false, false };

// LINES
const Plan LINE_LR = { L | R, false, false };
const Plan LINE_DU = { D | U, false, false };
const Plan LINE_U = { UL | U | UR, false, false };
const Plan LINE_D = { DL | D | DR, false, false };

// DIAGONALES
const Plan DIAG_UL = { U | L, false, false };
const Plan DIAG_UR = { U | R, false, false };
const Plan DIAG_DR = { D | R, false, false };
const Plan DIAG_DL = { D | L, false, false };

// TRIANGLES
const Plan TRI_UL = DIAG_UL | LED_UL;
const Plan TRI_UR = DIAG_UR | LED_UR;
const Plan TRI_DR = DIAG_DR | LED_DR;
const Plan TRI_DL = DIAG_DL | LED_DL;
const Plan TRI_U = LINE_LR | LED_U;
const Plan TRI_D = LINE_LR | LED_D;
const Plan TRI_R = LINE_DU | LED_R;
const Plan TRI_L = LINE_DU | LED_L;

// SQUARES
const Plan LOSANJ = LED_D | LED_L | LED_U | LED_R;
const Plan SQUARE = LED_DR | LED_DL | LED_UL | LED_UR;
const Plan SQUAR8 = SQUARE | LOSANJ;
const Plan NO_VFD = ON - VFD;



#endif
