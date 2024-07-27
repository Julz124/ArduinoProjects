#include <HCSR04.h>
#include <Wire.h>

UltraSonicDistanceSensor distanceSensor(2, 4);

byte i2c_rcv;               // data received from I2C bus
unsigned long time_start;   // start time in milliseconds
int stat_LED;               // status of LED: 1 = ON, 0 = OFF
byte value_pot;             // potentiometer position

void setup () {
    Serial.begin(9600);
    Wire.begin(); 
}

void loop () {
    float distance = distanceSensor.measureDistanceCm();
    Serial.println(distance);

    byte floatBytes[sizeof(float)];
    memcpy(floatBytes, &distance, sizeof(float));

    Wire.beginTransmission(8);
    Wire.write(floatBytes, sizeof(float)); 
    Wire.endTransmission();
    delay(500);





}