#ifndef LOW_H
#define LOW_H

#include <Arduino.h>


/*

PINS

*/

// MAIN PINS, CONNECTED TO THE BOARD
const int DATA_PIN = 9;
const int CLOCK_PIN = 2;

// OSCILLO PINS, FOR DEBUG ONLY
const int DATA_OSC_PIN = 10;
const int CLOCK_OSC_PIN = 3;


// SWITCH PINS
const int SWITCH_IN_PIN = A3;
const int SWITCH_OUT_PIN = 13;


/*

SWITCH & PROGRAMS

*/

// Constants
const float V_REF = 5.0;     // Analog reference voltage (e.g., 5V or 3.3V)
const float R_BITS = 10.0;   // ADC resolution (bits)
const float ADC_STEPS = (1 << int(R_BITS)) - 1; // Number of steps (2^R_BITS - 1)


bool LAST_SWITCH = 0;
const int PROG_COUNT = 3;


/*

PACKET TIMING & OSCILLO

*/

// TOGGLE THE OSCILLO
const bool SEND_PROCESSING_PACKETS = false;
// activating the above bool will automatically
// multiply the 4 below values by 100
// to see the oscillo on the processing script
const unsigned long DATA_SETUP_US = 5;
const unsigned long CLOCK_HIGH_US = 20;
const unsigned long DATA_LATCH_US = 5;
const unsigned long CLOCK_LOW_US = 10;

// processing oscillo useful variables
const unsigned long SAMPLE_INTERVAL = 1000;
unsigned long lastSample = 0;
uint16_t currentPattern = 0;

/*

ANIM TIMING

*/


// ANIM TIMINGS
const unsigned long ms = 1000;
const unsigned long tick = 400;


/*

LOGS

*/

const bool LOG_ANIM = false;
const bool LOG_SWITCH = false;
const bool LOG_SEQ = true;
const bool LOG_PROG = true;



// ==========================================
// SEND SIGNALS TO PROCESSING
// ==========================================

void oscilloDigital()
{

  if (!SEND_PROCESSING_PACKETS) { return; }

  byte clockValue = digitalRead(CLOCK_OSC_PIN);
  byte dataValue = digitalRead(DATA_OSC_PIN);


  Serial.write(0xAA);
  Serial.write(clockValue);
  Serial.write(dataValue);

  // Current 12-bit pattern: low byte, then high byte
  Serial.write((byte)(currentPattern & 0xFF));
  Serial.write((byte)(currentPattern >> 8));
}


// ==========================================
// WAIT WHILE SAMPLING
// ==========================================

void waitAndSample(unsigned long durationUs) {

  unsigned long start = micros();

  while ((unsigned long)(micros() - start) < durationUs) {

    unsigned long now = micros();

    if ((unsigned long)(now - lastSample) >= SAMPLE_INTERVAL) {

      lastSample = now;
      oscilloDigital();
    }
  }
}


// ==========================================
// CHECK SWITCH STATUS
// ==========================================
bool refreshSwitch()
{
    bool switch_is_on = true;
    int nb_reps = 30;
    for (int i=0;i<nb_reps;i++)
    {
        int new_switch = analogRead(SWITCH_IN_PIN);

        int rawValue = analogRead(SWITCH_IN_PIN); // Read the analog input
        float voltage = (rawValue / ADC_STEPS) * V_REF; // Convert to voltage
        if (LOG_SWITCH) { Serial.println("[SWITCH] reading switch : " + String(i) + " with voltage : " + String(voltage)); }
        if (voltage < 4.90)
        {
            switch_is_on = false;
            break;
        }
    }
    bool switch_was_toggled = false;

    // if (LOG_SWITCH) { Serial.println("[SWITCH] last switch : " + String(LAST_SWITCH) + "    | voltage : " + String(voltage)); }
    if (switch_is_on != LAST_SWITCH)
    {
        switch_was_toggled = true;
        if (LOG_PROG) { Serial.println("[PROG] >>>>>>> SWITCH DETECTED : " + String(switch_is_on)); }
    }
    LAST_SWITCH = switch_is_on;
    return switch_was_toggled;
}

// ==========================================
// BU2090F TRANSMISSION
// ==========================================

void sendBU2090(uint16_t pattern) {

  currentPattern = pattern & 0x0FFF;
  unsigned long time_mult = 1;
  if (SEND_PROCESSING_PACKETS) { time_mult = 100; }

  digitalWrite(CLOCK_PIN, LOW);

  for (int i = 11; i >= 0; i--) {

    byte bitValue = (currentPattern >> i) & 0x01;

    digitalWrite(DATA_PIN, bitValue);
    waitAndSample(DATA_SETUP_US * time_mult);

    // Rising edge: shift bit
    digitalWrite(CLOCK_PIN, HIGH);
    waitAndSample(CLOCK_HIGH_US * time_mult);

    if (i == 0) {
      digitalWrite(DATA_PIN, HIGH);
    } else {
      digitalWrite(DATA_PIN, LOW);
    }

    waitAndSample(DATA_LATCH_US * time_mult);

    // Falling edge
    digitalWrite(CLOCK_PIN, LOW);

    if (i == 0) {

      waitAndSample(DATA_LATCH_US * time_mult);

      digitalWrite(DATA_PIN, LOW);

      waitAndSample((CLOCK_LOW_US - DATA_LATCH_US) * time_mult);

    } else {

      waitAndSample(CLOCK_LOW_US * time_mult);
    }
  }

  digitalWrite(DATA_PIN, LOW);
  waitAndSample(DATA_SETUP_US * time_mult);
}

#endif