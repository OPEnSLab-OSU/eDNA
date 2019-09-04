#pragma once
#include "Arduino.h"
#include "KPConstants.hpp"
#include "KPList.hpp"
#include "KPStatus.hpp"
#include "KPNotificationCenter.hpp"

enum class Direction {
    normal = 0, reverse = 1
};

//===============================================================
// [+_+] State Identifiers
//===============================================================
enum class KPStateIdentifier {stop, flush, sample, dry, preserve, decontaminate1, decontaminate2 ,decontaminate3, idle};

//===============================================================
// [+_+] Alias
//===============================================================
using VoidFunctionPointer = void (*)();
using BoolFunctionPointer = bool (*)();
using ulong = unsigned long;
using uint = unsigned int;

//===============================================================
// [+_+] Helper Fucntions
//===============================================================
extern "C" char *sbrk(int i);

int free_ram () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

void printFreeRam() {
	Serial.println();
	Serial.print("Free Memory: ");
	Serial.println(free_ram());
}

void error(const char * message, int time = 2000) {
	while (true) {
		Serial.println(message);
		delay(time);
	}	
}

template<typename T>
void print(T value) {
	Serial.print(value);
}

template<typename T, typename ...Types>
void print(const T & first, const Types & ...args){
	Serial.print(first);
	Serial.print(" ");
	print(args...);
}

template<typename T>
void println(T value) {
	Serial.println(value);
}

template<typename T, typename ...Types>
void println(const T & first, const Types & ...args){
	Serial.print(first);
	Serial.print(" ");
	println(args...);
}

//===============================================================
// [+_+] C String Fucntions
//===============================================================
bool strStartWith(const char * a, const char * b) {
	return strncmp(a, b, strlen(b));
}


//===============================================================
// [+_+] Array Fucntions
//===============================================================

// Find the index of the element with max or min property
template<typename T> 
int arrayPivot(T * array, int size, bool(*selector)(T & pivot, T & next)) {
	if (size == 1){
		return 0;
	}

	int absolute = 0, count = 1;
	for (int i = 1; i < size; i++) {
		if (!selector(array[absolute], array[i])) {
			absolute = i;
			count++;
		}
	}

	return count == size ? -1 : absolute;
}

template<typename T, int n> 
int arrayPivot(T (&array)[n],  bool(*selector)(T & pivot, T & next)) {
	return arrayPivot<T>(array, n, selector);
}

//===============================================================
// [+_+] Array reduce functional-style methods
//===============================================================
// template<typename output, typename T> 
// output arrayReduce(T * array, int size, void (*callback)(output & accum, T & next)) {
// 	output _accum{};
// 	for (int i = 0; i < size; i++) callback(_accum, array[i]);
// 	return _accum;
// } 

// template<typename output, typename T, int n> 
// output arrayReduce(T (&array)[n], void (*callback)(output & accum, T & next)) {
// 	return arrayReduce<output, T>(array, n, callback);
// } 

template<typename output, typename T> 
output arrayReduce(T * array, int size, output initialValue, void (*callback)(output & accum, T & next, int index)) {
	output _accum{initialValue};
	for  (int i = 0; i < size; i++) callback(_accum, array[i], i);
	return _accum;
} 

template<typename output, typename T, int n> 
output arrayReduce(T (&array)[n], output initialValue, void (*callback)(output & accum, T & next, int index)) {
	return arrayReduce<output, T>(array, n, initialValue, callback);
} 

template<typename T, int n> int arrayFind(T (&array)[n], bool(selector)(T & next)) {
	for (int i = 0; i < n; i++) {
		if (selector(array[i])) return i;
	}

	return -1;
}

template<typename  T> void arrayPrint(T * array, int size) {
	Serial.print("[");
	for (int i = 0; i < size; i++){
		Serial.print(array[i]);
		Serial.print(",");
	}
	Serial.println("]");
}

template<typename T, int n> void arrayPrint(T (&array)[n]) {
	arrayPrint(array, n);
}

//===============================================================
// [+_+] JSON Keys
//===============================================================
namespace JsonKey {
	// [+_+] State
	static constexpr const char * stateName 			{"stateName"};
	static constexpr const char * stateId  				{"stateId"};

	static constexpr const char * stateTimeLimit 		{"time"};
	static constexpr const char * statePressureLimit	{"pressure"};
	static constexpr const char * stateVolumeLimit  	{"volume"};

	static constexpr const char * statePropertyName  	{"name"};
	static constexpr const char * statePropertyValue  	{"value"};
	static constexpr const char * statePropertyMax  	{"max"};
	static constexpr const char * statePropertyMin  	{"min"}; 
	static constexpr const char * statePropertyUnit 	{"unit"};

	// [+_+] Valveb    
	static constexpr const char * valveBegin  			{"valveBegin"};
	static constexpr const char * valveLowerBound  		{"valveLowerBound"};
	static constexpr const char * valveUpperBound  		{"valveUpperBound"};
	static constexpr const char * valveCurrent 			{"valveCurrent"};
	static constexpr const char * valves 				{"valves"};
	static constexpr const char * valveInterval 		{"valveInterval"};

	// [+_+] Time
	static constexpr const char * timeUTC 				{"timeUTC"};

	// [+_+] Sensor Data
	static constexpr const char * pressure 				{"pressure"};
	static constexpr const char * temperature 			{"temperature"};
	static constexpr const char * waterVolume 			{"waterVolume"};
	static constexpr const char * waterFlow 			{"waterFlow"};
	static constexpr const char * waterDepth 			{"waterDepth"};
};

namespace TaskKey {
	static constexpr const char * id					{"id"};
	static constexpr const char * name					{"name"};
	static constexpr const char * creation				{"creation"};
	static constexpr const char * schedule				{"schedule"};
	static constexpr const char * interval				{"interval"};
	static constexpr const char * valves				{"valves"};
	static constexpr const char * numberOfValves		{"numberOfValves"};
	static constexpr const char * notes					{"notes"};
	
	// [+_+] FLush State
	static constexpr const char * flushTime				{"flushTime"};
	static constexpr const char * flushVolume			{"flushVolume"};

	// [+_+] Sample State
	static constexpr const char * sampleTime			{"sampleTime"};
	static constexpr const char * samplePressure		{"samplePressure"};
	static constexpr const char * sampleVolume			{"sampleVolume"};

	// [+_+] Clean State
	static constexpr const char * cleanTime				{"cleanTime"};

	// [+_+] Preserve State
	static constexpr const char * preserveTime			{"preserveTime"};
};