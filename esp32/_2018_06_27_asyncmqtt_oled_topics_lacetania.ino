//_2018_06_27_asyncmqtt_oled_topics_lacetania.ino
//Jaume Nogues, Institut Rambla Prim
//jnogues@gmail.com


/*
This example uses FreeRTOS softwaretimers as there is no built-in Ticker library
*/


#include <WiFi.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include <SimpleTimer.h>
#include "SSD1306.h" //https://github.com/squix78/esp8266-oled-ssd1306
SSD1306  display(0x3c, 5, 4);//oled 128x64 pixels

#define WIFI_SSID "IoT-2"
#define WIFI_PASSWORD "pitufito"

#define MQTT_HOST "iot.eclipse.org"
//#define MQTT_HOST "vps249990.ovh.net"//servidor jnogues
//#define MQTT_HOST IPAddress(192, 168, 1, 222)//RPi
#define MQTT_PORT 1883

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

#define thisNODE        "esp32-14"
#define thisNode        "/lacetania/esp32-14"
#define commands        "/lacetania/esp32-14/commands"
#define replyQ26        "/lacetania/esp32-14/relay/Q26"
#define setQ26          "/lacetania/esp32-14/relay/Q26/set"
#define replyI25        "/lacetania/esp32-14/button/I25"
#define replyI16        "/lacetania/esp32-14/button/I16"
#define topicUpTime     "/lacetania/esp32-14/upTime"
#define topicHearBeat   "/lacetania/esp32-14/heartBeat"
#define topicState      "/lacetania/esp32-14/state"
#define topicError      "/lacetania/esp32-14/error"
#define topicOled       "/lacetania/esp32-14/oled/+"
#define topicOledLine1  "/lacetania/esp32-14/oled/line1"
#define topicOledLine2  "/lacetania/esp32-14/oled/line2"

long lastMsg = 0;
char msg[50];
int value = 0;

boolean Q26=0;
byte commandOK=0;
boolean estatI16=1;
boolean estatI16ant=1;
boolean estatI25=1;
boolean estatI25ant=1;
byte dotOled=0;
boolean dotTog=0;

String estatESP="";
char strIP[20];

String line1="";
String line2="";

SimpleTimer timer;

void setup() {
  pinMode(15,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(16, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  digitalWrite(15,LOW);
  digitalWrite(26,LOW);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("[NODE] This node is: ");
  Serial.println(thisNode);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
  //display.drawStringMaxWidth(0,0,20, "pardillo");
  display.drawString(0, 0, thisNode);
  display.display();
  delay(3000);//temps per  

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  timer.setInterval(62000L, publishUpTime); //  Here you set interval (1sec) and which function to call 
  timer.setInterval(600000L, publishHeartBeat); 
  timer.setInterval(10000L, publishState);
  timer.setInterval(1000L, alive);
  timer.setInterval(305L, pushButtons);
  timer.setInterval(300L, pushButtonI25);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  //mqttClient.setCredentials("pi", "raspberry");//Utilitzar per brokers amb usuari i password

  connectToWifi();
}

void loop() 
{
  timer.run();
}


void connectToWifi() {
  Serial.println("[WIFI] Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}


void connectToMqtt() {
  Serial.println("[MQTT] Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("[WIFI] connected");
        Serial.print("[WIFI] IP address: ");
        Serial.println(WiFi.localIP());
        WiFi.setHostname(thisNode);
        display.clear();
        display.setFont(ArialMT_Plain_24);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
        display.drawString(25, 5, "WIFI OK");
        display.display();
        connectToMqtt();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("[WIFI] WiFi lost connection");
        display.clear();
        display.setFont(ArialMT_Plain_24);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
        display.drawString(25, 15, "WIFI KO");
        display.display();
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.print("[MQTT] Connected to MQTT broker ");
  Serial.println(MQTT_HOST);
  Serial.print("[MQTT] Session present: ");
  Serial.println(sessionPresent);
  IPAddress ip;
  ip=WiFi.localIP();
  sprintf(strIP, "%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
  mqttClient.publish(thisNode, 0, true, strIP); 
  uint16_t packetIdSub = mqttClient.subscribe(commands, 0);
  uint16_t packetIdSub2 = mqttClient.subscribe(setQ26, 0);
  uint16_t packetIdSub3 = mqttClient.subscribe(topicOled, 0); 
  display.setFont(ArialMT_Plain_10);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
  display.drawString(50, 30, thisNODE);
  display.drawString(40, 45, strIP);
  display.display();

}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("[MQTT] Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) 
{
  commandOK=1;
  //El payload es rebia amb caracters estranys, he seguit aquest consell
  //https://github.com/marvinroger/async-mqtt-client/issues/104
  //Metode 1, dubtos
  //char copyPayload[len+1];
  //copyPayload[len+1] = '\0';
  //strncpy(copyPayload, payload, len); 
  //Metode 2, ok
  //String fixedStr = ((String)payload).substring(0,len);
  //Metode 3, ok, el que he triat
  char _payload[len + 1];
    for (int i = 0; i <=len; i++) {
      if (i == len) {
        _payload[i] = '\0';
        continue;
      }
      _payload[i] = payload[i];
  }
  
  
  Serial.print("[MQTT] Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("len:");
  Serial.print(len);
  Serial.print("  ");
  //Serial.println(payload);
  //Serial.println(copyPayload);
  //Serial.println(fixedStr);
  Serial.println(_payload);
   
  if (strcmp(topic, setQ26)==0)
  {
    //set Q26
      if (strcmp_P(_payload, PSTR("1"))==0){
          Q26=1;
          digitalWrite(26,HIGH);
          mqttClient.publish(replyQ26, 0, true, "1");
          commandOK=0;  
          }
      if (strcmp_P(_payload, PSTR("0"))==0){
          Q26=0;
          digitalWrite(26,LOW);
          mqttClient.publish(replyQ26, 0, true, "0");
          commandOK=0;  
          } 
  }

  if (strcmp(topic, commands)==0)
  {
      //sortida Q26
      if (strcmp_P(_payload, PSTR("q26on"))==0){
          Q26=1;
          digitalWrite(26,HIGH);
          mqttClient.publish(replyQ26, 0, true, "1");
          commandOK=0;
          
          }
       else if (strcmp_P(_payload, PSTR("q26off"))==0){
          Q26=0;
          digitalWrite(26,LOW); 
          mqttClient.publish(replyQ26, 0, true, "0");
          commandOK=0;
          
          }    
        else if (strcmp_P(_payload, PSTR("q26tog"))==0){
          //Q26=!Q26;
          Q26=!digitalRead(26); 
          digitalWrite(26,Q26); 
          if (Q26==1) mqttClient.publish(replyQ26, 0, true, "1");
          if (Q26==0) mqttClient.publish(replyQ26, 0, true, "0");
          commandOK=0;
          
          }
        else if (strcmp_P(_payload, PSTR("q26?"))==0){
          Q26=digitalRead(26); 
          if (Q26==1) mqttClient.publish(replyQ26, 0, true, "1");
          if (Q26==0) mqttClient.publish(replyQ26, 0, true, "0");
          commandOK=0;
          
          }
        
        //entrada I25  
        else if (strcmp_P(_payload, PSTR("i25?"))==0){
          boolean i25=digitalRead(25); 
          if (i25==0) mqttClient.publish(replyI25, 0, true, "1");
          if (i25==1) mqttClient.publish(replyI25, 0, true, "0");
          commandOK=0;
          
          }
    
        //entrada I16  
        else if (strcmp_P(_payload, PSTR("i16?"))==0){
          boolean i16=digitalRead(16); 
          if (i16==0) mqttClient.publish(replyI16, 0, true, "1");
          if (i16==1) mqttClient.publish(replyI16, 0, true, "0");
          commandOK=0;
          
          }
               
        //info
        else if (strcmp_P(_payload, PSTR("info?"))==0){
          publishState();
          commandOK=0;
          
          }
        
        //allOFF
        else if (strcmp_P(_payload, PSTR("allOFF"))==0){
          digitalWrite(26,LOW);
          commandOK=0;      
        }
        //allON
        else if (strcmp_P(_payload, PSTR("allON"))==0){
          digitalWrite(26,HIGH);
          commandOK=0;
        }
        
  }
  
  if (strcmp(topic, topicOledLine1)==0)
  {
    //oled
    line1=_payload;
    display.clear();
    display.setFont(ArialMT_Plain_24);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
    //display.drawString(0, 10, topic);
    display.drawString(0, 2, line1);
    display.drawString(0, 28, line2);
    display.display();
    commandOK=0;   
  }

  else if (strcmp(topic, topicOledLine2)==0)
  {
    //oled
    line2=_payload;
    display.clear();
    display.setFont(ArialMT_Plain_24);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
    display.drawString(0, 2, line1);
    display.drawString(0, 28, line2);
    display.display();
    commandOK=0;   
  } 

  if (commandOK)
  {
    //ERROR
     mqttClient.publish(topicError, 0, true, "command ERROR");                             
  }
}

void publishState(){
  char estat[256];
  memset(estat,0,256);//esborra estat
  boolean I16=!digitalRead(16);
  boolean I25=!digitalRead(25);
  boolean Q26=digitalRead(26);
  unsigned long uT=millis()/60000;
  estatESP="{\"I16\":\""+ String(I16) + "\", ";
  estatESP=estatESP+"\"I25\":\""+ String(I25) + "\", ";
  estatESP=estatESP+"\"Q26\":\""+ String(Q26) + "\", ";
  estatESP=estatESP+"\"uT\":\""+ String(uT) + "\"";
  estatESP=estatESP + "}";
  estatESP.toCharArray(estat,256);
  mqttClient.publish(topicState, 0, true, estat);
}

void publishUpTime(){
  char strUpTime[10];
  unsigned long upTime=millis()/60000;
  sprintf (strUpTime, "%u", upTime);
  mqttClient.publish(topicUpTime, 0, true, strUpTime);
  Serial.print("[TIME] ");
  Serial.println(upTime); 
}

void publishHeartBeat(){
  char strHeartBeat[10];
  unsigned long heartBeat=millis()/60000;
  sprintf (strHeartBeat, "%u", heartBeat);
  mqttClient.publish(topicHearBeat, 0, true, strHeartBeat);
}

void alive()
{
  dotTog=!dotTog;
  if (dotTog==1)
  {
      //display.setPixel(dotOled,0);
      display.setColor(BLACK);
      display.setFont(ArialMT_Plain_10);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
      display.drawString(122, 55, "*");
      display.display();
      display.setColor(WHITE);      
  }
  if (dotTog==0)
  {
      display.setFont(ArialMT_Plain_10);// ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
      display.drawString(122, 55, "*");
      display.display();
  }
}

void pushButtons()
{
  estatI16=digitalRead(16);
  if (estatI16ant!=estatI16){
    if (estatI16==0) mqttClient.publish(replyI16, 0, true, "0");
    if (estatI16==1) mqttClient.publish(replyI16, 0, true, "1");
  estatI16ant=estatI16;  
  }
}

void pushButtonI25()
{
  estatI25=digitalRead(25);
  if (estatI25ant==0 && estatI25==1)
  {
    bool temp=digitalRead(26);
    if (temp){ digitalWrite(26,LOW);  mqttClient.publish(replyQ26, 0, true, "0");}
    if (!temp){ digitalWrite(26,HIGH);mqttClient.publish(replyQ26, 0, true, "1");}
    estatI25ant=1;
  }
  if (estatI25ant==1 && estatI25==0)
  {
    estatI25ant=0;
  }
}
