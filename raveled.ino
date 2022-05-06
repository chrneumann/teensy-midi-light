#include "FastLED.h"
#include <avr/power.h>

#define PIN            11
#define NUMPIXELS      300
#define CONTROL_CHANNEL   13
#define BRIGTHNESS_CONTROL   19
#define CONTRAST_CONTROL   15
#define STEPS_CONTROL   11
#define FPS 50

CRGB leds[NUMPIXELS];
int delayval = 5;

int vu_mono = 100;
int direction = 1;
int position = 0;
int pulse = 0;
int bpm = 60;
float brightness = 0.2;
float contrast = 1;
float steps = 0;
unsigned int color = 1000;

unsigned long lastBeat = 0;
unsigned long lastStep = 0;
unsigned long lastUpdate = 0;
unsigned long lastDePulse = 0;

void setup()
{
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUMPIXELS);
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleControlChange(myControlChange);
}

void myControlChange(byte channel, byte control, byte value) {
  if (channel == CONTROL_CHANNEL) {
    if (control == CONTRAST_CONTROL) {
      contrast = (float) value / 127;
    }
    if (control == BRIGTHNESS_CONTROL) {
      brightness = (float) value / 127;
    }
    if (control == STEPS_CONTROL) {
      steps = (float) value / 127;
    }
  }
}

void myNoteOn(byte channel, byte note, byte velocity) {
  if (note == 68) {
    vu_mono = velocity;
    return;
  }
  if (note == 50) {
    // Beat
    doPulse();
    Serial.print(millis() - lastBeat);
    lastBeat = millis();
    lastStep = lastBeat;
    Serial.println(" beat");
    return;
  }
  if (note == 52) {
    // BPM
    usbMIDI.sendNoteOff(note, velocity + 50, channel);
    bpm = velocity + 50;
    return;
  }
}


void doPulse() {
  direction *= -1;
  pulse = 255 * contrast;
}

void handleEffects() {
  int j = 0;
  int jmod = 1;
  for (int i = 0; i < NUMPIXELS; i++) {
    leds[(i + position) % NUMPIXELS] = CHSV(color % 255, 255, min(255, j * brightness + pulse));
    if (j >= 255) {
      jmod = -1;
    }
    if (j <= 0) {
      jmod = 1;
    }
    j = j + jmod * 10;
    j = max(j, 0);
    j = min(j, 255);
  }
  position += direction;
}

int frameCount = 0;
unsigned long lastFrameMeasure = 0;

void loop()
{
  unsigned long thisStep = millis();
  int factor = lroundf(pow(2, (int) (steps * 100 / 25)));
  const unsigned long stepLength = 60000/((bpm)*factor);
  if(lastStep + stepLength  < thisStep) {
    doPulse();
    Serial.print(millis());
    Serial.print(" stepLength: ");
    Serial.print(stepLength);
    Serial.print(" ");
    Serial.println(thisStep-lastStep);
    lastStep = thisStep;
  }
  if(lastDePulse + (1000/100) < thisStep) {
    lastDePulse = thisStep;
    pulse -= 50;
    pulse = max(pulse, 0);
  }
  if(lastUpdate + (1000/FPS) < thisStep) {
    lastUpdate = thisStep;
    handleEffects();
    FastLED.show();
    frameCount++;
    if (lastFrameMeasure + 1000 < thisStep) {
      lastFrameMeasure = thisStep;
      Serial.print(frameCount);
      Serial.print(" fps | ");
      // Serial.print(steps * 100 / 25);
      // Serial.print(" steps * 100 / 25 | ");
      // Serial.print((int) steps * 100 / 25);
      // Serial.print("(int) steps * 100 / 25 | ");
      // Serial.print((int) pow(2, (int) steps * 100 / 25));
      // Serial.print("pow 2 (int) steps * 100 / 25 | ");
      // Serial.print(steps);
      // Serial.print(" steps | ");
      Serial.print(factor);
      Serial.print(" step factor | ");
      Serial.print(bpm);
      Serial.println(" bpm");
      frameCount = 0;
      color += 1;
    }
  }
  while (usbMIDI.read()) {
  }
}
