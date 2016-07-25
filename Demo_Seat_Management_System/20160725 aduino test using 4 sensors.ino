#define trigPin1 9
#define trigPin2 11
#define trigPin3 6
#define trigPin4 5
#define DEBUG //DEBUG switch

#define echoPin1 8
#define echoPin2 12
#define echoPin3 7
#define echoPin4 4

#define buttonPin 2
#define ledPin 3
#define error 10
#define limit 400
#define trigoffdelay 200000
#define trigondelay 100000

#define count 4 // the number of sensers
#define standard 150 //the standard whether there is a person near a table. 

int ledState = LOW;
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
  int person;
  long start = 0, check[count] = {0};
  long duration[count] = {0}, distance[count] = {0}, sum[count] = {0}, avr[count] = {0};

//// the looping for average and filtering for strange value.

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


    for (int a = 0; a < count; a++)
      distance[a] = duration[a] * 17 / 1000;



    delay(1);

    for (int a = 0; a < count; a++) { 
      if (distance[a] >= limit) {
        i = -1;
        for (int b = 0; b < count; b++) {
          sum[b] = 0;
        }
        continue;
      }
    }

    delay(1);

    for (int a = 0; a < count; a++)
      sum[a] = sum[a] + distance[a];

    //delay(1000);
  }
  //Serial.println(sum[0]);

  for (int a = 0; a < count; a++) //Average
    avr[a] = (sum[a] / 10);

#ifdef DEBUG
    Serial.print("avr[0] = ");
    Serial.println(avr[0]);
    Serial.print("avr[1] = ");
    Serial.println(avr[1]);
    Serial.print("avr[2] = ");
    Serial.println(avr[2]);
    Serial.print("avr[3] = ");
    Serial.println(avr[3]);
#endif


    if((avr[0] < 140) && (avr[1] < 140) && (avr[2] < 140) && (avr[3] < 140) ) //if there is a person near table,
    {
      //  Serial.println("111111111111111");
         digitalWrite(ledPin, HIGH);
         
    }
    else //없다면? 
    {
     //  Serial.println("00000000000000");
        digitalWrite(ledPin, LOW);
    }
}

