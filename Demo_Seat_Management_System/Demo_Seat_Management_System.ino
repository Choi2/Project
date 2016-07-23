//책상 위
#define trigPin1 4
#define echoPin1 12
#define buttonPin 2
#define ledPin 13
#define error 10//허용오차범위(cm)
#define limit 1000//센싱 한계 표시(cm)
#define trigoffdelay 20000//trig off시간
#define trigondelay 10000//tring on 시간
int ledState = LOW;
int buttonState;
int lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;
int t=10;

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
}

void loop()
{
  int i, a, b, person;
  long duration1, distance1, sum1 = 0, avr1, start = 0, check1 = 0;
  long temp1[10] = {0};
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
    delayMicroseconds(trigoffdelay);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(trigondelay);
    digitalWrite(trigPin1, LOW);
    duration1 = pulseIn(echoPin1, HIGH);
    distance1 = duration1 * 17 / 1000;
    temp1[i] = {distance1};
    
    //Serial.println(distance1);
    
    delay(1);
    if (temp1[i] >= limit) {
      i = -1;
      for (a = 0; a < 10; a++)
        temp1[a] = {0};
    }
    delay(1);
    //Serial.println(i);
  }
  for (i = 0; i < 10; i++) {
    sum1 = sum1 + temp1[i];
  }
  //Serial.println(sum);
  avr1 = (sum1 / 10);
  //Serial.println(avr1);
 
  if (ledState == LOW){ 
    start = avr1
    Serial.print("초기값 변수 : ");    
    Serial.println(start);
  }
  else if (ledState == HIGH) {
    check1 = avr1;
    Serial.print("비교값 변수 : ");
    Serial.println(start);

    if (check1 <= (start - error) || check2 <= (start - error) ) {
      person = 1; //사람 유
      Serial.print("사람존재유무 : ");
      Serial.println(person);
    }
    else {
      person = 0; //사람 무
      Serial.print("사람존재유무 : ");
      Serial.println(person);
    }
  }
}
