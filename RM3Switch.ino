#include "KeysAndSetup.h" // Blynk setup, token and template ID, Hostanme, OTA and WiFi keys
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoOTA.h>

#include <Ticker.h>
#include <ezButton.h>

#define RELAY_1 D1  // Generator Relay
#define RELAY_2 D2  // Transmitter Relay
#define RELAY_3 D5  // Camera Power Relay

#define VBUTTON_1 V16
#define VBUTTON_2 V17
#define VBUTTON_3 V18

#define METER_1_TIME V41
#define METER_2_TIME V42
#define RUN_TIME V40

#define source1PIN D6  // SWITCH CONTACT
#define source2PIN D7  // SWITCH CONTACT
#define VLED_1 V6
#define VLED_2 V7

WidgetTerminal Terminal(V29);  // debug instance
Ticker powerSourceIN1;
Ticker powerSourceIN2;
Ticker checkPowerOutput;
Ticker gen1Timer;
Ticker gen2Timer;
//instance of BlynkTimer to send data to blynk server.
BlynkTimer timer;

bool debug;
unsigned long startTime;
float calTime;
float meterTime1;
float meterTime2;

#include "HourMeter.h"
#include "SwitchLogic.h"
#include "GridMonitor.h"
#include "ScheduledTimers.h"
#include "Debugging.h"
#include "VoltMeter.h"

// Your wifi settings
char ssid[] = YOUR_SSID;
char pass[] = YOUR_WIFI_PASSWORD;
// char deviceHostName[] = "Power & Monitoring";
char deviceHostName[] = DEVICE_HOSTNAME;

SwitchLogic genSet(RELAY_1, VBUTTON_1, "genSet");
SwitchLogic transmitter(RELAY_2, VBUTTON_2, "transmitter");
SwitchLogic camera(RELAY_3, VBUTTON_3, "ip_camera");

//-- button setup -- //
BLYNK_CONNECTED() {
  Blynk.sendInternal("utc", "time");     // Unix timestamp (with msecs)
  Blynk.sendInternal("utc", "tz_rule");  // POSIX TZ rule
  //sync switch and LED state on connect
  restoreState();
  //pull values from server
  Blynk.syncVirtual(V1);   // alarm1 Time Input
  Blynk.syncVirtual(V24);  // debug switch
  Blynk.syncVirtual(V40);  // Last Run
  Blynk.syncVirtual(V41);  // Total Time1
  Blynk.syncVirtual(V42);  // Total Time2
  Blynk.syncVirtual(V83);  // Voltage Calibrator

  debugPrintOut();
}

BLYNK_WRITE(VBUTTON_1) {
  if (param.asInt() == 1) {
    startGenSet();
  }
  if (param.asInt() == 0) {
    stopGenSet();
  }
}

BLYNK_WRITE(VBUTTON_2) {
  if (param.asInt() == 1) {
    transmitter.start();
  }

  if (param.asInt() == 0) {
    transmitter.stop();
  }
}

BLYNK_WRITE(VBUTTON_3) {
  if (param.asInt() == 1) {
    camera.start();
  }

  if (param.asInt() == 0) {
    camera.stop();
  }
}
// debug button //
BLYNK_WRITE(V24) {
  debug = param.asInt();
  if (debug) {
    Terminal.println("\nDebugging ON !");
  }
}

BLYNK_WRITE(V25) {
  ESP.restart();
}
// V36 - Numeric Widget for Hour Meter Calibration
BLYNK_WRITE(V36) {
  calTime = param.asFloat();
}
// V37 Calibrate Button
BLYNK_WRITE(V37) {
  if (param.asInt() == 1) {
    generator1.setMeterTime(calTime);
    // or and?
    Blynk.virtualWrite(METER_1_TIME, generator1.getMeterTime());
  }
}
// V39 Calibrate Button
BLYNK_WRITE(V39) {
  if (param.asInt() == 1) {
    meterTime2 = calTime;
    // genSetMeter2.setMeterTime(calTime);
    Blynk.virtualWrite(V42, meterTime2);
  }
}
BLYNK_WRITE(METER_1_TIME) {
  // meterTime1 = param.asFloat();
  generator1.setMeterTime(param.asFloat());
}

BLYNK_WRITE(METER_2_TIME) {
  // meterTime2 = param.asFloat();
  generator2.setMeterTime(param.asFloat());
}

//-- Start logic app --//
void startGenSet() {
  genSet.start();
  Blynk.virtualWrite(RUN_TIME, "00:00");
  //change LED label
  Terminal.println(local.dateTime());
  checkPowerOutput.attach_ms(30000, powerOutputCheck);
}

void stopGenSet() {
  genSet.stop();
  Terminal.println(local.dateTime());
  checkPowerOutput.detach();
  // if (isScheduleOn) {
  //   isScheduleOn = false;
  // }
}
void startTransmitter() {
  transmitter.start();
}

void stopTransmitter() {
  transmitter.stop();
}

void startCamera() {
  camera.start();
}

void stopCamera() {
  camera.stop();
}

void restoreState() {
  //get source input state to update LED
  Blynk.virtualWrite(VLED_1, !powerSource1.getState());
  Blynk.virtualWrite(VLED_2, !powerSource2.getState());
  // update generator button if either inputs is ON.
  if (!powerSource1.getState() == 1) {
    Blynk.virtualWrite(VBUTTON_1, HIGH);
  } else {
    Blynk.virtualWrite(VBUTTON_1, LOW);
  }
  if (!powerSource2.getState() == 1) {
    Blynk.virtualWrite(VBUTTON_1, HIGH);
  } else {
    Blynk.virtualWrite(VBUTTON_1, LOW);
  }
}
// -- send Data to Blynk -- //

void sendData() {
  Blynk.virtualWrite(V26, WiFi.RSSI());
  getVoltage();

  // update timer values on server if a source is active.
  if (!powerSource1.getState()) {
    Blynk.virtualWrite(METER_1_TIME, generator1.getMeterTime());
    Blynk.virtualWrite(RUN_TIME, generator1.getRunTime());
  }
  if (!powerSource2.getState()) {
    Blynk.virtualWrite(METER_2_TIME, generator2.getMeterTime());
    Blynk.virtualWrite(RUN_TIME, generator2.getRunTime());
  }
  // Terminal.print("\n" + local.dateTime());
  // Terminal.println("\nData uploaded to server.");
}

void setup() {
  Serial.begin(115200);

  //setup buttons or input power source monitors
  powerSource1.setDebounceTime(50);  // set debounce time to 5 seconds
  powerSource2.setDebounceTime(50);  // set debounce time to 5 seconds

  delay(50);
  powerSourceIN1.attach_ms(491, powerSource1StatusCheck);
  delay(78);
  powerSourceIN2.attach_ms(620, powerSource2StatusCheck);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  WiFi.hostname(deviceHostName);

  // OTA
  ArduinoOTA.setHostname(deviceHostName);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();

    setupSchedules();

  //schedules
  timer.setInterval(15000, checkAllSchedules);  // Check alarms every 30sec
  //send data to blynk
  timer.setInterval(60000L, sendData);  // Every 60sec push Voltage to V3
}

void loop() {
  timer.run();          // Initiates SimpleTimer
  ArduinoOTA.handle();  //Initiate OTA
  Blynk.run();
}
