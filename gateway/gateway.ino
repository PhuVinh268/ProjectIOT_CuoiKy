#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include "RSAAuth.h"
#define BAUD_RATE 9600

const char ssid[] = "Sgu";
const char pass[] = "........";
WiFiClient net;
MQTTClient client;
DynamicJsonBuffer jsonBuffer;
unsigned long lastMillis = 0;

char cstr[255];
byte en[255];
byte pac[255];

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
    JsonObject& root = jsonBuffer.parseObject(payload);
  String con1=root[String("IDGW")];
  String con2=root[String("contac")];
  String con3=root[String("ID")];
  String con4=root[String("servo")];
  for(int i=0;i<100;i++)
  cstr[i]=0;
  if(con2 == "on" && con3 == "DV1")
  {
    sprintf(cstr, "{\"IDGW\":\"GW\",\"contac\":\"on\",\"ID\":\"DV1\"}");
    Serial.println("Goi:");
    Serial.println((char*)cstr);
    yield();
    rsa.Send(cstr);
  }
  else if(con2 == "off" && con3 == "DV1")
  {
    sprintf(cstr, "{\"IDGW\":\"GW\",\"contac\":\"off\",\"ID\":\"DV1\"}");
    Serial.println("Goi:");
    Serial.println((char*)cstr);
    yield();
    rsa.Send(cstr);
  }  
  else if(con2 == "on" && con3 == "DV2")
  {
    sprintf(cstr, "{\"IDGW\":\"GW\",\"contac\":\"on\",\"ID\":\"DV2\"}");
    Serial.println("Goi:");
    Serial.println((char*)cstr);
    yield();
    rsa.Send(cstr);
  }
  else if(con2 == "off" && con3 == "DV2")
  {
    sprintf(cstr, "{\"IDGW\":\"GW\",\"contac\":\"off\",\"ID\":\"DV2\"}");
    Serial.println("Goi:");
    Serial.println((char*)cstr);
    yield();
    rsa.Send(cstr);
  }
  if(con4 != NULL)
  {
    sprintf(cstr, "{\"IDGW\":\"GW\",\"servo\":\"%s\",\"ID\":\"DV1\"}",con4.c_str());
    Serial.println("Goi:");
    Serial.println((char*)cstr);
    yield();
    rsa.Send(cstr);
  }
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "vinhhien", "123")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("GW");
  // client.unsubscribe("/hello");
}


void setup() {
  Serial.begin(BAUD_RATE);
  WiFi.begin(ssid, pass);
  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("192.168.43.16", net);
  client.onMessage(messageReceived);

  connect();
  swSer.begin(BAUD_RATE);

  Serial.println("\nRSA serial test started");
}
byte buff[255];
int BEGIN=0;
int END=0;
int DesLen=0;
int val = 0;


//byte unpac[255];
byte m[255];
void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }
  
  DesLen=0;
  if(rsa.Receive(buff,&BEGIN,&END,m,&DesLen))
  {
    if(DesLen>0)
    {
      Serial.println("Nhan:");
      Serial.println((char*)m);
      JsonObject& root1 = jsonBuffer.parseObject((char*)m);
      String con4=root1[String("Src")];
      String con5=root1[String("Des")];
      int con6=root1[String("temp")];
      int con7 = root1[String("MQ2")];
      if(con7 == NULL)
      {
        Serial.println("{\"IDGW\":\"GW\",\"ID\":\""+(String)con4+"\",\"temp\":\""+(String)con6+"\"}");
        client.publish("common","{\"IDGW\":\"GW\",\"ID\":\""+con4+"\",\"temp\":\""+(String)con6+"\"}"); 
      }
      else if(con6 == NULL)
      {
        Serial.println("{\"IDGW\":\"GW\",\"ID\":\""+(String)con4+"\",\"MQ2\":\""+(String)con7+"\"}");
        client.publish("common","{\"IDGW\":\"GW\",\"ID\":\""+con4+"\",\"MQ2\":\""+(String)con7+"\"}"); 
      }
    }
  }

  // publish a message roughly every second.
}
