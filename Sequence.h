#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <Arduino.h>
#include "Anim.h"

template <size_t N>
struct Sequence {
    // int id;
    AnimRef anims[N];
    uint8_t reps[N]; // repetitions of the anims
};

template <size_t N, size_t M>
Sequence<N + M> operator|(const Sequence<N>& a, const Sequence<M>& b)
{
    Sequence<N + M> result;

    // result.id = a.id + b.id;

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




// ==========================================
// SEQUENCE DEFINITIONS
// ==========================================

const Sequence<6> idle =
{
    {
        build(force2),
        build(loop_inverse),
        build(vfd_blink),
        build(full_blink),
        build(sec_on),
        build(on_to_UL)
    },
    { 4, 3, 5, 1, 1, 1}
};
const Sequence<4> circlify =
{
    {
        build(squary),
        build(losanj),
        build(circly),
        build(losanj),
    },
    { 4,1,4,1}
};
const Sequence<6> wavy =
{
    {
        build(sec_off),
        build(on_5),
        build(on_10),
        build(sec_on),
        build(on_10),
        build(on_5),
    },
    { 1, 1000, 2000, 1, 2000, 1000 }
};

// chained sequences
const auto circlified_idle = idle | circlify | circlify | circlify | circlify;





#endif