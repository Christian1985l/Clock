#include <Arduino.h>
#include <Wire.h>
#include "DHT.h"
#include <Time.h>
#include <DS1307RTC.h>
#include "TimerOne.h"
#include <AH_74HC595.h>

#define SER_Pin 4   //Serial data input
#define RCLK_Pin 3  //Register clock
#define SRCLK_Pin 2 //Shift register clock
#define DHTPIN 9    //Pin für DHT22
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//Initialisation
AH_74HC595 shifter(SER_Pin, RCLK_Pin, SRCLK_Pin); 
int a,hr,mi,i;
int d[4];
boolean val, sync;
int LED1=5;
int PIR = 6;
DHT dht(DHTPIN, DHTTYPE);


void setup(){
  Serial.begin(9600); 
  while (!Serial) ; // wait for serial
  delay(200);
  pinMode(5, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(6, INPUT);
  
  Timer1.initialize(5000); // 1 Sekunde (1000000), ab 5 ms kein flackern
  Timer1.attachInterrupt(anzeige);
  digitalWrite(LED1, LOW);
  dht.begin();
  a=0;
  sync=true;
}

void anzeige(){
  if (sync ==true | a==0){
    digitalWrite(7, HIGH);
    sync = false;
    pinMode(7, INPUT);
  }  
  if (val == true){
    if (a>3){
      a=0;
    }
    shifter.clear();  
  }else{
    if (a>3){
      a=0;
    }
    if (d[a]!=50){
      shifter.showNumber(d[a]);
    }
    else{
      shifter.setValue(B11100011);
    }}
  digitalWrite(LED1, !digitalRead(LED1)); // Pin LED 1 wechselt den Zustand (toggle)
  a++;
  digitalWrite(LED1, !digitalRead(LED1));
}

void loop(){
  for( i = 0; i < 4; i++ ){
      val=true;
      d[i]=50;
  }
  if (digitalRead(PIR) == HIGH)
  {
    val=false;
    for( i = 0; i < 4; i++ ){
      Time();
      digitalWrite(8, HIGH);
      delay(3000);
      Temp();
      digitalWrite(8, LOW);
      delay(3000);
    } 
  }    
}

void Time(){
  tmElements_t tm;
    if (RTC.read(tm)) {
    hr = tm.Hour;
    mi = tm.Minute;
    d[3] =  hr % 10;
    d[2] = (hr - d[3]) % 100 / 10;
    d[1] =  mi % 10;
    d[0] = (mi - d[1]) % 100 / 10;}  
}

void Temp(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    return;
  }
  d[3] =  (int) t % 10;
  d[2] = ((int) t - d[3]) % 100 / 10;
  d[1] = 12; // entspricht einem C auf der Anzeige
  d[0] = 50;
}

