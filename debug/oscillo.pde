/*

This script is a processing script to help debugging the BU2090F sent packet.

*/


import processing.serial.*;

Serial port;

// Oscilloscope buffer
final int HISTORY_SIZE = 50000;

int[] valuesClock = new int[HISTORY_SIZE];
int[] valuesData  = new int[HISTORY_SIZE];

int writeIndex = 0;
int sampleCount = 0;


float zoomX = 1.0f;
float zoomY = 1.0f;

int packetState = 0;
int incomingClock = 0;
int incomingData = 0;
int patternLow = 0;

int currentPattern = 0;


void setup() {
  size(1280, 480);

  printArray(Serial.list());

  // Change index if necessary to select your Arduino port
  port = new Serial(this, Serial.list()[1], 115200);

  for (int i = 0; i < HISTORY_SIZE; i++) {
    valuesClock[i] = 0;
    valuesData[i] = 0;
  }

  background(0);
  smooth();
}
void readSerial() {

  while (port.available() > 0) {

    int incoming = port.read();

    switch (packetState) {

      case 0:
        if (incoming == 0xAA) {
          packetState = 1;
        }
        break;

      case 1:
        incomingClock = incoming;
        packetState = 2;
        break;

      case 2:
        incomingData = incoming;
        packetState = 3;
        break;

      case 3:
        patternLow = incoming;
        packetState = 4;
        break;

      case 4:

        currentPattern = patternLow | (incoming << 8);

        int clockVal = incomingClock * 1023;
        int dataVal = incomingData * 1023;

        valuesClock[writeIndex] = clockVal;
        valuesData[writeIndex] = dataVal;

        writeIndex = (writeIndex + 1) % HISTORY_SIZE;

        if (sampleCount < HISTORY_SIZE) {
          sampleCount++;
        }

        packetState = 0;
        break;
    }
  }
}
String binary12(int value) {

  String result = "";

  for (int i = 11; i >= 0; i--) {

    result += ((value >> i) & 1);
  }

  return result;
}

// Convert a chronological sample position into its circular buffer index
int getBufferIndex(int chronologicalIndex) {

  int oldestIndex = (writeIndex - sampleCount + HISTORY_SIZE)
                    % HISTORY_SIZE;

  return (oldestIndex + chronologicalIndex) % HISTORY_SIZE;
}


int getY(int val, float offset) {

  float centeredVal = val - 512;

  centeredVal *= zoomY * 0.198f;

  float y = (height / 2.0)
            - (centeredVal / 1023.0 * (height - 1))
            + offset;

  return (int)y;
}


void drawLines() {

  if (sampleCount < 2) return;

  // Number of historical samples to display
  int displaySamples = (int)(width / zoomX);

  displaySamples = constrain(displaySamples, 2, sampleCount);

  // Start position in the history buffer
  int startSample = sampleCount - displaySamples;

  strokeWeight(2);

  // ---------------- CLOCK ----------------

  stroke(0, 255, 0);

  int previousY = 0;

  for (int x = 0; x < width; x++) {

    // Map screen pixels to historical samples.
    // Several samples may be skipped between pixels.
    int sampleOffset = startSample
      + (int)((long)x * (displaySamples - 1) / (width - 1));

    int bufferIndex = getBufferIndex(sampleOffset);

    int y = getY(valuesClock[bufferIndex], -100);

    if (x > 0) {
      line(x - 1, previousY, x, y);
    }

    previousY = y;
  }


  // ---------------- DATA ----------------

  stroke(0, 150, 255);

  previousY = 0;

  for (int x = 0; x < width; x++) {

    int sampleOffset = startSample
      + (int)((long)x * (displaySamples - 1) / (width - 1));

    int bufferIndex = getBufferIndex(sampleOffset);

    int y = getY(valuesData[bufferIndex], 100);

    if (x > 0) {
      line(x - 1, previousY, x, y);
    }

    previousY = y;
  }
}


void draw() {

  background(0);

  readSerial();

  drawLines();

  
  // Current pattern information
  
  fill(255);
  textSize(16);
  
  text("DECIMAL: " + currentPattern, 250, 25);
  
  text("BINARY: " + binary12(currentPattern), 250, 45);
  
  text("HEX: 0x" + hex(currentPattern, 3), 250, 65);
  
  
  fill(0, 255, 0);
  textSize(14);
  text("CLOCK", 20, 25);

  fill(0, 150, 255);
  text("DATA", 20, 45);

  fill(255);
  textSize(11);

  text("Zoom X: " + nf(zoomX, 1, 3), 20, height - 35);
  text("Zoom Y: " + nf(zoomY, 1, 3), 20, height - 55);
  text("Samples displayed: " + (int)(width / zoomX),
       20, height - 15);
}


void keyReleased() {

  // X axis
  if (key == '+') {
    zoomX *= 2.0f;
  }

  else if (key == '-') {
    zoomX /= 2.0f;

    if (zoomX < 0.001f) {
      zoomX = 0.001f;
    }
  }

  // Y axis
  else if (key == CODED) {

    if (keyCode == UP) {
      zoomY *= 1.5f;
    }

    else if (keyCode == DOWN) {
      zoomY /= 1.5f;
    }
  }
}
