#pragma once
#define __k_auto constexpr auto

#define RED(x)   "\033[31;1m" x "\033[0m"
#define GREEN(x) "\033[32;1m" x "\033[0m"
#define BROWN(x) "\033[33;1m" x "\033[0m"
#define BLUE(x)  "\033[34;1m" x "\033[0m"

enum class Direction { normal, reverse };

struct ValveBlock {
    const int regIndex;
    const int pinIndex;
};

namespace TPICDevices {
    __k_auto INTAKE_POS     = 0;
    __k_auto INTAKE_NEG     = 1;
    __k_auto AIR_VALVE      = 2;
    __k_auto ALCHOHOL_VALVE = 3;
    __k_auto FLUSH_VALVE    = 4;
};  // namespace TPICDevices

// Make sure LIVE is running on the actual machine. Otherwise, all pins will
// turn on
namespace HardwarePins {
    __k_auto POWER_MODULE = A0;
#if  defined(LIVE) || defined(COMPONENT_TEST)
    // A1 is eDNA, 12 for HYPNOS
    __k_auto RTC_INTERRUPT = A1;
#else
    __k_auto RTC_INTERRUPT = 12;  // Change this to A1 to force using A1 for RTC
#endif
    __k_auto BATTERY_VOLTAGE   = A2;
    //A4 is unused but would be ANALOG_SENSOR_2
    __k_auto ANALOG_SENSOR_1   = A3;
    __k_auto BUTTON_PIN   = 13; //originally 13, but that's built-in led and may cause conflicts
    __k_auto SHUTDOWN_OVERRIDE = A5;
    __k_auto MOTOR_REVERSE     = 5;
    __k_auto MOTOR_FORWARD     = 6;
    __k_auto SHFT_REG_LATCH    = 9;
    __k_auto SD_CARD           = 10;
    __k_auto SHFT_REG_CLOCK    = 11;
    __k_auto SHFT_REG_DATA     = 12;
};  // namespace HardwarePins

namespace ProgramSettings {
    __k_auto CONFIG_FILE_PATH          = "config.js";
    __k_auto SD_FILE_NAME_LENGTH       = 13;
    __k_auto CONFIG_JSON_BUFFER_SIZE   = 800;
    __k_auto STATUS_JSON_BUFFER_SIZE   = 800;
    __k_auto TASK_JSON_BUFFER_SIZE     = 1000;
    __k_auto TASKREF_JSON_BUFFER_SIZE  = 50;
    __k_auto MAX_VALVES                = 24;
    __k_auto VALVE_JSON_BUFFER_SIZE    = 500;
    __k_auto VALVEREF_JSON_BUFFER_SIZE = 50;
    __k_auto VALVE_GROUP_LENGTH        = 25;
};  // namespace ProgramSettings

namespace TaskSettings {
    __k_auto NAME_LENGTH  = 25;
    __k_auto GROUP_LENGTH = 25;
    __k_auto NOTES_LENGTH = 80;
};  // namespace TaskSettings

//
// ────────────────────────────────────────────────────────── I ──────────
//   :::::: J S O N   K E Y S : :  :   :    :     :        :          :
// ────────────────────────────────────────────────────────────────────
//
// These are used to retrieve values from ArduinoJson's JSON Object

namespace ConfigKeys {
    __k_auto VALVES_FREE       = "freeValves";
    __k_auto VALVE_UPPER_BOUND = "valveUpperBound";
    __k_auto FILE_LOG          = "logFile";
    __k_auto FILE_STATUS       = "statusFile";
    __k_auto FOLDER_TASK       = "taskFolder";
    __k_auto FOLDER_VALVE      = "valveFolder";
    __k_auto PRESSURE_CUTOFF   = "cutoffPressure";
    __k_auto PRESSURE_MAX      = "maxPressure";
}  // namespace ConfigKeys

namespace TaskKeys {
    __k_auto ID              = "id";
    __k_auto NAME            = "name";
    __k_auto STATUS          = "status";
    __k_auto VALVES          = "valves";
    __k_auto VALVES_OFFSET   = "valvesOffset";
    __k_auto CREATED_AT      = "createdAt";
    __k_auto SCHEDULE        = "schedule";
    __k_auto TIME_BETWEEN    = "timeBetween";
    __k_auto NOTES           = "notes";
    __k_auto DELETE          = "deleteOnCompletion";
    __k_auto FLUSH_TIME      = "flushTime";
    __k_auto FLUSH_VOLUME    = "flushVolume";
    __k_auto SAMPLE_TIME     = "sampleTime";
    __k_auto SAMPLE_VOLUME   = "sampleVolume";
    __k_auto DRY_TIME        = "dryTime";
    __k_auto PRESERVE_TIME   = "preserveTime";
    __k_auto CURR_VALVE   = "currentValve";
}  // namespace TaskKeys

namespace ValveKeys {
    __k_auto ID     = "id";
    __k_auto STATUS = "status";
    __k_auto GROUP  = "group";
}  // namespace ValveKeys

namespace StatusKeys {
    __k_auto VALVE_ID        = "valveId";
    __k_auto VALVE_GROUP     = "valveGroup";
    __k_auto VALVE_SCHEDULE  = "valveSchedule";
    __k_auto VALVE_CURRENT   = "valveCurrent";
    __k_auto VALVE_STATUS    = "valveStatus";
    __k_auto VALVES          = "valves";
    __k_auto VALVES_COUNT    = "valvesCount";
    __k_auto SENSOR_PRESSURE = "pressure";
    __k_auto SENSOR_TEMP     = "temperature";
    __k_auto SENSOR_BARO     = "barometric";
    __k_auto SENSOR_VOLUME   = "waterVolume";
    __k_auto SENSOR_FLOW     = "waterFlow";
    __k_auto SENSOR_DEPTH    = "waterDepth";
    __k_auto CURRENT_TASK    = "currentTask";
    __k_auto CURRENT_STATE   = "currentState";
    __k_auto LOW_BATTERY     = "lowBattery";
    __k_auto SAMPLE_VOLUME   = "sampleVolume";
    __k_auto PRESSURE_CUTOFF   = "cutoffPressure";
};  // namespace StatusKeys

#undef __k_auto
