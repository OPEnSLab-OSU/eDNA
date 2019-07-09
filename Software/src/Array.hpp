#include "Arduino.h"
template <typename T>
class Array {
public:
    T * data = nullptr;
    int capacity = 10;
public:
    int size = 0;
    
public:
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

	void _set(int index, T value) {
		data[index] = value;
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
};
