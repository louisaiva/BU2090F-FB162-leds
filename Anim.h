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

    for (size_t i = 0; i < N; i++)
    {
        uint16_t pattern = planToBU2090(anim.plans[i]);

        sendBU2090(pattern);
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


#endif