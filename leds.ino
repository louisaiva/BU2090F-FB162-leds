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
        Serial.println("[SEQ] " + String(i) + " : Playing anim for " + String(seq.reps[i]) + " repetitions");
        for (uint8_t j = 0; j < seq.reps[i]; j++)
        {
            seq.anims[i].play(seq.anims[i].data);
        }
        i = (i + 1) % N;
    }
}

// ==========================================
// MAIN LOOP
// ==========================================

void loop()
{
    play(circlified_idle);
}
