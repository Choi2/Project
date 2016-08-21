#define buttonPin1 8
#define buttonPin2 11
#define ledPin1 9
#define ledPin2 12
int ledState1 = HIGH;//LED초기상태 : ON
int ledState2 = HIGH;//LED초기상태 : ON
int buttonState1, buttonState2;
int lastButtonState1 = LOW;
int lastButtonState2 = LOW;
long lastDebounceTime[2] = {0, 0};
long debounceDelay[2] = {50, 50};

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, ledState1);
  digitalWrite(ledPin2, ledState2);
}

void loop()
{
  delay(1);
  change();
  delay(1);
    /*
    * 둘다 켜진 경우 책상과 의자 거리 초기화
    * 하나만 켜진 경우 책상과 바닥 거리 초기화
    * 둘다 꺼진 경우 초기화 종료
    */
}

void change() {
  //////////////버튼 스위치의 토글스위치화//////////
  int reading1 = digitalRead(buttonPin1);
  int reading2 = digitalRead(buttonPin2);
  if (reading1 != lastButtonState1) {
    lastDebounceTime[0] = millis();
  }
  if (reading2 != lastButtonState2) {
    lastDebounceTime[1] = millis();
  }
  if ((millis() - lastDebounceTime[0]) > debounceDelay[0]) {
    if (reading1 != buttonState1) {
      buttonState1 = reading1;
      if (buttonState1 == HIGH) {
        ledState1 = !ledState1;
      }
    }
  }
  if ((millis() - lastDebounceTime[1]) > debounceDelay[1]) {
    if (reading2 != buttonState2) {
      buttonState2 = reading2;
      if (buttonState2 == HIGH) {
        ledState2 = !ledState2;
      }
    }
  }
  digitalWrite(ledPin1, ledState1);
  digitalWrite(ledPin2, ledState2);
  lastButtonState1 = reading1;
  lastButtonState2 = reading2;
}
