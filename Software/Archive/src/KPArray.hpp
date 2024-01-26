#pragma once
#include "Arduino.h"

template <typename T>
class Array {
public:
    T * data = nullptr;
    int capacity = 10;
public:
    int size = 0;
    
protected:
    void resize() {
        T * ptr = data;
        capacity = capacity * 2;
        data = new T[capacity];
        for (int i = 0; i < size; i++) {
            data[i] = ptr[i];
        }
        
        delete[] ptr;
    }

	void resizeIfFull() {
		if (size == capacity) {
			resize();
		}
	}
public:
    Array<T>() {
        data = new T[capacity];
    }

	Array<T>(int cap) {
		capacity = cap;
		data = new T[cap];
	}
    
    Array<T>(const Array<T> &obj) {
        delete[] data;
        data = new T[obj.capacity];
        for (int i = 0; i < obj.size; i++) {
            data[i] = obj.data[i];
        }
    }
    
    ~Array() {
        size = 0;
        capacity = 0;
        delete[] data;
    }
    
    int append(T value) {
        if (size >= capacity) {
            resize();
        }
        
        data[size] = value;
        size++;
        return size;
    }

	int append(T * values, int size) {
		for (int i = 0; i < size; i++) {
			append(values[i]);
		}

		return size;
	}
    
    bool remove(int index) {
        if (index < 0 || index >= size) {
            while(true) {
               Serial.print(F("REMOVE | Invalid Index: "));
               Serial.println(index);
               delay(2000);
            }
        }
        
        for (int i = index; i < size - 1; i++) {
            data[i] = data[i + 1];
        }
        
        size--;
        return true;
    }

	bool removeAll() {
		for (int i = size - 1; i >= 0; i--) {
			remove(i);
		}

		return true;
	}
    
    
    bool removeLast() {
        return remove(size - 1);
    }
    
    T & get(int index) {
        if (index < 0 || index >= size) {
            while(true) {
               Serial.print(F("GET | Invalid Index: "));
               Serial.println(index);
               delay(3000);
            }
        }
        
        return data[index];
    }

	T & operator[](int index) {
		return get(index);
	}

	template<int n> void replace(T (&array)[n], int start = 0) {
		for (int i = start; i < start + n; i++) {
			delete data[i];
			data[i] = array[i];
		}
	}

	void toArray(T * array) {
		for (int i = 0; i < size; i++) {
			array[i] = data[i];
		}
	}

	void print() {
		Serial.print("[");
		for (int i = 0; i < size; i++) {
			Serial.print(get(i));
			Serial.print(", ");
		}
		Serial.print("]");
	}
};
