#include <ezTime.h>

// Structure to hold timer settings
struct Timer {
    int startHour;
    int startMinute;
    int stopHour;
    int stopMinute;
    int days[7];
    bool isScheduleDay;
    bool isScheduleOn;
    void (*startFunction)();
    void (*stopFunction)();
    int blynkVPin;  // Virtual pin number for this timer
};

Timezone local;

// Define your timer instances
Timer genSetTimer = {0, 0, 0, 0, {0}, false, false, nullptr, nullptr, V1};
Timer pumpTimer = {0, 0, 0, 0, {0}, false, false, nullptr, nullptr, V2};
Timer lightTimer = {0, 0, 0, 0, {0}, false, false, nullptr, nullptr, V3};

// Function declarations
void startGenSet();
void stopGenSet();
void startPump();
void stopPump();
void startLight();
void stopLight();

void setupTimers() {
    // Initialize timer functions
    genSetTimer.startFunction = startGenSet;
    genSetTimer.stopFunction = stopGenSet;
    
    pumpTimer.startFunction = startPump;
    pumpTimer.stopFunction = stopPump;
    
    lightTimer.startFunction = startLight;
    lightTimer.stopFunction = stopLight;
}

// Generic function to process timer settings
void processTimerSettings(Timer& timer, TimeInputParam t) {
    if (t.hasStartTime()) {
        timer.startHour = t.getStartHour();
        timer.startMinute = t.getStartMinute();
        Terminal.println(String("Start: ") + timer.startHour + ":" + timer.startMinute);
    }

    if (t.hasStopTime()) {
        timer.stopHour = t.getStopHour();
        timer.stopMinute = t.getStopMinute();
        Terminal.println(String("Stop: ") + timer.stopHour + ":" + timer.stopMinute);
    }

    // Process weekdays
    for (int i = 1; i <= 7; i++) {
        timer.days[i-1] = t.isWeekdaySelected(i) ? i : 0;
        if (t.isWeekdaySelected(i)) {
            Terminal.println(String("Day ") + i + " is selected");
        }
    }
}

// Blynk write handlers for each timer
BLYNK_WRITE(V1) {  // GenSet Timer
    TimeInputParam t(param);
    processTimerSettings(genSetTimer, t);
}

BLYNK_WRITE(V2) {  // Pump Timer
    TimeInputParam t(param);
    processTimerSettings(pumpTimer, t);
}

BLYNK_WRITE(V3) {  // Light Timer
    TimeInputParam t(param);
    processTimerSettings(lightTimer, t);
}

// Generic function to check timer schedule
void checkTimerSchedule(Timer& timer) {
    timer.isScheduleDay = false;
    
    // Check if today is a scheduled day
    for (int i = 0; i < 7; i++) {
        if (timer.days[i] == local.weekday()) {
            timer.isScheduleDay = true;
            break;
        }
    }

    // Check start time
    if (!timer.isScheduleOn && timer.isScheduleDay && 
        local.hour() == timer.startHour && local.minute() == timer.startMinute) {
        timer.isScheduleOn = true;
        if (timer.startFunction != nullptr) {
            timer.startFunction();
        }
    }

    // Check stop time
    if (timer.isScheduleOn && timer.isScheduleDay && 
        local.hour() == timer.stopHour && local.minute() == timer.stopMinute) {
        timer.isScheduleOn = false;
        if (timer.stopFunction != nullptr) {
            timer.stopFunction();
        }
    }
}

// Function to check all timer schedules
void checkAllSchedules() {
    checkTimerSchedule(genSetTimer);
    checkTimerSchedule(pumpTimer);
    checkTimerSchedule(lightTimer);
}

// UTC time handler (unchanged)
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