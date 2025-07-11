
#include <Wire.h>
#include "Adafruit_TCS34725.h"

// ---------------------------------------- Events

volatile bool on_btn_evt = false;
volatile bool foundShiny = false;

// ---------------------------------------- Pinlayout

#define ON_BUTTON_PIN 2

#define ON_SWITCH 3
#define Start_Button 4
#define Select_Button 5
#define A_Button 6
#define B_Button 7
#define L_Button 8
#define R_Button 9
#define Up_Button 10
#define Down_Button 11
#define Left_Button 12
#define Right_Button 13

// ---------------------------------------- Variables

int resetCount = 0;
unsigned int r, g, b, c, colorTemp, lux;
unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long totalTime = 0;

// ---------------------------------------- Setup

Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_24MS,  // Integration Time
  TCS34725_GAIN_60X                // Gain 
);

void setup() {
  Serial.begin(9600);

  // Initiate TCS3472 IC
  if (tcs.begin()) {
    Serial.println("Found TCS3472 sensor");
  } 
  else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  // Initialize Button Interrupt
  pinMode(ON_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ON_BUTTON_PIN), button_ISR, FALLING);

  // Initialize Pins
  for (int i = 3; i <= 11; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
}

void loop() {

  if(on_btn_evt) {
    on_btn_evt = false;
    SoftReset();
    foundShiny = false;
  }

  if(foundShiny) {

  } else {
    SoftReset();
    delay(10000);
    PressButton(Start_Button);
    delay(3000);
    PressButton(Start_Button);
    delay(3000);
    PressButton(A_Button);
    delay(3000);
    PressButton(Start_Button);
    delay(1000);
    PressButton(A_Button);
    delay(1000);
    PressButton(A_Button);
    delay(7000);

    startTime = millis();

    do {
      tcs.getRawData(&r, &g, &b, &c);
      //Serial.println(r);
    } while (r < g || r < b || r < 100);

    endTime = millis();
    totalTime = endTime - startTime;

    Serial.println(totalTime);

    if (totalTime > 8500){
      foundShiny = true;
    }

    delay(1000); 
  }

}

// ---------------------------------------- Button ISR

void button_ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if ((interruptTime - lastInterruptTime) > 200) {
    on_btn_evt = true;
  }

  lastInterruptTime = interruptTime;
}

// ---------------------------------------- Console Button Press

void PressButton(int btn) {
  Serial.print("Button ");
  Serial.print(btn);
  Serial.print(" pressed\n");
  digitalWrite(btn, LOW);
  delay(200);
  digitalWrite(btn, HIGH);
}

void SoftReset() {
  Serial.println("Soft Reset");

  digitalWrite(Start_Button, LOW);
  digitalWrite(Select_Button, LOW);
  digitalWrite(L_Button, LOW);
  digitalWrite(R_Button, LOW);

  delay(100);

  digitalWrite(Start_Button, HIGH);
  digitalWrite(Select_Button, HIGH);
  digitalWrite(L_Button, HIGH);
  digitalWrite(R_Button, HIGH);

  startTime = 0;
  endTime = 0;
  totalTime = 0;

  resetCount++;
}

void FoundShiny() {
  foundShiny = true;

  String outputString = String("Found After " + String(resetCount));
  Serial.println(outputString + " resets");
}

void ResetCount() {
  resetCount = 0;
  foundShiny = false;
}