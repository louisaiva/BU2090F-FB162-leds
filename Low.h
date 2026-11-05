#ifndef LOW_H
#define LOW_H

#include <Arduino.h>

// MAIN PINS, CONNECTED TO THE BOARD
const int DATA_PIN = 9;
const int CLOCK_PIN = 2;


// OSCILLO PINS, FOR DEBUG ONLY
const int DATA_OSC_PIN = 10;
const int CLOCK_OSC_PIN = 3;

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

// ANIM TIMINGS
const unsigned long ms = 1000;
const unsigned long tick = 400;



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