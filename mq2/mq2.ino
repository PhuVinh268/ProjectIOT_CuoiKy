#include <ArduinoJson.h>


#include "RSAAuth.h"
#define BAUD_RATE 9600

DynamicJsonBuffer jsonBuffer;
static unsigned long t_demo = 0;
int max_demo = 3 * 1000;

char cstr[255];
byte en[255];
byte pac[255];
void timer_demo () {
  
  for(int i=0;i<100;i++)
  cstr[i]=0;
  sprintf(cstr, "{\"Src\":\"DV2\",\"Des\":\"GW\",\"MQ2\":%d}", map(analogRead(A0),1023,0,0,100));
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
  
  DesLen=0;
  if(rsa.Receive(buff,&BEGIN,&END,m,&DesLen))
  {
    if(DesLen>0)
    {
      JsonObject& root = jsonBuffer.parseObject((char*)m);
      String con1=root[String("IDGW")];
      String con2=root[String("contac")];
      String con3=root[String("ID")];
      //Serial.println((char*)m);
      if(con1=="GW" && con3 == "DV2")
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
      } 
    }
  }
  else Serial.println("Nhan error:");
  
  demo_TIMER();
}
