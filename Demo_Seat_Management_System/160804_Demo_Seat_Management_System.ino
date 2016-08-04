#include <OneWire.h>    
#include <DallasTemperature.h>    
#include <string.h>  
  
///////////////////////////////////////////  
#include <SoftwareSerial.h>  
     
#define SSID "SeongJin"  //공유기 SSID
#define PASS "0632287895"   //공유기 비번
#define DST_IP "175.126.112.111"   //MYSQL 서버 주소 
SoftwareSerial dbgSerial(3, 2); // RX, TX 3번,2번핀 
///////////////////////////////////////////  

#define trigPin1 9
#define trigPin2 11  
#define echoPin1 10
#define echoPin2 12  

#define count 2 // the number of sensers  
#define trigoffdelay 200000
#define trigondelay 100000
#define error 10
#define limit 400
      
//DS18B20 온도 센서의 데이터선인 가운데 핀을 아두이노 7번에 연결합니다.     
#define ONE_WIRE_BUS 7  
      
//1-wire 디바이스와 통신하기 위한 준비    
OneWire oneWire(ONE_WIRE_BUS);    
      
// oneWire선언한 것을 sensors 선언시 참조함.    
DallasTemperature sensors(&oneWire);    
      
//다비아스 주소를 저장할 배열 선언    
DeviceAddress insideThermometer;    
      
      
      
char * floatToString(char * outstr, double val, byte precision, byte widthp){  
 char temp[16]; //increase this if you need more digits than 15  
 byte i;  
  
 temp[0]='\0';  
 outstr[0]='\0';  
  
 if(val < 0.0){  
   strcpy(outstr,"-\0");  //print "-" sign  
   val *= -1;  
 }  
  
 if( precision == 0) {  
   strcat(outstr, ltoa(round(val),temp,10));  //prints the int part  
 }  
 else {  
   unsigned long frac, mult = 1;  
   byte padding = precision-1;  
     
   while (precision--)  
     mult *= 10;  
  
   val += 0.5/(float)mult;      // compute rounding factor  
     
   strcat(outstr, ltoa(floor(val),temp,10));  //prints the integer part without rounding  
   strcat(outstr, ".\0"); // print the decimal point  
  
   frac = (val - floor(val)) * mult;  
  
   unsigned long frac1 = frac;  
  
   while(frac1 /= 10)   
     padding--;  
  
   while(padding--)   
     strcat(outstr,"0\0");    // print padding zeros  
  
   strcat(outstr,ltoa(frac,temp,10));  // print fraction part  
 }  
  
 // generate width space padding   
 if ((widthp != 0)&&(widthp >= strlen(outstr))){  
   byte J=0;  
   J = widthp - strlen(outstr);  
  
   for (i=0; i< J; i++) {  
     temp[i] = ' ';  
   }  
  
   temp[i++] = '\0';  
   strcat(temp,outstr);  
   strcpy(outstr,temp);  
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
  
    
   boolean connected=false;  
   for(int i=0;i<10;i++)  
   {  
       if(connectWiFi())  
       {  
         connected = true;  
         break;  
       }  
   }  
     
   if (!connected){while(1);}  
   delay(5000);  
    
   dbgSerial.println("AT+CIPMUX=0");  
  ///////////////////////////////////////////////////////////////////////////     
       
   
   
      
  //1-wire 버스 초기화    
  sensors.begin();    
        
  //발견한 디바이스 갯수    
  Serial.print("Found ");    
  Serial.print(sensors.getDeviceCount(), DEC);    
  Serial.println(" devices.");    
      
  // parasite power 모드일 때에는  2핀(GND와 DQ 핀)만 연결하면 됨.    
  Serial.print("Parasite power is: ");     
  if (sensors.isParasitePowerMode()) Serial.println("ON");    
  else Serial.println("OFF");    
        
       
  //버스에서 첫번째 장치의 주소를 가져온다.    
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");     
        
  //버스에서 발견한 첫번째 장치의 주소 출력    
  Serial.print("Device 0 Address: ");    
  printAddress(insideThermometer);    
  Serial.println();    
      
  //데이터시트에서 확인결과 9~12비트까지 설정 가능    
  sensors.setResolution(insideThermometer, 10);    
       
  Serial.print("Device 0 Resolution: ");    
  Serial.print(sensors.getResolution(insideThermometer), DEC);     
  Serial.println();   

  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);


  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);


   
}    
      
      
// 온도를 출력하는 함수    
void printTemperature(DeviceAddress deviceAddress)    
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

   
        
    Serial.print("avr[0] = ");
    Serial.println(avr[0]);
    Serial.print("avr[1] = ");
    Serial.println(avr[1]);
 
     String cmd = "AT+CIPSTART=\"TCP\",\"";  
     cmd += DST_IP;  
     cmd += "\",80";  
     Serial.println(cmd);  
     dbgSerial.println(cmd);  
     if(dbgSerial.find("Error"))  
    {  
      Serial.println( "TCP connect error" );  
      return;  
    }  
     
  
    char test[20];  
    String temp(floatToString(test,avr[0], 2, 0));  
      
     cmd = "GET /index.php?avr="+temp+"\n\r";  
     dbgSerial.print("AT+CIPSEND=");  
     dbgSerial.println(cmd.length());  
          
       
     Serial.println(cmd);  
       
       
     if(dbgSerial.find(">"))  
     {  
       Serial.print(">");  
       }else  
       {  
         dbgSerial.println("AT+CIPCLOSE");  
         Serial.println("connect timeout");  
         delay(1000);  
         return;  
       }  
         
       dbgSerial.print(cmd);  
       delay(2000);  
       //Serial.find("+IPD");  
       while (Serial.available())  
       {  
         char c = Serial.read();  
         dbgSerial.write(c);  
         if(c=='\r') dbgSerial.print('\n');  
       }  
       Serial.println("====");  
       delay(1000);  
}    
      
//디바이스 주소를 출력하는 함수    
void printAddress(DeviceAddress deviceAddress)    
{    
  for (uint8_t i = 0; i < 8; i++)    
  {    
    if (deviceAddress[i] < 16) Serial.print("0");    
        Serial.print(deviceAddress[i], HEX);    
  }    
}    
      
      
void loop(void)    
{     
  Serial.print("Requesting temperatures...");    

  sensors.requestTemperaturesByIndex(0); //첫번째 센서의 온도값 읽어옴    
  Serial.println("DONE");    
       
  //센서에서 읽어온 온도를 출력    
  printTemperature(insideThermometer);    
}    
  
  
  
  
boolean connectWiFi()  
{  
   //dbgSerial.println("AT+CWMODE=1");  
     
   String cmd="AT+CWJAP=\"";  
   cmd+=SSID;  
   cmd+="\",\"";  
   cmd+=PASS;  
   cmd+="\"";  
   dbgSerial.println(cmd);  
   Serial.println(cmd);  
   delay(3000);  
    
   if(dbgSerial.find("OK"))  
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

