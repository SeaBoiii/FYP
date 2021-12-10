int set = A0;
int right = A1;
int left = A2;
int down = A3;
int up = A4;
int setbtn = A5;

int sval = 0;
int rval = 0;
int lval = 0;
int dval = 0;
int uval = 0;
int val = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  sval = analogRead(set);
  Serial.print("Set: ");
  Serial.println(sval);   // 0 = Pressed
  rval = analogRead(right);
  Serial.print("Right: ");
  Serial.println(rval);
  lval = analogRead(left);
  Serial.print("Left: ");
  Serial.println(lval);
  dval = analogRead(down);
  Serial.print("Down: ");
  Serial.println(dval);
  uval = analogRead(up);
  Serial.print("Up: ");
  Serial.println(uval);
  val = analogRead(setbtn);
  Serial.print("setbtn: ");
  Serial.println(setbtn);
  delay(500);
}
