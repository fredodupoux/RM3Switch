// samples to collect and pass to algorithm
#define FILTER_SAMPLES 19
int sensSmoothArray[FILTER_SAMPLES];
int voltMeterIn = A0;
float voltage;
float vCalibrator = 0.0299;

#include "NoiseAlgorithm.h"

// set voltage calibrator
BLYNK_WRITE(V38) {
  float vIn;
  // vCalibrator = param.asFloat();
  vIn = param.asFloat();
  vCalibrator = vIn / analogRead(voltMeterIn);
  Blynk.virtualWrite(V83, vCalibrator);

}

// measuring loop sending VOLTAGE value to median algorithm
void measureVoltage() {
  float sortedValue;  // variables for sensor1 data
    for (int count = 0; count < FILTER_SAMPLES; count++) {
      sortedValue = digitalSmooth(analogRead(voltMeterIn), sensSmoothArray);
    }
    // compute and update voltage value
    voltage = (sortedValue * vCalibrator);
    Blynk.virtualWrite(V43, voltage);
  }