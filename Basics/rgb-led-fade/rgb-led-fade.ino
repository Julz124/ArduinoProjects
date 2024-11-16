#include <avr/sleep.h>
#include <LowPower.h>

// ---------------------------------------- Events

volatile bool fade_evt = false;
volatile bool btn_evt = false;
volatile bool sleep_evt = false;

// ---------------------------------------- Pinlayout

int LEDrot_1 = 3;
int LEDgruen_1 = 5;
int LEDblau_1 = 6;

int LEDrot_2 = 9;
int LEDgruen_2 = 10;
int LEDblau_2 = 11;

int BTN_pin = 2;

// ---------------------------------------- RGB fade variables

volatile int r1 = 255, g1 = 0, b1 = 0;
volatile int r2 = 0, g2 = 0, b2 = 255;


// ---------------------------------------- RGB LED Routine Counter

int btn_counter = 0;

// ---------------------------------------- Sleep Timer Variables

unsigned long startTime = 0;
unsigned long interval = 0;
bool timerActive = false;

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
  OCR2B = 255;                // Finally we set compare register B to this value
}

void updateTimer2Interval(unsigned long interval_ms) {
  unsigned long ocr2b_value = ((interval_ms * 2000000UL) / 1024) - 1;
  ocr2b_value = ocr2b_value / 1000;

  if (ocr2b_value > 65535) {
    ocr2b_value = 65535;
  }

  TIMSK1 &= ~B00000010;
  OCR2B = ocr2b_value;
  TIMSK1 |= B00000010;
}

void setup() {

  cli(); // noInterrupts Makro

  //CLKPR = 0x80; // Enable Clock Prescaler Change by setting CLKPCE bit to 1 and every other to 0
  //CLKPR = 0x03; // Set Clock Prescaler to Clock Division Factor 8 (16MHz / 8 = 2MHz)
  
  pinMode(LEDblau_1, OUTPUT);
  pinMode(LEDgruen_1, OUTPUT);
  pinMode(LEDrot_1, OUTPUT);

  pinMode(LEDblau_2, OUTPUT);
  pinMode(LEDgruen_2, OUTPUT);
  pinMode(LEDrot_2, OUTPUT);

  pinMode(BTN_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_pin), button_ISR, FALLING);

  //setupTimer1();
  //setupTimer2();

  //Serial.begin(76800);
  Serial.begin(9600);

  //disable_Timer_ISR();
  
  sei(); // interrupts Makro
}

// ---------------------------------------- Main Routine

void loop() {

  if (timerActive) {
    if (millis() - startTime >= interval) {
      sleep_evt = true;
      timerActive = false;
      Serial.println("Timer elapsed, sleep_evt set to 1");
    }
  }

  if (sleep_evt == false) {
    increase_fade();
  } else {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  if (fade_evt) { 
    fade_evt = false; // Reset the flag

    // Update LEDs
    analogWrite(LEDrot_1, r1);
    analogWrite(LEDrot_2, r2);

    analogWrite(LEDgruen_1, g1);
    analogWrite(LEDgruen_2, g2);

    analogWrite(LEDblau_1, b1);
    analogWrite(LEDblau_2, b2);
  }
  
  if (btn_evt) {
    btn_evt = false;
    Serial.println("Button pressed");
    setLEDColor(btn_counter);
    delay(1000);

    switch (btn_counter) {
      case 1:
        startSleepTimer(30);
        break;
            
      case 2:
        startSleepTimer(60);
        break;
            
      case 3:
        startSleepTimer(120);
        break;

      case 4:
        startSleepTimer(0);
        break;
            
      default:
        stopSleepTimer();
        break;
    }
  }
}

// ---------------------------------------- Button ISR

void button_ISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  if ((interruptTime - lastInterruptTime) > 100) {
    btn_counter++;
    btn_counter = btn_counter % 5;
    btn_evt = true;
  }

  lastInterruptTime = interruptTime;
}

// ---------------------------------------- Timer ISR

//IRS Timer 1 trigger each 1s.
ISR(TIMER1_COMPA_vect){
  TCNT1  = 0;                  //First, set the timer back to 0 so it resets for next interrupt

}

// IRS Timer 2 trigger each ~10ms.
ISR(TIMER2_COMPB_vect){   
  static int phase = 0; // Track the phase of the fade
  phase = (phase + 1) % 765; // 765 = 3 * 255 (cycle length for all fades)

  // LED 1 (normal phase)
  if (phase < 255) { 
    r1 = 255 - phase; g1 = phase; b1 = 0; 
  } else if (phase < 510) { 
    r1 = 0; g1 = 510 - phase; b1 = phase - 255; 
  } else { 
    r1 = phase - 510; g1 = 0; b1 = 765 - phase; 
  }

  // LED 2 (offset phase)
  int offsetPhase = (phase + 255) % 765; // Offset by 255
  if (offsetPhase < 255) { 
    r2 = 255 - offsetPhase; g2 = offsetPhase; b2 = 0; 
  } else if (offsetPhase < 510) { 
    r2 = 0; g2 = 510 - offsetPhase; b2 = offsetPhase - 255; 
  } else { 
    r2 = offsetPhase - 510; g2 = 0; b2 = 765 - offsetPhase; 
  }

  fade_evt = true; // Signal that the fade is complete
}

void disable_Timer_ISR() {
    //TCCR1B = B00000000;
    TCCR2B = B00000000;
    Serial.print("Timer disabled!\n");
}

void enable_Timer_ISR() {
    //TCCR1B = B00000111;
    TCCR2B = B00000111;
    Serial.print("Timer enabled!\n");
}

// ---------------------------------------- RGB LED Routines

void increase_fade()  {
  static int phase = 0; // Track the phase of the fade
  phase = (phase + 1) % 765; // 765 = 3 * 255 (cycle length for all fades)

  // LED 1 (normal phase)
  if (phase < 255) { 
    r1 = 255 - phase; g1 = phase; b1 = 0; 
  } else if (phase < 510) { 
    r1 = 0; g1 = 510 - phase; b1 = phase - 255; 
  } else { 
    r1 = phase - 510; g1 = 0; b1 = 765 - phase; 
  }

  // LED 2 (offset phase)
  int offsetPhase = (phase + 255) % 765; // Offset by 255
  if (offsetPhase < 255) { 
    r2 = 255 - offsetPhase; g2 = offsetPhase; b2 = 0; 
  } else if (offsetPhase < 510) { 
    r2 = 0; g2 = 510 - offsetPhase; b2 = offsetPhase - 255; 
  } else { 
    r2 = offsetPhase - 510; g2 = 0; b2 = 765 - offsetPhase; 
  }

  delay(10);
  fade_evt = true; // Signal that the fade is complete
}

void setLEDColor(int color) {
  switch (color) {
    case 1: // Green
      analogWrite(LEDrot_1, 0); analogWrite(LEDgruen_1, 255); analogWrite(LEDblau_1, 0);
      analogWrite(LEDrot_2, 0); analogWrite(LEDgruen_2, 255); analogWrite(LEDblau_2, 0);
      break;

    case 2: // Blue
      analogWrite(LEDrot_1, 0); analogWrite(LEDgruen_1, 0); analogWrite(LEDblau_1, 255);
      analogWrite(LEDrot_2, 0); analogWrite(LEDgruen_2, 0); analogWrite(LEDblau_2, 255);
      break;

    case 3: // Red
      analogWrite(LEDrot_1, 255); analogWrite(LEDgruen_1, 0); analogWrite(LEDblau_1, 0);
      analogWrite(LEDrot_2, 255); analogWrite(LEDgruen_2, 0); analogWrite(LEDblau_2, 0);
      break;
    default: // Turn off LEDs
      analogWrite(LEDrot_1, 0); analogWrite(LEDgruen_1, 0); analogWrite(LEDblau_1, 0);
      analogWrite(LEDrot_2, 0); analogWrite(LEDgruen_2, 0); analogWrite(LEDblau_2, 0);
      break;
  }
}

// ---------------------------------------- Sleep Timer

void startSleepTimer(int minutes) {
  Serial.println("SleepTimer set to: " +  String(minutes) + " minutes");
  interval = minutes * 60 * 1000UL;
  startTime = millis();
  timerActive = true;
  sleep_evt = false;
}

void stopSleepTimer() {
  Serial.println("SleepTimer deactivated");
  interval = 0;
  startTime = 0;
  timerActive = false;
  sleep_evt = false;
}
