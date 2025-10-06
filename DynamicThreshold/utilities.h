// 1st order derivative -------------------------------------------------------

class Delta {
  bool firstcall;
  float v;
  float prevv;

public:
  Delta() : firstcall(true), prevv(0) {}

  float process(float vin) {
    if (firstcall) {
      prevv = vin;
      firstcall = false;
    }

    v = vin - prevv;
    prevv = vin;
    return v;
  }
};

// schmitt trigger ------------------------------------------------------------

class Schmitt {
  float hithresh;
  float lothresh;
  bool on;

public:
  Schmitt(float hi, float lo) : hithresh(hi), lothresh(lo) {}

  bool getState() { return on; }

  // returns true when the internal state changes, in which case
  // getState() should be called to get the new state
  bool process(float vin) {
    if (!on && vin > hithresh) {
      on = true;
      return true;
    } else if (on && vin < lothresh) {
      on = false;
      return true;
    }

    return false;
  }
};

// min max monitoring ---------------------------------------------------------

class MinMax {
  float min;
  float max;

public:
  MinMax() {
    reset();
  }

  void reset() {
    min = +1e+9;
    max = -1e+9;
  }

  float getMin() { return min; }
  float getMax() { return max; }

  // returns true when an extremum changes, in which case
  // getMin() and getMax() should be called to get the new extrema
  bool process(float vin) {
    bool changed = false;

    if (vin < min) {
      min = vin;
      changed = true;
    }

    if (vin > max) {
      max = vin;
      changed = true;
    }

    return changed;
  }
};