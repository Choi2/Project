//책상 위
#define trigPin1 9
#define trigPin2 11
#define trigPin3 6
#define trigPin4 5
#define echoPin1 8
#define echoPin2 12
#define echoPin3 7
#define echoPin4 4
#define buttonPin 2
#define ledPin 3
#define error 10//허용오차범위(cm)
#define limit 1000//센싱 한계 표시(cm)
#define trigoffdelay 20000//trig off시간
#define trigondelay 10000//tring on 시간
int ledState = LOW;//LED초기상태 : ON
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

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
  int a, b, i;
  int person;//사람 유무 판별 변수
  int start = 0, check1, check2, check3, check4;//초기값(기준값) 변수와 비교값 변수
  long duration1, duration2, duration3, duration4, distance1, distance2, distance3, distance4, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, avr1, avr2, avr3, avr4;
  int reading = digitalRead(buttonPin);
  //////////////버튼 스위치의 토글스위치화//////////
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
  ///////////초음파 센서를 통한 거리측정///////////
  for (i = 0; i < 10; i++) {
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH);
    // 밑에 2개더 추가분 소스
    digitalWrite(trigPin3, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin3, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin3, LOW);
    duration3 = pulseIn(echoPin3, HIGH);
    digitalWrite(trigPin4, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin4, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin4, LOW);
    duration4 = pulseIn(echoPin4, HIGH);

    distance1 = duration1 * 17 / 1000;
    distance2 = duration2 * 17 / 1000;
    distance3 = duration3 * 17 / 1000;
    distance4 = duration4 * 17 / 1000;

    //Serial.println(distance1);
    //Serial.println(distance2);
    /////////////센싱된 값들의 안정화_ 불안정한 값이 입력될경우 초기화///////////
    delay(1);
    if (distance1 >= limit) {
      i = -1;
      sum1 = 0;
      sum2 = 0;
      sum3 = 0;
      sum4 = 0;
      continue;
    }
    delay(1);
    if (distance2 >= limit) {
      i = -1;
      sum1 = 0;
      sum2 = 0;
      sum3 = 0;
      sum4 = 0;
      continue;
    }
    delay(1);
    if (distance3 >= limit) {
      i = -1;
      sum1 = 0;
      sum2 = 0;
      sum3 = 0;
      sum4 = 0;
      continue;
    }
    delay(1);
    if (distance4 >= limit) {
      i = -1;
      sum1 = 0;
      sum2 = 0;
      sum3 = 0;
      sum4 = 0;
      continue;
    }
    delay(1);

    sum1 = sum1 + distance1;
    sum2 = sum2 + distance2;
    sum3 = sum3 + distance3;
    sum4 = sum4 + distance4;
    //Serial.println(i);
  }
  //Serial.println(sum);
  ///////////////각 센서에서 센싱한 값들의 평균//////////
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
  /////////////LED가 켜져있을 경우_평균이 초기값 변수에 입력////////////////
  if (ledState == LOW)
    start = avr1;//초기값은 하나로 측정해서 변수하나에 넣어도 충분(일렬로 나열되어있기때문에)
  ///////////LED가 꺼져있을 경우_평균이 비교값 변수에 입력////////////////
  else if (ledState == HIGH) {
    check1 = avr1;
    check2 = avr2;
    check3 = avr3;
    check4 = avr4;
    /////////////비교값이 초기값 범위(오차포함)안에 있을경우 사람이 있는걸로 판별/////////////////
    if (check1 <= (start - error) || check2 <= (start - error) || check3 <= (start - error) || check4 <= (start - error) ) {
      person = 1; //사람 유
    }
    else {
      person = 0; //사람 무
    }
  }
}
