//책상 아래
#define trigPin1 7
#define trigPin2 9
#define trigPin3 11
#define trigPin4 13
#define echoPin1 6
#define echoPin2 8
#define echoPin3 10
#define echoPin4 12
#define buttonPin1 2
#define buttonPin2 4
#define ledPin1 3
#define ledPin2 5
#define error 10//허용오차범위(cm)
#define limit 1000//센싱 한계 표시(cm)
#define trigoffdelay 20000//trig off시간
#define trigondelay 10000//tring on 시간
int ledState1 = LOW;//LED초기상태 : ON
int ledState2 = LOW;//LED초기상태 : ON
int buttonState1, buttonState2;
int lastButtonState1 = LOW;
int lastButtonState2 = LOW;
long lastDebounceTime[2] = {0, 0};
long debounceDelay[2] = {50, 50};

void setup()
{
  //Serial.begin(9600);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, ledState1);
  digitalWrite(ledPin2, ledState2);
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
  int person;//사람 유무 판별 변수
  long start[2] = {0}, check[4] = {0};//초기값(기준값) 변수와 비교값 변수
  long duration[4] = {0}, distance[4] = {0}, sum[4] = {0}, avr[4] = {0};
  ///////////초음파 센서를 통한 거리측정///////////
  for (int i = 0; i < 10; i++) {
    delay(1);
    void change_internal();
    delay(1);
    void change_external();
    delay(1);
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin1, LOW);
    duration[0] = pulseIn(echoPin1, HIGH);
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin2, LOW);
    duration[1] = pulseIn(echoPin2, HIGH);
    // 밑에 2개더 추가분 소스
    digitalWrite(trigPin3, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin3, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin3, LOW);
    duration[2] = pulseIn(echoPin3, HIGH);
    digitalWrite(trigPin4, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin4, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin4, LOW);
    duration[3] = pulseIn(echoPin4, HIGH);

    for (int a = 0; a < 4; a++) {
      distance[a] = duration[a] * 17 / 1000;
    }

    //Serial.println(distance[0]);
    /////////////센싱된 값들의 안정화_ 불안정한 값이 입력될경우 초기화///////////
    delay(1);

    for (int a = 0; a < 4; a++) {
      if (distance[a] >= limit) {
        i = -1;
        for (int b = 0; b < 4; b++) {
          sum[b] = 0;
        }
        continue;
      }
    }

    delay(1);

    for (int a = 0; a < 4; a++) {
      sum[a] = sum[a] + distance[a];
    }
    //Serial.println(i);
  }
  //Serial.println(sum[0]);
  ///////////////각 센서에서 센싱한 값들의 평균//////////
  for (int a = 0; a < 4; a++) {
    avr[a] = (sum[a] / 10);
  }
  /////////////LED가 켜져있을 경우_평균이 초기값 변수에 입력////////////////
  if (ledState1 == LOW) {
    if (ledState2 == LOW) {
      start[0] = avr[0] //의자에 대한 초기값
    }
    else if (ledState2 == HIGH) {
      start[1] = avr[0];//바닥에 대한 초기값
    }
  }
  ///////////LED가 꺼져있을 경우_평균이 비교값 변수에 입력////////////////
  else if (ledState1 == HIGH) {
    for (int a = 0; a < 4; a++) {
      check[a] = avr[a];
    }
    /////////////비교값이 초기값 범위(오차포함)안에 있을경우 사람이 있는걸로 판별/////////////////의자와 바닥사이, 의자와 책상사이//////////
    if ((check[0] >= (start[0] + error) && check[0] <= (start[1] - error)) || (check[1] >= (start[0] + error) && check[1] <= (start[1] - error))) {
      person = 1; //사람 유
    }
    else if ((check[2] >= (start[0] + error) && check[2] <= (start[1] - error)) || (check[3] >= (start[0] + error) && check[3] <= (start[1] - error))) {
      person = 1;
    }
    else if (check[0] <= (start[0] - error) || check[1] <= (start[0] - error) || check[2] <= (start[0] - error) || check[3] <= (start[0] - error)) {
      person = 1;
    }
    else {
      person = 0; //사람 무
    }
  }
}

void change_internal() {
  //////////////버튼 스위치의 토글스위치화//////////
  int reading1 = digitalRead(buttonPin1);
  if (reading1 != lastButtonState1) {
    lastDebounceTime[0] = millis();
  }
  if ((millis() - lastDebounceTime[0]) > debounceDelay[0]) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;
      if (buttonState1 == HIGH) {
        ledState1 = !ledState1;
      }
    }
  }
  digitalWrite(ledPin1, ledState1);
  lastButtonState1 = reading1;
}

void change_external() {
  //////////////버튼 스위치의 토글스위치화//////////
  int reading2 = digitalRead(buttonPin2);
  if (reading2 != lastButtonState2) {
    lastDebounceTime[1] = millis();
  }
  if ((millis() - lastDebounceTime[1]) > debounceDelay[1]) {
    if (reading2 != buttonState2) {
      buttonState2 = reading2;
      if (buttonState2 == HIGH) {
        ledState2 = !ledState2;
      }
    }
  }
  digitalWrite(ledPin2, ledState2);
  lastButtonState2 = reading2;
}
