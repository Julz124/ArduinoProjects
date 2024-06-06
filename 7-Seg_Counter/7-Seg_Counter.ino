char eins[] = {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW};
char zwei[] = {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH, LOW};
char drei[] = {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH, LOW};
char vier[] = {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH, LOW};
char fuenf[] = {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW};
char sechs[] = {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
char sieben[] = {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW};
char acht[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW};
char neun[] = {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH, LOW};
char null[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW, LOW};

char* zahlen[] = {eins, zwei, drei, vier, fuenf, sechs, sieben, acht, neun, null};

char m_idx = 0;
char* zahl;

void setup() {
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
}

void loop() {
  
  zahl = zahlen[m_idx];
  
  for (int i = 2; i <= 8; i++){
    digitalWrite(i, *zahl);
    zahl++;
  }

  delay(1000);

  m_idx += 1;
  m_idx = m_idx % (sizeof(zahlen) / sizeof(zahlen[0]));

}
