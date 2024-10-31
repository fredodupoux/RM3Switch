/*
Hour meter class, to create hour meters and keep
time of running equipement. 

Author: Frederic Dupoux
Date : March 26 2024
Version: 1.3
*/

class HourMeter {
private:
  bool counter;
  int runHours;
  int secsRemaining;
  int runMinutes;
  int runSeconds;
  String HHmm;
  unsigned long startMillis;
  unsigned long reStartMillis = 0;
  unsigned long endMillis;
  unsigned long allSeconds;
  unsigned long currentTime;
  float runTime;
  float totalTime;

public:
  HourMeter() {
    counter = false;
    runTime = 0.0;
    totalTime = 0.0;
  }

  void startMeter() {
    startMillis = millis();
    counter = true;
    Terminal.println("Meter Started");
  }

  void stopMeter() {
    if (counter) {
      // stop counter
      endMillis = millis();
      counter = false;
      Terminal.println("Meter Stopped");

      //  compute total time
      allSeconds = (endMillis - startMillis) / 1000;
      runTime = allSeconds * 0.0002777777778;
      totalTime += runTime;
      // setMeterTime(totalTime);
      // Terminal.println(runTime);

      // compute last run in HH:MM
      secsRemaining = allSeconds % 86400;
      runHours = secsRemaining / 3600;
      secsRemaining = allSeconds % 3600;
      runMinutes = secsRemaining / 60;

      // sprintf(buf, "%02d:%02d", runHours, runMinutes);  // Update buf with last run time
      HHmm = String(runHours) + ":" + String(runMinutes);
      // Terminal.println(buf);
    }
  }
  
    String millisToHours (long totalMillis) {
    String hhMM;
    runTime = totalMillis * 0.0002777777778;
    secsRemaining = totalMillis % 86400;
    runHours = secsRemaining / 3600;
    secsRemaining = totalMillis % 3600;
    runMinutes = secsRemaining / 60;
    hhMM = String(runHours) + ":" + String(runMinutes);

    return hhMM;
  }


  void runningTimer() {
      if (reStartMillis == 0) {
        runSeconds = 0;
      } else {
        runSeconds = (millis() - reStartMillis) / 1000;
        runTime = runSeconds * 0.0002777777778;
      }
      //update restartMillis
      reStartMillis = millis();
      // update variables.
      allSeconds += runSeconds;
      totalTime += runTime; 
      HHmm = millisToHours(allSeconds);
      // Terminal.println("\nTimer updated.");  
      }

  void setRestartMillis(){
    reStartMillis = millis();
  }

  void resetTimerValues(){
    runTime = 0;
    allSeconds = 0;
    reStartMillis = 0;
  }
// meter1.setMeterTime(getOnGoingTimer())

  void setMeterTime(float time) {
   totalTime = time;
  }

  float getMeterTime() {
    return totalTime;
  }

  String getRunTime() {
    int hours = runHours;
    int minutes = runMinutes;

    // Format the hours and minutes with leading zeros if necessary
    String hoursStr = (hours < 10) ? "0" + String(hours) : String(hours);
    String minutesStr = (minutes < 10) ? "0" + String(minutes) : String(minutes);
    // Combine the hours and minutes into the HH:MM format
    return hoursStr + ":" + minutesStr;
  }
};