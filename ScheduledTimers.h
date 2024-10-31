#include <ezTime.h>

// Structure to hold schedule settings
struct Schedule {
    int startHour;
    int startMinute;
    int stopHour;
    int stopMinute;
    int days[7];
    bool isScheduleDay;
    bool isActive;
    void (*startFunction)();
    void (*stopFunction)();
    int blynkVPin;  // Virtual pin number for this schedule
};

Timezone local;

// Define your schedule instances
Schedule genSetSchedule = {0, 0, 0, 0, {0}, false, false, nullptr, nullptr, V1};
Schedule transmitterSchedule = {0, 0, 0, 0, {0}, false, false, nullptr, nullptr, V2};
Schedule cameraSchedule = {0, 0, 0, 0, {0}, false, false, nullptr, nullptr, V3};

// Function declarations
void startGenSet();
void stopGenSet();
void startTransmitter();
void stopTransmitter();
void startCamera();
void stopCamera();

void setupSchedules() {
    // Initialize schedule functions
    genSetSchedule.startFunction = startGenSet;
    genSetSchedule.stopFunction = stopGenSet;
    
    transmitterSchedule.startFunction = startTransmitter;
    transmitterSchedule.stopFunction = stopTransmitter;
    
    cameraSchedule.startFunction = startCamera;
    cameraSchedule.stopFunction = stopCamera;
}

// Generic function to process schedule settings
void processScheduleSettings(Schedule& schedule, TimeInputParam t) {
    if (t.hasStartTime()) {
        schedule.startHour = t.getStartHour();
        schedule.startMinute = t.getStartMinute();
        Terminal.println(String("Start: ") + schedule.startHour + ":" + schedule.startMinute);
    }

    if (t.hasStopTime()) {
        schedule.stopHour = t.getStopHour();
        schedule.stopMinute = t.getStopMinute();
        Terminal.println(String("Stop: ") + schedule.stopHour + ":" + schedule.stopMinute);
    }

    // Process weekdays
    for (int i = 1; i <= 7; i++) {
        schedule.days[i-1] = t.isWeekdaySelected(i) ? i : 0;
        if (t.isWeekdaySelected(i)) {
            Terminal.println(String("Day ") + i + " is selected");
        }
    }
}

// Blynk write handlers for each schedule
BLYNK_WRITE(V1) {  // GenSet Schedule
    TimeInputParam t(param);
    processScheduleSettings(genSetSchedule, t);
}

BLYNK_WRITE(V2) {  // Pump Schedule
    TimeInputParam t(param);
    processScheduleSettings(transmitterSchedule, t);
}

BLYNK_WRITE(V3) {  // Light Schedule
    TimeInputParam t(param);
    processScheduleSettings(cameraSchedule, t);
}

// Generic function to check schedule
void checkSchedule(Schedule& schedule) {
    schedule.isScheduleDay = false;
    
    // Check if today is a scheduled day
    for (int i = 0; i < 7; i++) {
        if (schedule.days[i] == local.weekday()) {
            schedule.isScheduleDay = true;
            break;
        }
    }

    // Check start time
    if (!schedule.isActive && schedule.isScheduleDay && 
        local.hour() == schedule.startHour && local.minute() == schedule.startMinute) {
        schedule.isActive = true;
        if (schedule.startFunction != nullptr) {
            schedule.startFunction();
        }
    }

    // Check stop time
    if (schedule.isActive && schedule.isScheduleDay && 
        local.hour() == schedule.stopHour && local.minute() == schedule.stopMinute) {
        schedule.isActive = false;
        if (schedule.stopFunction != nullptr) {
            schedule.stopFunction();
        }
    }
}

// Function to check all schedules
void checkAllSchedules() {
    checkSchedule(genSetSchedule);
    checkSchedule(transmitterSchedule);
    checkSchedule(cameraSchedule);
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
