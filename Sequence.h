#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <Arduino.h>
#include "Anim.h"

template <size_t N>
struct Sequence {
    AnimRef anims[N];
    uint8_t reps[N]; // repetitions of the anims
};

template <size_t N, size_t M>
Sequence<N + M> operator|(const Sequence<N>& a, const Sequence<M>& b)
{
    Sequence<N + M> result;

    for (size_t i = 0; i < N; i++)
    {
        result.anims[i] = a.anims[i];
        result.reps[i] = a.reps[i];
    }

    for (size_t i = 0; i < M; i++)
    {
        result.anims[N + i] = b.anims[i];
        result.reps[N + i] = b.reps[i];
    }

    return result;
}

#endif