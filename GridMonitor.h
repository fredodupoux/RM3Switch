//instances of ezButton to monitor power inputs.
ezButton powerSource1(source1PIN);
ezButton powerSource2(source2PIN);
extern float meterTime1;
extern float meterTime2;
//instance HourMeters to keep track of power source time.
HourMeter generator1;
HourMeter generator2;

void powerSource1StatusCheck() {  // EDH SWITCH
  powerSource1.loop();

  if (powerSource1.isPressed()) {
    //set power source state, virtual LED, and log event
    Blynk.virtualWrite(VLED_1, !powerSource1.getState());     //sync vLED state with source input
    Blynk.virtualWrite(VBUTTON_1, !powerSource1.getState());  //sync vButton with source in case of external trigger.
    Blynk.logEvent("powerSource1_on");
    Terminal.println("Source 1 activated.");
    //start meter
    generator1.setRestartMillis();
    Blynk.setProperty(RUN_TIME, "label", "Running...");
    gen1Timer.attach_ms(60000, std::bind(&HourMeter::runningTimer, &generator1));
    // or
    // gen1Timer.attach_ms(60000, [&]() {generator1.OnGoingTimer();});
  }

  if (powerSource1.isReleased()) {
    Blynk.virtualWrite(VLED_1, !powerSource1.getState());     //sync vLED state with source input
    Blynk.virtualWrite(VBUTTON_1, !powerSource1.getState());  //sync vButton with source in case of external trigger.
    Blynk.logEvent("powerSource1_off");
    Terminal.println("Source 1 deactivated.");
    Blynk.setProperty(RUN_TIME, "label", "Last Run");
    //stop meter, and reset store values.
    gen1Timer.detach();
    generator1.resetTimerValues();
    // udate data in blynk.
    Blynk.virtualWrite(RUN_TIME, generator1.getRunTime());
    Blynk.virtualWrite(METER_1_TIME, generator1.getMeterTime());
  }
}

void powerSource2StatusCheck() {  // EDH SWITCH
  powerSource2.loop();

  if (powerSource2.isPressed()) {
    //set power source state, virtual LED, and log event
    Blynk.virtualWrite(VLED_2, !powerSource2.getState());     //sync vLED state with source input
    Blynk.virtualWrite(VBUTTON_1, !powerSource2.getState());  //sync vButton with source in case of external trigger.
    Blynk.logEvent("powerSource2_on");
    Terminal.println("Source 2 actived.");
     //start meter
    generator2.setRestartMillis();
    Blynk.setProperty(RUN_TIME, "label", "Running...");
    gen2Timer.attach_ms(60000, std::bind(&HourMeter::runningTimer, &generator2));
  }

  if (powerSource2.isReleased()) {
    Blynk.virtualWrite(VLED_2, !powerSource2.getState());     //sync vLED state with source input
    Blynk.virtualWrite(VBUTTON_1, !powerSource2.getState());  //sync vButton with source in case of external trigger.
    Blynk.logEvent("powerSource2_off");
    Terminal.println("Source 2 deactivated.");
   //stop meter, and reset store values.
    gen2Timer.detach();
    generator2.resetTimerValues();
    // udate data in blynk.
    Blynk.virtualWrite(RUN_TIME, generator2.getRunTime());
    Blynk.virtualWrite(METER_2_TIME, generator2.getMeterTime());
  }
}
void powerOutputCheck() {
  // checking to see if generator is sending power
  if ( millis() - startTime >= 60000 ) {      //verifies 60 seconds have passed since start function was sent.
    if (powerSource1.getState() && powerSource2.getState()) {  //verifies if one input is active. if not output terminal and logevent
      Terminal.println("No power source detected. Please verify generator.");
      Blynk.logEvent("no_power_detected");
      checkPowerOutput.detach();  // stops checking after reporting once.
    } else {
            checkPowerOutput.detach();  // stops if power is In.
    }
  }
}