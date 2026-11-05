#include "Low.h"
#include "Mid.h"

#include "Plan.h"
#include "Anim.h"
#include "Sequence.h"


// ==========================================
// SETUP
// ==========================================

void setup() {

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  pinMode(DATA_OSC_PIN, INPUT);
  pinMode(CLOCK_OSC_PIN, INPUT);

  digitalWrite(DATA_PIN, LOW);
  digitalWrite(CLOCK_PIN, LOW);

  Serial.begin(115200);

  lastSample = micros();
}




// ==========================================
// SEQUENCE METHODS
// ==========================================
template <size_t N>
void play(const Sequence<N>& seq)
{
    size_t i = 0;
    while (true)
    {
        for (uint8_t j = 0; j < seq.reps[i]; j++)
        {
            seq.anims[i].play(seq.anims[i].data);
        }
        i = (i + 1) % N;
    }
}







// ==========================================
// PLANS & ANIMS DEFINITIONS
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

// ==========================================
// ANIMS DEFINITIONS
// ==========================================

// base anims
const Anim<1> sec_off = { { OFF }, { 2500 } };
const Anim<1> sec_on = { { ON }, { 2500 } };
const Anim<1> circly = { { CIRCLE }, { 100 } };
const Anim<1> losanj = { { LOSANJ }, { 100 } };
const Anim<1> squary = { { SQUARE }, { 100 } };
const Anim<4> vfd_blink = {
  { VFD, OFF, VFD, OFF },
  { 500*2.5 , 100*2.5 , 50*2.5 , 3000 }
};
const Anim<5> full_blink = {
  { VFD, ON, OFF, ON, TRI_U },
  { 100 , 50, 150, 400 , 200 }
};

// triangle / loops anims
const Anim<8> triangulate = {
  { TRI_UL, TRI_U, TRI_UR, TRI_R, TRI_DR, TRI_D, TRI_DL, TRI_L },
  { 300 , 75, 300, 75 , 300 , 75 ,300 , 75 }
};
const Anim<8> loop_inverse = {
  { LED_R , LED_UR , LED_U , LED_UL , LED_L , LED_DL , LED_D , LED_DR },
  { 150 , 150 , 150 , 150 , 150 , 150 , 150 , 150 },
};
const Anim<8> loop_clockwise = {
  { LED_R , LED_DR , LED_D , LED_DL , LED_L , LED_UL , LED_U , LED_UR },
  { 150 , 150 , 150 , 150 , 150 , 150 , 150 , 150 },
};

// fast anims (appears less bright)
const Anim<2> on_10 = { { ON, OFF }, { 1,10 } };
const Anim<2> on_5 = { { ON, OFF }, { 1,20 } };

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

// ==========================================
// MAIN LOOP
// ==========================================

void loop()
{
    play(circlified_idle);
}
