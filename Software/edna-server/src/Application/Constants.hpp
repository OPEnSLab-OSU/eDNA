#pragma once

#define Power_Module_Pin	A0
#define RTC_Interrupt_Pin	A1
#define Battery_Voltage_Pin A2
#define Analog_Sensor_1_Pin A3
#define Analog_Sensor_2_Pin A4
#define Override_Mode_Pin	A5
#define Motor_Forward_Pin	5
#define Motor_Reverse_Pin	6
#define SR_Latch_Pin		9
#define SDCard_Pin			10
#define SR_Clock_Pin		11
#define SR_Data_Pin			12
#define Button_Pin			13

#define consts constexpr const char *
#define consti constexpr int

namespace ProgramSettings {
	consts CONFIG_FILE_PATH	   = "config.js";
	consti SD_FILE_NAME_LENGTH = 13;

	consti CONFIG_JSON_BUFFER_SIZE = 500;
	consti STATUS_JSON_BUFFER_SIZE = 500;

	consti TASK_JSON_BUFFER_SIZE	= 500;
	consti TASKREF_JSON_BUFFER_SIZE = 50;

	consti MAX_VALVES				 = 24;
	consti VALVE_JSON_BUFFER_SIZE	 = 500;
	consti VALVEREF_JSON_BUFFER_SIZE = 50;

	consti VALVE_GROUP_LENGTH = 25;

};	// namespace ProgramSettings

namespace ConfigKeys {
	consts VALVES_FREE		 = "freeValves";
	consts VALVE_UPPER_BOUND = "valveUpperBound";
	consts FILE_LOG			 = "logFile";
	consts FILE_STATUS		 = "statusFile";
	consts FOLDER_TASK		 = "taskFolder";
	consts FOLDER_VALVE		 = "valveFolder";
}  // namespace ConfigKeys

namespace TaskSettings {
	consti NAME_LENGTH	= 25;
	consti GROUP_LENGTH = 25;
	consti NOTES_LENGTH = 128;
};	// namespace TaskSettings

namespace JsonKeys {
	consts STATE_ID				 = "stateId";
	consts STATE_CURRENT_NAME	 = "stateCurrentName";
	consts STATE_NAME			 = "stateName";
	consts STATE_INDEX			 = "stateIndex";
	consts VALVES				 = "valves";
	consts VALVES_COUNT			 = "valvesCount";
	consts VALVE_ID				 = "valveId";
	consts VALVE_GROUP			 = "valveGroup";
	consts VALVE_SCHEDULE		 = "valveSchedule";
	consts VALVE_CURRENT		 = "valveCurrent";
	consts VALVE_STATUS			 = "valveStatus";
	consts VALVE_FLUSH_TIME		 = "valveFlushTime";
	consts VALVE_FLUSH_VOLUME	 = "valveFlushVolume";
	consts VALVE_SAMPLE_TIME	 = "valveSampleTime";
	consts VALVE_SAMPLE_PRESSURE = "valveSamplePressure";
	consts VALVE_SAMPLE_VOLUME	 = "valveSampleVolume";
	consts VALVE_DRY_TIME		 = "valveDryTime";
	consts VALVE_PRESERVE_TIME	 = "valvePreserveTime";
	consts TASK_CURRENT_NAME	 = "taskCurrentName";
	consts TIME_UTC				 = "timeUTC";
	consts SENSOR_PRESSURE		 = "pressure";
	consts SENSOR_TEMP			 = "temperature";
	consts SENSOR_BARO			 = "barometric";
	consts SENSOR_VOLUME		 = "waterVolume";
	consts SENSOR_FLOW			 = "waterFlow";
	consts SENSOR_DEPTH			 = "waterDepth";
};	// namespace JsonKeys

#undef consts
#undef consti
