#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <WiFiManager.h>
#include <DNSServer.h>

#include <Ticker.h>

#define DS 5 
#define SHCP 4
#define STCP 13
#define NUM9 14
#define NUM8 12
#define lightSW 15


#define Lightime 60
#define gaptime 60

bool ifupdate = 0;
bool if1min = 0;

int Nums[] = {1,2,4,8,16,32,64,128};

u_int settimer = 1;
int icounter = 0;

WiFiManager wifiManager;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"ntp1.aliyun.com");

Ticker add1secTicker;
Ticker normal1min;

void drawNum(int Num){
  switch (Num)
  {
  case 9:
    digitalWrite(NUM9,HIGH); 
    break;
  case 8:
    digitalWrite(NUM8,HIGH);
    break;
  default:
    digitalWrite(STCP,LOW);
    shiftOut(DS,SHCP,MSBFIRST,Nums[Num]);
    digitalWrite(STCP,HIGH);
    break;
  }
}


void drawdig1(){
  int hour = timeClient.getHours();
  if((hour / 10) > 0){
     drawNum(hour / 10);
    }
  icounter++;
  settimer = millis();
  Serial.println(hour/10);
}
void drawdig2(){
  int hour = timeClient.getHours();
  drawNum(hour % 10);
  icounter++;
  settimer = millis();
  Serial.println(hour%10);
}

void drawdig3(){
  int minu = timeClient.getMinutes();
  drawNum(minu / 10);
  icounter++;
  settimer = millis();
  Serial.println(minu/10);
}
void drawdig4(){
  int minu = timeClient.getMinutes();
  drawNum(minu % 10);
  icounter = 0;
  settimer = millis();
  Serial.println(minu%10);

  
}
void add60sec(){
  ifupdate = 1;
}




void cleandig(){
  digitalWrite(NUM8,LOW);
  digitalWrite(NUM9,LOW);
  digitalWrite(STCP,LOW);
  shiftOut(DS,SHCP,MSBFIRST,0);
  digitalWrite(STCP,HIGH);
  
}
void drawvoid(){
  cleandig();
  icounter++;
  
  settimer = millis();
}
void castNums(){
switch (icounter)
 {
 case 1:
  settimer = 0;
  cleandig();
  drawdig2();
  
 break;
 case 2:
  settimer = 0;
  cleandig();
  drawvoid();
 break;
 case 3:
  cleandig();
  settimer = 0;
  drawdig3();
 break;

 case 4:
  cleandig();
  settimer = 0;
  drawdig4();
  
 break;
  
 default:
 cleandig();
  break;
 }
}

void add1min(){
  if1min = 1;
}



void setup() {
  pinMode(NUM8,OUTPUT);
  pinMode(NUM9,OUTPUT);
  pinMode(DS,OUTPUT);
  pinMode(SHCP,OUTPUT);
  pinMode(STCP,OUTPUT);
  pinMode(lightSW,OUTPUT);

  digitalWrite(STCP,HIGH);
  digitalWrite(lightSW,HIGH);
  
  wifiManager.autoConnect("AutoConnectAP");

  //Serial.begin(115200);
  add1secTicker.attach(8,add60sec);
  normal1min.attach(gaptime,add1min);
  
  timeClient.begin();
  timeClient.setTimeOffset(28800);
  timeClient.update();

  cleandig();

     

}




void loop() {
  if(if1min){
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    drawdig1();
    if1min = 0;
  }
  if(ifupdate){
    timeClient.update();
    add1secTicker.detach();
    ifupdate = 0;
    timeClient.update();
  }  



  if((millis() - settimer) / 10 == Lightime){
    castNums();
  }
     

}