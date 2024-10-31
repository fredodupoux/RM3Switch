/*
#include <ezTime.h>
Timezone local;
int genSetStartHour = 0;
int genSetStartMinute = 0;
int genSetStopHour = 0;
int genSetStopMinute = 0;
//
int genSetDays[7] = {0};
bool isGenSetScheduleDay = false;
bool isGenSetScheduleOn = false;

void startGenSet ();
void stopGenSet ();

// ezTime
BLYNK_WRITE(InternalPinUTC) {
    String cmd = param[0].asStr();
    if (cmd == "time") {
        const uint64_t utc_time = param[1].asLongLong();
        UTC.setTime(utc_time / 1000, utc_time % 1000);
    } else if (cmd == "tz_rule") {
        String tz_rule = param[1].asStr();
        local.setPosix(tz_rule);
    }
}
  // Terminal.print("\n" + local.dateTime());

BLYNK_WRITE(V1) {   //genSet
  TimeInputParam t(param);

  // Process start time
  if (t.hasStartTime()) {
    genSetStartHour = t.getStartHour();
    genSetStartMinute = t.getStartMinute();
    Terminal.println(String("Start: ") + genSetStartHour + ":" + genSetStartMinute);
  } else {
    // Do nothing
  }

  // Process stop time
  if (t.hasStopTime()) {
    genSetStopHour = t.getStopHour();
    genSetStopMinute = t.getStopMinute();
    Terminal.println(String("Stop: ") + genSetStopHour + ":" + genSetStopMinute);
  } else { }
  // Process timezone
  // Timezone is already added to start/stop time
  Terminal.println(String("Time zone: ") + t.getTZ());
  // Get timezone offset (in seconds)
  Terminal.println(String("Time zone offset: ") + t.getTZ_Offset());

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)
  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      Terminal.println(String("Day ") + i + " is selected");
      genSetDays[i] = i; 
    }
  }

  genSetStopMinute = t.getStopMinute();
}


void checkGenSetSchedule() {
  for (int i = 1; i <= 7; i++) {
    if (genSetDays[i] == local.weekday()) {
      isGenSetScheduleDay = true;
      break;
    }
  }
  // If schedule is not active and it's a scheduled day and the current time matches the start time, trigger the alarm
  if (!isGenSetScheduleOn && isGenSetScheduleDay && local.hour() == genSetStartHour && local.minute() == genSetStartMinute) {
    isGenSetScheduleOn = true;
    // Terminal.println("RING THE ALARM!");
    startGenSet();
  } else {
    // do nothing
  }

  // If it's a scheduled day and the current time matches the stop time, turn off the alarm
  if (isGenSetScheduleOn && isGenSetScheduleDay && local.hour() == genSetStopHour && local.minute() == genSetStopMinute) {
    isGenSetScheduleOn = false;
    // Terminal.println("TURN OFF THE ALARM!");
        stopGenSet();

  } else {
    // do nothing
  }
