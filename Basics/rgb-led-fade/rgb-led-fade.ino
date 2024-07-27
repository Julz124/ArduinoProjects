#include <avr/sleep.h>

// ---------------------------------------- Events

int fade_evt = 0;
int blink_evt = 0;

// ---------------------------------------- Pinlayout

int LEDrot_1 = 3;
int LEDgruen_1 = 5;
int LEDblau_1 = 6;

int LEDrot_2 = 9;
int LEDgruen_2 = 10;
int LEDblau_2 = 11;

int BTN_pin = 2;

// ---------------------------------------- RGB fade variables

int r = 255;
int g = 0;
int b = 0;

int fade_cnt = 0;

// ---------------------------------------- RGB blink variables

int blink_state = 0;

int rgb_pins[] = {LEDrot_1, LEDgruen_1, LEDblau_1, LEDrot_2, LEDgruen_2, LEDblau_2};
int red = 0;
int green = 1;
int blue = 2;

// ---------------------------------------- RGB LED Routine Counter

int counter = 0;

// ---------------------------------------- Timer Initialisation

// 16-Bit Timer
void setupTimer1() {
  //Timer 1 (interrupt each ~1s)
  TCCR1A = 0;                 // Reset entire TCCR1A to 0
  TCCR1B = 0;                 // Reset entire TCCR1B to 0
  TCCR1B |= B00000111;        // Set CS20, CS21 and CS22 to 1 so we get prescalar 1024
  TIMSK1 |= B00000010;        // Set OCIE1A to 1 so we enable compare match A
  OCR1A = 1953;               // (1s * 2MHz) / 1024 = 15625
}

// 8-Bit Timer
void setupTimer2() {
  // interrupt time = 1/(2Mhz/1024) * 255 =  10.24ms;
  TCCR2A = 0;                 // Reset entire TCCR1A to 0
  TCCR2B = 0;                 // Reset entire TCCR1B to 0
  TCCR2B |= B00000111;        // Set CS20, CS21 and CS22 to 1 so we get prescalar 1024
  TIMSK2 |= B00000100;        // Set OCIE1B to 1 so we enable compare match B
  OCR2B = 1;                // Finally we set compare register B to this value
}

void updateTimer2Interval(unsigned long interval_ms) {
  unsigned long ocr2b_value = ((interval_ms * 2000000UL) / 1024) - 1;
  ocr2b_value = ocr2b_value / 1000;

  if (ocr2b_value > 65535) {
    ocr2b_value = 65535;
  }

  TIMSK1 &= ~B00000010;
  OCR1A = ocr2b_value;
  TIMSK1 |= B00000010;
}

void setup() {

  cli(); // noInterrupts Makro

  CLKPR = 0x80; // Enable Clock Prescaler Change by setting CLKPCE bit to 1 and every other to 0
  CLKPR = 0x03; // Set Clock Prescaler to Clock Division Factor 8 (16MHz / 8 = 2MHz)
  
  pinMode(LEDblau_1, OUTPUT);
  pinMode(LEDgruen_1, OUTPUT);
  pinMode(LEDrot_1, OUTPUT);

  pinMode(LEDblau_2, OUTPUT);
  pinMode(LEDgruen_2, OUTPUT);
  pinMode(LEDrot_2, OUTPUT);

  pinMode(BTN_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_pin), button_ISR, FALLING);

  setupTimer1();
  //setupTimer2();

  Serial.begin(76800);

  disable_Timer1_ISR();
  
  sei(); // interrupts Makro
}

// ---------------------------------------- Main Routine

void loop() {

  while (counter == 0) {
    RGB_fade();
  }

  while (counter == 1) {
    RGB_blink();
  }

}

// ---------------------------------------- Button ISR

void button_ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > 200) {
    counter++;
    counter = counter % 5;
    Serial.print("Setting LED to: ");
    Serial.print(counter);
    Serial.print("\n");
  }
  lastInterruptTime = interruptTime;

  if (counter == 1) {
    enable_Timer1_ISR();
  } else if (counter == 2) {
    disable_Timer1_ISR();
    set_color(red);
  } else if (counter == 3) {
    set_color(green);
  } else if (counter == 4) {
    set_color(blue);
  }  
}

// ---------------------------------------- Timer ISR

//IRS Timer 1 trigger each 1s.
ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt

  blink_state += 1;
  blink_state = blink_state % 3;
}

// IRS Timer 2 trigger each ~10ms.
ISR(TIMER2_COMPB_vect){   
  
}

// ---------------------------------------- RGB LED Routines

void RGB_fade(){
  for (; r>=0, b<255; b++, r--) {
    analogWrite(LEDrot_1, r);
    analogWrite(LEDblau_1, b);
    
    analogWrite(LEDblau_2, r);
    analogWrite(LEDgruen_2, b);
    delay(10);
  }

  for (; b>=0, g<255; g++, b--) {
    analogWrite(LEDblau_1, b);
    analogWrite(LEDgruen_1, g);
    
    analogWrite(LEDgruen_2, b);
    analogWrite(LEDrot_2, g);
    delay(10);
  }

  for (; g>=0, r<255; r++, g--) {
    analogWrite(LEDrot_1, r);
    analogWrite(LEDgruen_1, g);
        
    analogWrite(LEDrot_2, r);
    analogWrite(LEDblau_2, g);
    delay(10);
  } 
}

void RGB_blink() {
  if (blink_state == 0) {
    digitalWrite(LEDrot_1, 1);
    digitalWrite(LEDgruen_1, 0);
    digitalWrite(LEDblau_1, 0);

    digitalWrite(LEDrot_2, 0);
    digitalWrite(LEDgruen_2, 1);
    digitalWrite(LEDblau_2, 0);
  } else if (blink_state == 1) {
    digitalWrite(LEDrot_1, 0);
    digitalWrite(LEDgruen_1, 1);
    digitalWrite(LEDblau_1, 0);

    digitalWrite(LEDrot_2, 0);
    digitalWrite(LEDgruen_2, 0);
    digitalWrite(LEDblau_2, 1);
  } else if (blink_state == 2) {
    digitalWrite(LEDrot_1, 0);
    digitalWrite(LEDgruen_1, 0);
    digitalWrite(LEDblau_1, 1);

    digitalWrite(LEDrot_2, 1);
    digitalWrite(LEDgruen_2, 0);
    digitalWrite(LEDblau_2, 0);
  }
}
    
void set_color(int idx){  
  Serial.print("Changed color");
  Serial.print("\n");
  for (int i = 0; i <= 2; i++){
    if (i == idx){
      digitalWrite(rgb_pins[idx], 1);
      digitalWrite(rgb_pins[idx + 3], 1);
    } else {
      digitalWrite(rgb_pins[i], 0);
      digitalWrite(rgb_pins[i + 3], 0);
    }
  }
}

void reset_LEDs(){
    digitalWrite(LEDrot_1, 0);
    digitalWrite(LEDgruen_1, 0);
    digitalWrite(LEDblau_1, 1);

    digitalWrite(LEDrot_2, 1);
    digitalWrite(LEDgruen_2, 0);
    digitalWrite(LEDblau_2, 0);
    Serial.print("RGB LEDs Resetted!\n");
}

void disable_Timer1_ISR() {
    TCCR1B = B00000000;
    //TCCR2B = B00000000;
    Serial.print("Timer 1 disabled!\n");
}

void enable_Timer1_ISR() {
    TCCR1B = B00000111;
    //TCCR2B = B00000111;
    Serial.print("Timer 1 enabled!\n");
}