/*
#include "DHT.h"

#define DHTPIN 2
#define TYPE DHT11

float tmp = 0.00;
float hum = 0.00;
DHT dht(DHTPIN, TYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  
  hum = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(String(hum));

  tmp = dht.readTemperature();
  Serial.print("\tTemperature: ");
  Serial.print(String(tmp));

  Serial.print("\n");
  delay(500);
}
*/

#include <DHT11.h>

DHT11 dht11(2);

void setup() {
    Serial.begin(9600);
}

void loop() {
    int temperature, humidity;

    int result = dht11.readTemperatureHumidity(temperature, humidity);

    if (result == 0) {
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print(",Humidity:");
        Serial.println(humidity);
    } else {
        Serial.println(DHT11::getErrorString(result));
    }
}
