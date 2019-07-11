#include <Wire.h>
#include <Arduino.h>

#define SENSORADDRESS 0x07 // each I2C object has a unique bus address, the DS1307 is 0x68

void setup()
{
 Wire.begin(); // wake up I2C bus
 delay (500);
 Serial.begin(9600);
}


void loop()
{

    Wire.beginTransmission(SENSORADDRESS); // "Hey, SENSOR @ Address! Message for you"
    Wire.write(1);  // send a bit asking for register one, the data register (as specified by the pdf)
    Wire.endTransmission(); // "Thanks, goodbye..."
    // now get the data from the sensor
  
    delay (20);

    Wire.requestFrom(SENSORADDRESS, 2);
    while(Wire.available() == 0);
    byte a     = Wire.read(); // first received byte stored here ....Example bytes one: 00011001 10000000
    //delay(5);
    byte b     = Wire.read(); // second received byte stored here ....Example bytes two: 11100111 00000000
    //delay(5);
	
	int flow =  [(a << 8) + b] / 10;

           Serial.print("Flow    (mL / min) ");
           Serial.println(flow);	
           delay (1000);
}
