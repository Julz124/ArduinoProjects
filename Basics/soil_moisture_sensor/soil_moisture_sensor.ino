// Analog

int Messwert = 0;

void setup(){
  Serial.begin(9600);
}

void loop() {
  Messwert = analogRead(A0);

  Serial.print("Feuchtigkeits-Messwert:");
  Serial.println(Messwert);

  delay(1000);
}

/*
// Digital

int sensorPin = 7;
int sensorValue = 0;

void setup() {
  pinMode(sensorPin, INPUT); // Setzt den Pin als Eingang
  Serial.begin(9600); // Startet die serielle Kommunikation
}

void loop() {
  sensorValue = digitalRead(sensorPin); // Liest den Wert vom Sensor
  
  if (sensorValue == HIGH) {
    Serial.println("Trocken");
  } else {
    Serial.println("Feucht");
  }
  
  delay(1000); // Warte 1 Sekunde vor dem nächsten Auslesen
}
*/
