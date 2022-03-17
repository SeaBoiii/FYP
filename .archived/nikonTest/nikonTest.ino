void setup() {
pinMode(3, OUTPUT);
//pinMode(4, OUTPUT);
}

void loop() {
  Serial.begin(9600);
  Serial.print("3: ");
  Serial.println(digitalRead(3));
  //Serial.print("4: ");
  //Serial.println(digitalRead(4));
  nikonexp();
  delay(5000);
}

void nikonexp() {
  digitalWrite(3, LOW); // close prime/focus (half pressed shutter)*-8
  //digitalWrite(4, LOW); // close shutter (shutter fully presed)
  delay(20);
  digitalWrite(3, HIGH); //return to open position
  //digitalWrite(4, HIGH); //return to open position
  delay(20);
}
