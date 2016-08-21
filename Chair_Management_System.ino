//책상 아래
#define trigPin1 7
#define trigPin2 9
#define trigPin3 11
#define trigPin4 13
#define echoPin1 6
#define echoPin2 8
#define echoPin3 10
#define echoPin4 12
#define startPin 4
#define chairPin 5
#define error 10//허용오차범위(cm)
#define limit 1000//센싱 한계 표시(cm)
#define trigoffdelay 20000//trig off시간
#define trigondelay 10000//tring on 시간
int reading_start;
int reading_chair;
int debug = 0;//debug전용
int laststart = LOW;
int lastchair = LOW;
int lastDebounceTime[2] = {0, 0};

void setup()
{
  Serial.begin(9600);
  pinMode(startPin, INPUT);
  pinMode(chairPin, INPUT);
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
    //Serial.println(distance[1]);
    //Serial.println(distance[2]);
    //Serial.println(distance[3]);
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

  Serial.print("avr[0] = ");
  Serial.println(avr[0]);
  Serial.println(avr[1]);
  Serial.println(avr[2]);
  Serial.println(avr[3]);
  
  reading_start = digitalRead(startPin);
  reading_chair = digitalRead(chairPin);
///////초기화MODE_책상과_의자사이_거리////////////////////// 
  if (reading_start == HIGH && reading_chair == HIGH) {
    start[0] = avr[0]; //의자에 대한 초기값
    debug = 1;
    Serial.print("MODE = ");
    Serial.println(debug);
    delay(300);
  }
  /////////////초기화MODE_책상과_바닥사이_거리////////////////
  else if ((reading_start == HIGH && reading_chair == LOW)) {
    start[1] = avr[0]; //바닥에 대한 초기값
    debug = 2;
    Serial.print("MODE = ");
    Serial.println(debug);
    delay(300);
  }
  ////////////초기화종료MODE//////////////////////////
  else if (reading_start == LOW && reading_chair == LOW) {
    for (int a = 0; a < 4; a++) {
      check[a] = avr[a];
    }
    debug = 3;
    Serial.print("MODE = ");
    Serial.println(debug);
    delay(300);
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
