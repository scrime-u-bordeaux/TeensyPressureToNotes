// max velocity is determined by maxValue - midThresh
const int lowthresh   = 200;
const int midthresh   = 600;
const int highthresh  = 800;

const int note    = 60;
const int channel = 1;

int v = 0;
int prevv = 0;
int dv = 0;

bool canSendNoteOn    = true;
bool mustSendNoteOff  = false;

void setup() {}

void loop() {
  v = analogRead(A11);

  if (v <= midthresh && prevv > midthresh) {
    canSendNoteOn = true;
  }

  if (v <= lowthresh && prevv > lowthresh) {
    usbMIDI.sendNoteOff(note, 0, channel);
    mustSendNoteOff = false;
  }

  if (v > highthresh && prevv <= highthresh && canSendNoteOn) {
    if (mustSendNoteOff) {
      usbMIDI.sendNoteOff(note, 0, channel);
    }

    // get velocity from dv :
    dv = v - prevv;
    // we consider maxValue to be 1000
    float ratio = powf((float)(dv) / (1000 - midthresh), 0.5);
    ratio = ratio > 1.f ? 1.f : ratio;
    int velocity = (int)(ratio * 127.f);
    velocity = velocity < 1 ? 1 : velocity;

    usbMIDI.sendNoteOn(note, velocity, channel);
    
    mustSendNoteOff = true;
    canSendNoteOn = false;
  }

  prevv = v;

  delay(10);
}