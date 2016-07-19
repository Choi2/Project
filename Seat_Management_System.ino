//책상 위
const int buttonPin = 2;
const int ledPin = 3;
int ledState = HIGH;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;
int trigPin1 = 9, trigPin2 = 11, trigPin3 = 6, trigPin4 = 5;
int echoPin1 = 8, echoPin2 = 12, echoPin3 = 7, echoPin4 = 4;

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);
  pinMode(echoPin4, INPUT);
}

void loop()
{
  int i, a, b, person;
  long duration1, duration2, duration3, duration4, distance1, distance2, distance3, distance4, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, avr1, avr2, avr3, avr4, start = 0, check1, check2, check3, check4;
  long temp1[10] = {0}, temp2[10] = {0}, temp3[10] = {0}, temp4[10] = {0};
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }
  digitalWrite(ledPin, ledState);
  lastButtonState = reading;
  for (i = 0; i < 10; i++) {
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2000);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2000);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    // 밑에 2개더 추가분 소스
    digitalWrite(trigPin3, LOW);
    delayMicroseconds(2000);
    digitalWrite(trigPin3, HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigPin3, LOW);
    duration3 = pulseIn(echoPin3, HIGH);
    digitalWrite(trigPin4, LOW);
    delayMicroseconds(2000);
    digitalWrite(trigPin4, HIGH);
    delayMicroseconds(1000);
    digitalWrite(trigPin4, LOW);
    duration4 = pulseIn(echoPin4, HIGH);
    
    distance1 = duration1 * 17 / 1000;
    distance2 = duration2 * 17 / 1000;
    distance3 = duration3 * 17 / 1000;
    distance4 = duration4 * 17 / 1000;
    
    temp1[i] = {distance1};
    temp2[i] = {distance2};
    temp3[i] = {distance3};
    temp4[i] = {distance4};
    
    //Serial.println(distance1);
    //Serial.println(distance2);
    
    delay(1);
    if (temp1[i] >= 1000) {
      i = -1;
      for (a = 0; a < 10; a++)
        temp1[a] = {0};
    }
    delay(1);
    if (temp2[i] >= 1000) {
      i = -1;
      for (a = 0; a < 10; a++)
        temp2[a] = {0};
    }
    delay(1);
    if (temp3[i] >= 1000) {
      i = -1;
      for (a = 0; a < 10; a++)
        temp3[a] = {0};
    }
    delay(1);
    if (temp4[i] >= 1000) {
      i = -1;
      for (a = 0; a < 10; a++)
        temp4[a] = {0};
    }
    delay(1);
    //Serial.println(i);
  }
  for (i = 0; i < 10; i++) {
    sum1 = sum1 + temp1[i];
    sum2 = sum2 + temp2[i];
    sum3 = sum3 + temp3[i];
    sum4 = sum4 + temp4[i];
  }
  //Serial.println(sum);
  avr1 = (sum1 / 10);
  avr2 = (sum2 / 10);
  avr3 = (sum3 / 10);
  avr4 = (sum4 / 10);
  Serial.print(avr1);
  Serial.print(" ");
  Serial.print(avr2);
  Serial.print(" ");
  Serial.print(avr3);
  Serial.print(" ");
  Serial.println(avr4);
  if (ledState == HIGH) 
  start = avr1;//초기값은 하나로 측정해서 변수하나에 넣어도 충분(일렬로 나열되어있기때문에)
  else if (ledState == LOW) {
    check1 = avr1;
    check2 = avr2;
    check3 = avr3;
    check4 = avr4;
    if (check1 <= (start - 10) || check2 <= (start - 10) || check3 <= (start - 10) || check4 <= (start - 10) ) {
      person = 1; //사람 유
    }
    else {
      person = 0; //사람 무
    }
  }
}
