#include "Plan.h"

// MAIN PINS, CONNECTED TO THE BOARD
const int DATA_PIN = 9;
const int CLOCK_PIN = 2;


// OSCILLO PINS, FOR DEBUG ONLY
const int DATA_OSC_PIN = 10;
const int CLOCK_OSC_PIN = 3;

const bool SEND_PROCESSING_PACKETS = false;
const unsigned long SAMPLE_INTERVAL = 1000;

// RAISE THESE VALUES *100 to see the oscillo on the processing script
const unsigned long DATA_SETUP_US = 5 /* * 100 */;
const unsigned long CLOCK_HIGH_US = 20 /* * 100 */;
const unsigned long DATA_LATCH_US = 5 /* * 100 */;
const unsigned long CLOCK_LOW_US = 10 /* * 100 */;


unsigned long lastSample = 0;
uint16_t currentPattern = 0;


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

  digitalWrite(CLOCK_PIN, LOW);

  for (int i = 11; i >= 0; i--) {

    byte bitValue = (currentPattern >> i) & 0x01;

    digitalWrite(DATA_PIN, bitValue);
    waitAndSample(DATA_SETUP_US);

    // Rising edge: shift bit
    digitalWrite(CLOCK_PIN, HIGH);
    waitAndSample(CLOCK_HIGH_US);

    if (i == 0) {
      digitalWrite(DATA_PIN, HIGH);
    } else {
      digitalWrite(DATA_PIN, LOW);
    }

    waitAndSample(DATA_LATCH_US);

    // Falling edge
    digitalWrite(CLOCK_PIN, LOW);

    if (i == 0) {

      waitAndSample(DATA_LATCH_US);

      digitalWrite(DATA_PIN, LOW);

      waitAndSample(CLOCK_LOW_US - DATA_LATCH_US);

    } else {

      waitAndSample(CLOCK_LOW_US);
    }
  }

  digitalWrite(DATA_PIN, LOW);
  waitAndSample(DATA_SETUP_US);
}



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

