
#define trigPin1 12
#define trigPin2 8
#define DEBUG //DEBUG switch

#define echoPin1 13
#define echoPin2 9

#define buttonPin 2
#define ledPin 3
#define error 10
#define limit 1000
#define trigoffdelay 20000
#define trigondelay 10000

#define count 2 // the number of sensers
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

  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);

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
#endif


    if((avr[0] < 140) && (avr[1] < 140)) //if there is a person near table,
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





