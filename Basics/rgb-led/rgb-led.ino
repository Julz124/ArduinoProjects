int LEDrot = 5;
int LEDgruen = 6;
int LEDblau = 3;

int Pause = 1000;      // Pause mit 1000ms also 1 Sekunde
int Helligkeit = 255;  // Zahlenwert zwischen 0 und 255 – gibt die Leuchtstärke der einzelnen Farbe an

void setup() 
{
  pinMode(LEDblau, OUTPUT);
  pinMode(LEDgruen, OUTPUT);
  pinMode(LEDrot, OUTPUT);
}

void loop() 
{
  analogWrite(LEDrot, Helligkeit);   // rot einschalten
  delay(Pause);                      // Pause
  analogWrite(LEDrot, LOW);          // rot ausschalten
  analogWrite(LEDgruen, Helligkeit); // gruen einschalten
  delay(Pause);                      // Pause
  analogWrite(LEDgruen, LOW);        // gruen ausschalten
  analogWrite(LEDblau, Helligkeit);  // blau einschalten
  delay(Pause);                      // Pause
  analogWrite(LEDblau, LOW);         // blau ausschalten
}
