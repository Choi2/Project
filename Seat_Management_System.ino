//책상 위
const int buttonPin = 2;
const int ledPin = 3;
int ledState = HIGH;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;
int trigPin1 = 9, trigPin2 = 11;
int echoPin1 = 8, echoPin2 = 12;

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
}

void loop()
{
  int i, a, b, person;
  long duration1, duration2, distance1, distance2, sum1 = 0, sum2 = 0, avr1, avr2, start = 0, check1, check2;
  long temp1[10] = {0}, temp2[10] = {0};
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
    delayMicroseconds(20000);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10000);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(20000);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10000);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    distance1 = duration1 * 17 / 1000;
    distance2 = duration2 * 17 / 1000;
    temp1[i] = {distance1};
    temp2[i] = {distance2};
    
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
    //Serial.println(i);
  }
  for (i = 0; i < 10; i++) {
    sum1 = sum1 + temp1[i];
    sum2 = sum2 + temp2[i];
  }
  //Serial.println(sum);
  avr1 = (sum1 / 10);
  avr2 = (sum2 / 10);
  Serial.println(avr1);
  Serial.println(avr2);
  if (ledState == HIGH) 
  start = avr1;//초기값은 하나로 측정해서 변수하나에 넣어도 충분(일렬로 나열되어있기때문에)
  else if (ledState == LOW) {
    check1 = avr1;
    check2 = avr2;
    if (check1 <= (start - 10) || check2 <= (start - 10) ) {
      person = 1; //사람 유
    }
    else {
      person = 0; //사람 무
    }
  }
}
