#ifndef ANIM_H
#define ANIM_H

#include <Arduino.h>
#include "Plan.h"
#include "Low.h"

template <size_t N>
struct Anim {
    Plan plans[N];
    unsigned long times[N]; // nb of tick per plan. tick default is 1 ms
};



template <size_t N, size_t M>
Anim<N + M> operator|(const Anim<N>& a, const Anim<M>& b)
{
    Anim<N + M> result;

    for (size_t i = 0; i < N; i++)
    {
        result.plans[i] = a.plans[i];
        result.times[i] = a.times[i];
    }

    for (size_t i = 0; i < M; i++)
    {
        result.plans[N + i] = b.plans[i];
        result.times[N + i] = b.times[i];
    }

    return result;
}


// ==========================================
// ANIMS DEFINITIONS
// ==========================================

// base anims
const Anim<1> sec_off = { { OFF }, { 2500 } };
const Anim<1> sec_on = { { ON }, { 2500 } };
const Anim<1> on = { { ON }, { 100 } };
const Anim<1> off = { { OFF }, { 100 } };
const Anim<1> circly = { { CIRCLE }, { 100 } };
const Anim<1> losanj = { { LOSANJ }, { 100 } };
const Anim<1> squary = { { SQUARE }, { 100 } };
const Anim<4> vfd_blink = { { VFD, OFF, VFD, OFF }, { 500*2.5 , 100*2.5 , 50*2.5 , 3000 } };
const Anim<4> DR_blink = { { LED_DR, OFF, LED_DR, OFF }, { 300 , 100 , 50 , 100 } };
const Anim<5> full_blink = { { VFD, ON, OFF, ON, TRI_U }, { 100 , 50, 150, 400 , 200 } };
const Anim<6> squary_blink = { { SQUARE, SQUAR8 - LED_UR, OFF, SQUAR8, DIAG_UR, OFF }, { 100 , 50, 150, 250 , 300, 200 } };

// faded anims (appears less bright)
const Anim<2> on_10 = { { ON, OFF }, { 1,10 } };
const Anim<2> on_5 = { { ON, OFF }, { 1,20 } };
const Anim<2> losanj_5 = { { LOSANJ, OFF }, { 1,20 } };

// triangle / loops anims
const Anim<8> triangulate = {
  { TRI_DR, TRI_D, TRI_DL, TRI_L, TRI_UL, TRI_U, TRI_UR, TRI_R },
  { 100 , 100, 100, 100 , 100 , 100 ,100 , 100 }
};
const Anim<8> loop_inverse = {
  { LED_DR, LED_R , LED_UR , LED_U , LED_UL , LED_L , LED_DL , LED_D  },
  { 150 , 150 , 150 , 150 , 150 , 150 , 150 , 150 },
};
const Anim<8> loop_clockwise = {
  { LED_DR , LED_D , LED_DL , LED_L , LED_UL , LED_U , LED_UR , LED_R },
  { 150 , 150 , 150 , 150 , 150 , 150 , 150 , 150 },
};

// complexes anims
const Anim<7> force = {
  { LED_UL, TRI_UL, TRI_UL | CIRCLE , CIRCLE | DIAG_UL | DIAG_DR, CIRCLE | TRI_DR, TRI_DR, LED_DR },
  { 100 , 90, 75, 50 , 75 , 90 , 100 }
};
const Anim<20> force2 = {
  { OFF, LED_UL, OFF, LED_UL, TRI_UL, TRI_UL | CIRCLE , CIRCLE | DIAG_UL | DIAG_DR, CIRCLE | TRI_DR, TRI_DR, LED_DR ,
	OFF | VFD, LED_DR | VFD, OFF | VFD, LED_DR, TRI_DR, TRI_DR | CIRCLE , CIRCLE /* | DIAG_UL | DIAG_DR */, CIRCLE | TRI_UL, TRI_UL, LED_UL ,
  },
  { 800, 500, 100, 100 , 90, 75, 50 , 75 , 90 , 100,
	800, 500, 100, 100 , 90, 75, 50 , 75 , 90 , 100
  }
};
const Anim<7> on_to_UL = {
    {
        NO_VFD,
        NO_VFD - LED_DR,
        NO_VFD - TRI_DR,
        CIRCLE | TRI_UL | LED_DL | LED_UR,
        TRI_UL | CIRCLE,
        TRI_UL,
        LED_UL
    },
    { 250,250,250,250,250,250,250 }
};



// prog indicators
const Anim<1> ind_1 = { { LED_UL }, { 2500 } };
const Anim<1> ind_2 = { { LED_U }, { 2500 } };
const Anim<1> ind_3 = { { LED_UR }, { 2500 } };
const Anim<1> ind_4 = { { LED_L }, { 2500 } };
const Anim<1> ind_5 = { { LED_R }, { 2500 } };
const Anim<1> ind_6 = { { LED_DL }, { 2500 } };
const Anim<1> ind_7 = { { LED_D }, { 2500 } };
const Anim<1> ind_8 = { { LED_DR }, { 2500 } };
const Anim<1> ind_9 = { { VFD }, { 2500 } };
const Anim<1> ind_off = { { CIRCLE }, { 2500 } };
const Anim<1> prog_indicators[] = 
{
    ind_1,
    ind_2,
    ind_3,
    ind_4,
    ind_5,
    ind_6,
    ind_7,
    ind_8,
    ind_9
};




// ==========================================
// ANIM REF (needed to create sequences of different sized anims)
// ==========================================

// Reference to an animation of any size
struct AnimRef {
    const void* data;
    size_t count;
    void (*play)(const void* data);
};

// Function that plays a specific animation size
template <size_t N>
void playAnim(const void* data)
{
    const Anim<N>& anim =
        *static_cast<const Anim<N>*>(data);
    playAnim(anim);
}
template <size_t N>
void playAnim(const Anim<N>& anim)
{
    for (size_t i = 0; i < N; i++)
    {
        uint16_t pattern = planToBU2090(anim.plans[i]);
        sendBU2090(pattern);
        if (LOG_ANIM) { Serial.println("[ANIM] " + String(i) + " : " + String((uint16_t)pattern, HEX) + ".  Now waiting for " + String(anim.times[i]) + " ticks   (" + String((anim.times[i] * tick) / 1000) + " ms)"); }
        waitAndSample(anim.times[i] * tick);
    }
}

// Converts any Anim<N> into an AnimRef
template <size_t N>
AnimRef build(const Anim<N>& anim)
{
    return {
        &anim,
        N,
        playAnim<N>
    };
}

const AnimRef anim_off = build(off);


#endif