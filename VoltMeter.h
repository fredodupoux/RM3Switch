// samples to collect and pass to algorithm
#define FILTER_SAMPLES 19
int sensSmoothArray[FILTER_SAMPLES];
int voltMeterIn = A0; // analog input PIN
float voltageCalibrator = 0.0299;

#include "NoiseAlgorithm.h"

// set voltage calibrator
BLYNK_WRITE(V38) {
  float vIn;
  // voltageCalibrator = param.asFloat();
  vIn = param.asFloat();
  voltageCalibrator = vIn / analogRead(voltMeterIn);
  Blynk.virtualWrite(V83, voltageCalibrator);

}

// Function to measure the battery voltage based on noise reducing algorithm values
float batteryVoltage() {
  float sortedValue;  // variables for sensor1 data
      // read multiple values and average them out
      for (int count = 0; count < FILTER_SAMPLES; count++) {
      sortedValue = digitalSmooth(analogRead(voltMeterIn), sensSmoothArray);
    }
  // compute measurements conversion based on average readings and set calibrator
  float voltage = sortedValue * voltageCalibrator; 
  return voltage;
}
// Function to map values to percentage 
int voltageMap(float value, float fromLow, float fromHigh, int toLow, int toHigh){
  return value * toHigh / fromHigh;
}
// get battery percentage for a 5V reading 10.50V to 12.7V
int batteryPercent() {
  return voltageMap(batteryVoltage(), 10.50, 12.70, 0, 100);
}

// void getVoltage() {
//     Blynk.virtualWrite(V43, batteryVoltage());
//     Blynk.virtualWrite(V44, batteryPercent());
//   }