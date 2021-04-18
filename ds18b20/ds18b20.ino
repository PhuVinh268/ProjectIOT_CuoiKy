#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

#include "RSAAuth.h"
#define BAUD_RATE 9600

Servo myservo;
DynamicJsonBuffer jsonBuffer;
static unsigned long t_demo = 0;
static unsigned long last_time = 0;
int x = 180;
int max_demo = 3 * 1000;
String sta = "off";

char cstr[255];
byte en[255];
byte pac[255];

// Chân nối với Arduino
#define ONE_WIRE_BUS D5
//Thiết đặt thư viện onewire
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void timer_demo () {
  
  for(int i=0;i<100;i++)
  cstr[i]=0;
  sprintf(cstr, "{\"Src\":\"DV1\",\"Des\":\"GW\",\"temp\":%d}",(int)sensors.getTempCByIndex(0));
  Serial.println("Goi:");
  Serial.println((char*)cstr);
  yield();
  rsa.Send(cstr);
}
void demo_TIMER(void){
  unsigned long tnow = millis();
  if ((tnow - t_demo) >= max_demo) {
    t_demo = tnow;
    timer_demo();
  }
}
void setup() {
  Serial.begin(BAUD_RATE);
  swSer.begin(BAUD_RATE);
  sensors.begin();

  myservo.attach(D7);
  
  pinMode(D4,OUTPUT);
  digitalWrite(D4,HIGH);
  Serial.println("\nRSA serial test started");


}
byte buff[255];
int BEGIN=0;
int END=0;
int DesLen=0;

//byte unpac[255];
byte m[255];
void loop() {
  sensors.requestTemperatures();  
  DesLen=0;
  if(rsa.Receive(buff,&BEGIN,&END,m,&DesLen))
  {
    if(DesLen>0)
    {
      JsonObject& root = jsonBuffer.parseObject((char*)m);
      String con1=root[String("IDGW")];
      String con2=root[String("contac")];
      String con3=root[String("ID")];
      String con4=root[String("servo")];
      //Serial.println((char*)m);
      if(con1=="GW" && con3 == "DV1")
      {
        Serial.println((char*)m);
        Serial.println("Nhan:");
        if(con2 == String("on"))
        {
          digitalWrite(D4,LOW);
        }
        else if(con2==String("off"))
        {
          digitalWrite(D4,HIGH);
        }
        if(con4 == String("on"))
        {
          sta = "on";
        }
        else if(con4 == String("off"))
        {
          sta = "off";
        }
      } 
    }
  }
  else Serial.println("Nhan error:");
  if(sta == "on")
  {
    if(millis() - last_time >= 1000)
    {
      last_time = millis();
      myservo.write(x);
      if(x==180)
        x = 0;
       else x = 180;
    }
  }
  sensors.requestTemperatures();  
  demo_TIMER();
}
