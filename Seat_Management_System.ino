#include <string.h>
#include <EEPROM.h>
///////////////////////////////////////////
#include <SoftwareSerial.h>

#define SSID "SEE_2"  //공유기 SSID
#define PASS "see05524"   //공유기 비번
#define DST_IP "175.126.112.111"   //MYSQL 서버 주소 
SoftwareSerial dbgSerial(5, 4); // RX, TX 5번,4번핀
///////////////////////////////////////////
#define trigPin1 7
#define trigPin2 9
#define trigPin3 11
#define trigPin4 13
#define echoPin1 6
#define echoPin2 8
#define echoPin3 10
#define echoPin4 12
#define buttonPin 2
#define ledPin 3
#define count 1 // the number of sensers  
#define trigoffdelay 2000000
#define trigondelay 1000000
#define error 10
#define limit 300
#define rotation 10 //정확도 및 평균을 위한 센싱 횟수
volatile int ledState = HIGH;//LED초기상태 : ON
int person;//사람 유무 판별 변수
int cnt;//for EEPROM
long start_nonvolatile;//for EEPROM
long start, check[count];//초기값(기준값) 변수와 비교값 변수

char * floatToString(char * outstr, double val, byte precision, byte widthp) {
  char temp[16]; //increase this if you need more digits than 15
  byte i;

  temp[0] = '\0';
  outstr[0] = '\0';

  if (val < 0.0) {
    strcpy(outstr, "-\0"); //print "-" sign
    val *= -1;
  }

  if ( precision == 0) {
    strcat(outstr, ltoa(round(val), temp, 10)); //prints the int part
  }
  else {
    unsigned long frac, mult = 1;
    byte padding = precision - 1;

    while (precision--)
      mult *= 10;

    val += 0.5 / (float)mult;    // compute rounding factor

    strcat(outstr, ltoa(floor(val), temp, 10)); //prints the integer part without rounding
    strcat(outstr, ".\0"); // print the decimal point

    frac = (val - floor(val)) * mult;

    unsigned long frac1 = frac;

    while (frac1 /= 10)
      padding--;

    while (padding--)
      strcat(outstr, "0\0");   // print padding zeros

    strcat(outstr, ltoa(frac, temp, 10)); // print fraction part
  }

  // generate width space padding
  if ((widthp != 0) && (widthp >= strlen(outstr))) {
    byte J = 0;
    J = widthp - strlen(outstr);

    for (i = 0; i < J; i++) {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp, outstr);
    strcpy(outstr, temp);
  }
  return outstr;
}

void setup(void)
{
  Serial.begin(9600);
  Serial.setTimeout(5000);
  dbgSerial.begin(9600);
  Serial.println("ESP8266 connect");

  boolean connected = false;
  for (int i = 0; i < 10; i++)
  {
    if (connectWiFi())
    {
      connected = true;
      break;
    }
  }
  /*if (!connected) {
    while (1);
    }*/
  delay(1000);
  dbgSerial.println("AT+CIPMUX=0");

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  /*
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);
    pinMode(trigPin4, OUTPUT);
    pinMode(echoPin4, INPUT);
  */
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);               //출력으로 쓸것이라 정한다.
  digitalWrite(ledPin, ledState);
  attachInterrupt(0, change, FALLING);
}

void print_result() {
  long duration[count] = {0}, distance[count] = {0}, sum[count] = {0}, avr[count] = {0};
  long ch = 0;
  //// the looping for average and filtering for strange value.

  for (int k = 0; k < rotation; k++) {
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin1, LOW);
    duration[0] = pulseIn(echoPin1, HIGH);
    /*
      digitalWrite(trigPin2, LOW);
      delayMicroseconds(trigoffdelay);
      digitalWrite(trigPin2, HIGH);
      delayMicroseconds(trigondelay);
      digitalWrite(trigPin2, LOW);
      duration[1] = pulseIn(echoPin2, HIGH);
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
    */

    for (int a = 0; a < count; a++)
      distance[a] = duration[a] * 17 / 1000;

    delay(1);

    for (int a = 0; a < count; a++) {
      if (distance[a] >= limit) {
        k = -1;
        for (int b = 0; b < count; b++) {
          sum[b] = 0;
        }
        continue;
      }
    }

    delay(1);

    for (int a = 0; a < count; a++)
      sum[a] = sum[a] + distance[a];
  }

  for (int a = 0; a < count; a++) //Average
    avr[a] = (sum[a] / rotation);

  Serial.print("avr[0] = ");
  Serial.println(avr[0]);

  eeprom();

  /*LED가 켜져있을 경우_평균이 초기값 변수에 입력*/
  if (ledState == HIGH) {
    start = avr[0];//초기값은 하나로 측정해서 변수하나에 넣어도 충분(일렬로 나열되어있기때문에)
    Serial.print("start = ");
    Serial.println(start);
  }
  /*LED가 꺼져있을 경우_평균이 비교값 변수에 입력*/
  else if (ledState == LOW) {
    for (int a = 0; a < count; a++) {
      check[a] = avr[a];
    }
    Serial.print("check[0] = ");
    Serial.println(check[0]);
  }
  /* 센서가 4개 되었을때 주석을 제거 할 것
    /////////////비교값이 초기값 범위(오차포함)안에 있을경우 사람이 있는걸로 판별/////////////////
    if ((check[0] <= (start - error)) || (check[1] <= (start - error)) || (check[2] <= (start - error)) || (check[3] <= (start - error)) ) {
    person = 1; //사람 유
    Serial.print("person = ");
    Serial.println(person);
    }
    else {
    person = 0; //사람 무
    Serial.print("person = ");
    Serial.println(person);
    }*/
  for (int a = 0; a < count; a++) {
    if ((check[a] <= (start - error))) {
      person = 1; //사람 유
      break;
    }
    else {
      person = 0; //사람 무
    }
  }
  /*사람 유무판별 결과를 서버로 전송*/
  Serial.print("people exist?? : ");
  Serial.println(person);

  delay(1);
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += DST_IP;
  cmd += "\",80";
  Serial.println(cmd);
  dbgSerial.println(cmd);

  if (dbgSerial.find("Error"))
  {
    Serial.println( "TCP connect error" );
    return;
  }

  delay(1);
  char test[20];
  String temp(floatToString(test, person, 2, 0));

  cmd = "GET /index.php?table1=" + temp + "\n\r";
  dbgSerial.print("AT+CIPSEND=");
  dbgSerial.println(cmd.length());

  Serial.println(cmd);

  delay(1);
  if (dbgSerial.find(">"))
  {
    Serial.print(">");
  } else
  {
    dbgSerial.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    return;
  }

  dbgSerial.print(cmd);
  //Serial.find("+IPD");
  while (Serial.available())
  {
    char c = Serial.read();
    dbgSerial.write(c);
    if (c == '\r') dbgSerial.print('\n');
  }
  Serial.println("====");
}

void loop(void)
{
  print_result();
}

boolean connectWiFi()
{
  dbgSerial.println("AT+CWMODE=1");

  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  dbgSerial.println(cmd);
  Serial.println(cmd);
  delay(1000);

  if (dbgSerial.find("OK"))
  {
    Serial.println("OK, Connected to WiFi.");
    return true;
  }
  else
  {
    Serial.println("Can not connect to the WiFi.");
    return false;
  }
}

void change() {
  /*버튼 스위치의 토글스위치화*/
  delay(1);
  ledState = !ledState;
  delay(1);
  digitalWrite(ledPin, ledState);
}

void eeprom() {
  if (cnt == 0) {
    start = EEPROM.read(0) * 10;
    start_nonvolatile = start;
  }
  if (start != start_nonvolatile)
    EEPROM.write(0, start / 10);
  cnt++;
  if (cnt == 4)
    cnt = 1;
  delay(1);
}
