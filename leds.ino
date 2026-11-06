#include "Low.h"
#include "Mid.h"

#include "Plan.h"
#include "Anim.h"
#include "Sequence.h"


// ==========================================
// SETUP
// ==========================================

void setup()
{

    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_OSC_PIN, INPUT);
    pinMode(CLOCK_OSC_PIN, INPUT);
    pinMode(SWITCH_OUT_PIN, OUTPUT);
    // pinMode(SWITCH_IN_PIN, INPUT);

    // write first things
    digitalWrite(SWITCH_OUT_PIN, HIGH);
    digitalWrite(DATA_PIN, LOW);
    digitalWrite(CLOCK_PIN, LOW);

    // initialize few things
    Serial.begin(115200);
    lastSample = micros();
    refreshSwitch(); // set the LAST_SWITCH to the current switch status
}


// ==========================================
// MAIN LOOP
// ==========================================

// const auto seq_0 = circlified_idle;
// const auto seq_1 = idle;
// const auto seq_2 = wavy;

const SequenceRef programs[] = 
{
    build(circlified_idle),
    build(idle),
    build(wavy)
};
int current_seq = 0;

void loop()
{
    current_seq = current_seq % PROG_COUNT;
    SequenceRef seq = programs[current_seq];

    int seq_result = seq.play();
    if (seq_result == 1)
    {
        current_seq = (current_seq + 1) % PROG_COUNT;
        // will automatically play next seq next loop
        if (LOG_PROG) { Serial.println("[PROG] >>>>>>> PLAYING NEXT SEQUENCE : " + String(current_seq)); }
    }
}
