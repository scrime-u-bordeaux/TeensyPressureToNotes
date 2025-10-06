/*
 * This code transforms a pressure sensor into
 * a mechanism similar to the double escapement
 * action of a piano key.
 * It first analyzes the pressure variation (dp/dt)
 * and when it exceeds a high threshold (at t = t0), takes
 * the actual pressure as the lowest value for this note
 * and below which a NOTE_OFF will always be sent.
 * Then, when the pressure variation decreases below a low
 * (but still > 0) threshold (at t = t1), the peak pressure
 * between t0 and t1 is saved as the peak pressure for this
 * note and a NOTE_ON is sent, the velocity of which is
 * computed according to the pressure variation peak between
 * t0 and t1.
 * The note repetition escape threshold value is computed
 * from the lowest and peak pressure values previously saved
 * and a parameterizable ratio, using the following equation :
 * threshold = low + ratio * (peak - lowest).
 * A NOTE_ON can be sent if no NOTE_OFF has been sent yet,
 * provided that the pressure has decreased below this threshold.
 */

// TODO : impose a fixed minimum for (peak - lowest) to prevent involuntary
// successive NOTE_ON events too close in time.
// But how ? this would mean that no notes below a certain peak pressure could
// be triggered ? (sounds reasonable)
// we could : use the algorithm described above if peak - lowest is high enough,
// otherwise wait until peak - lowest is high enough while dp is still >= 0, and
// otherwise don't trig the note if peak - lowest is not high enough
// after dp < 0

#include "utilities.h"

const int minVal = 17;
const int maxVal = 1015;
const float valScale = 1.f / (maxVal - minVal);

float v;
float prevv;
float dv;

float hithresh = 0.05; //0.02;
float lothresh = 0.01;
float escapeRatio = 0.5;

float peakdv;
float startv;
float peakv;
float escapev;

bool canSendNoteOn = true;
bool canSendNoteOff = false;

//Delta delta;
Schmitt schmitt(hithresh, lothresh);

const int channel = 1;

int velocityFromDeltaPressure(float dp) {
  float res = powf(2 * dp, 0.5);
  res = res < 0.f ? 0.f : (res > 1.f ? 1.f : res);
  return (int)(res * 126) + 1;
}

void setup() {
  // Serial.begin(115200);
  prevv = 0;
}

void loop() {
  // read pressure input :
  int val = analogRead(A11);

  // normalize input :
  val = val > maxVal ? maxVal : (val < minVal ? minVal : val);
  v = (val - minVal) * valScale;

  // get pressure variation speed :
  // dv = delta.process(v);
  dv = v - prevv;

  // process schmitt trigger :
  if (schmitt.process(dv)) {
    if (schmitt.getState()) {
      peakdv = 0;
      peakv = 0;
      // we use startv to trig noteoff
      // startv = v;
      startv = prevv;
    } else {
      if (canSendNoteOn) {
        usbMIDI.sendNoteOn(60, velocityFromDeltaPressure(peakdv), channel);
        canSendNoteOff = true;
        canSendNoteOn = false; 
      }

      if (v > peakv) v = peakv;
      escapev = (peakv - startv) * escapeRatio + startv;
    }
  } else {
    if (schmitt.getState()) {
      peakdv = dv > peakdv ? dv : peakdv;
      peakv = v > peakv ? v : peakv;
    }

    if (!canSendNoteOn && v < escapev) {
      canSendNoteOn = true;
    } 

    if (canSendNoteOff && v < startv) {
    // if (canSendNoteOff && v < 0.01f) {
      usbMIDI.sendNoteOff(60, 0, channel);
      canSendNoteOff = false;
    }
  }

  // Serial.println(val);
  // usbMIDI.sendPitchBend(map(val, 0.f, 1023.f, -8192, 8191), channel);
  prevv = v;
  delay(10);
}
