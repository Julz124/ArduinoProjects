int buzzerPin = 8;
int buttonPin = 2;
int buttonState;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    tone(buzzerPin, 440);
  } else {
    noTone(buzzerPin);
  }

}
