#include <string.h>
#include <EEPROM.h>
///////////////////////////////////////////
#include <SoftwareSerial.h>

#define SSID "SEE_2"  //공유기 SSID
#define PASS "see05524"   //공유기 비번
#define DST_IP "175.126.112.111"   //MYSQL 서버 주소 
SoftwareSerial dbgSerial(3, 2); // RX, TX 3번,2번핀
///////////////////////////////////////////

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
#define limit 300//센싱 한계 표시(cm)
#define trigoffdelay 20000//trig off시간
#define trigondelay 10000//tring on 시간
#define count 2 // the number of sensers  
#define lotation 10//정확도 및 평균을 위한 센싱횟수
int reading_start;
int reading_chair;
int debug = 0;//debug전용
int laststart = LOW;
int lastchair = LOW;
int lastDebounceTime[2] = {0, 0};
int cnt;
int person;
long start[2], check[count];
long start_nonvolatile[2];

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
  //시리얼 포트 초기화
  Serial.begin(9600);

  /////////////////////////////////////////////////////////////////////////
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
  /*
    if (!connected) {
      while (1);
    }
  */
  delay(5000);

  dbgSerial.println("AT+CIPMUX=0");

  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
}


void print_result()
{
  long duration[count] = {0}, distance[count] = {0}, sum[count] = {0}, avr[count] = {0};
  //// the looping for average and filtering for strange value.

  for (int k = 0; k < lotation; k++) {
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
    /*
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
  //Serial.println(sum[0]);

  for (int a = 0; a < count; a++) //Average
    avr[a] = (sum[a] / lotation);

  Serial.print("avr[0] = ");
  Serial.println(avr[0]);
  Serial.print("avr[1] = ");
  Serial.println(avr[1]);

  eeprom();

  reading_start = digitalRead(startPin);
  reading_chair = digitalRead(chairPin);
  ///////초기화MODE_책상과_의자사이_거리//////////////////////
  if (reading_start == HIGH && reading_chair == HIGH) {
    start[0] = avr[0]; //의자에 대한 초기값
    debug = 1;
    delay(300);
    Serial.print("chair_start[0] = ");
    Serial.println(start[0]);
  }
  /////////////초기화MODE_책상과_바닥사이_거리////////////////
  else if ((reading_start == HIGH && reading_chair == LOW)) {
    start[1] = avr[0]; //바닥에 대한 초기값
    debug = 2;
    Serial.print("floor_start[1] = ");
    Serial.println(start[1]);
    delay(300);
  }
  ////////////초기화종료MODE//////////////////////////
  else if (reading_start == LOW && reading_chair == LOW) {
    for (int a = 0; a < count; a++) {
      check[a] = avr[a];
    }
    debug = 3;
    Serial.print("check[0] = ");
    Serial.println(check[0]);
    delay(300);
  }
  /*센서 4개 설치시 주석지울것!
    /////////////비교값이 초기값 범위(오차포함)안에 있을경우 사람이 있는걸로 판별/////////////////의자와 바닥사이, 의자와 책상사이//////////
    if ((check[0] >= (start[0] + error) && check[0] <= (start[1] - error)) || (check[1] >= (start[0] + error) && check[1] <= (start[1] - error))) {
      person = 1; //사람 유
      Serial.print("person = ");
      Serial.println(person);
    }
    else if ((check[2] >= (start[0] + error) && check[2] <= (start[1] - error)) || (check[3] >= (start[0] + error) && check[3] <= (start[1] - error))) {
      person = 1;
      Serial.print("person = ");
      Serial.println(person);
    }
    else if (check[0] <= (start[0] - error) || check[1] <= (start[0] - error) || check[2] <= (start[0] - error) || check[3] <= (start[0] - error)) {
      person = 1;
      Serial.print("person = ");
      Serial.println(person);
    }
    else {
      person = 0; //사람 무
      Serial.print("person = ");
      Serial.println(person);
    }*/
  for (int a = 0; a < count; a++) {
    if ((check[a] >= (start[0] + error) && check[a] <= (start[1] - error))) {
      person = 1; //사람 유
      break;
    }
    else if (check[a] <= (start[0] - error)) {
      person = 1;
      break;
    }
    else {
      person = 0; //사람 무
    }
  }
  Serial.print("person = ");
  Serial.println(person);

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
  //dbgSerial.println("AT+CWMODE=1");

  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"";
  dbgSerial.println(cmd);
  Serial.println(cmd);
  delay(3000);

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

void eeprom() {
  if (cnt == 0) {
    start[0] = EEPROM.read(0) * 10;
    start[1] = EEPROM.read(1) * 10;
    start_nonvolatile[0] = start[0];
    start_nonvolatile[1] = start[1];
  }
  if (start[0] != start_nonvolatile[0])
    EEPROM.write(0, start[0] / 10);
  if (start[1] != start_nonvolatile[1])
    EEPROM.write(1, start[1] / 10);
  cnt++;
  if (cnt == 4)
    cnt = 1;
  delay(1);
}
