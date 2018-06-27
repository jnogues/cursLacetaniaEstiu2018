//clientMQTTesp8266.ino
//juny 2018
//Jaume Nogues, Institut Rambla Prim
//jnogues@gmail.com

#include <JustWifi.h> //https://github.com/xoseperez/justwifi versio 2
#define MQTT_MAX_PACKET_SIZE 256 //default 128, Jaume, modificat a PubSubClient.h
#include <PubSubClient.h>
#include <SimpleTimer.h> // here is the SimpleTimer library
SimpleTimer timer; // Create a Timer object called "timer1"! 
WiFiClient espClient;
PubSubClient client(espClient);

//************************************************************************************
/*
#include <Adafruit_NeoPixel.h>
#define PIN 10
#define NUMPIXELS 8
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
*/

/*
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 10
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
*/

/*
#include <DHT.h>  //adafruit
#define DHTPIN 10
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
*/

/*
//Comentar tot allò relacionat amb gpio4 i gpio5
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>//https://github.com/adafruit/Adafruit_BME280_Library ojo, truc
// modificar  #define BME280_ADDRESS  (0x77) a #define BME280_ADDRESS    (0x77) al fitxer Adafruit_BME280.h
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
*/

//broker public
#define mqtt_server     "iot.eclipse.org"
//broker local RPi
//IPAddress mqtt_server(192, 168, 1, 222); 
//#define userMQTT      "pi" 
//#define passMQTT      "raspberry"

#define ssid2 "IoT-2"
#define pass2 "pitufito"

#define ssid3 "IoT"
#define pass3 "pitufito"

#define thisNode              "/lacetania/node-11"
#define INcommands            "/lacetania/node-11/commands"
#define replyQ0               "/lacetania/node-11/relay/Q0"
#define setQ0                 "/lacetania/node-11/relay/Q0/set"
#define replyQ2               "/lacetania/node-11/relay/Q2"
#define setQ2                 "/lacetania/node-11/relay/Q2/set"
#define replyQ13              "/lacetania/node-11/relay/Q13"
#define setQ13                "/lacetania/node-11/relay/Q13/set"
#define replyQ15              "/lacetania/node-11/relay/Q15"
#define setQ15                "/lacetania/node-11/relay/Q15/set"
#define setRelays             "/lacetania/node-11/relay/+/set"

#define replyI4               "/lacetania/node-11/reply/I4"
#define replyI5               "/lacetania/node-11/reply/I5"
#define replyI12              "/lacetania/node-11/reply/I12"
#define replyI14              "/lacetania/node-11/reply/I14"

#define topicUpTime           "/lacetania/node-11/upTime"
#define topicAI0              "/lacetania/node-11/AI0"
#define topicHearBeat         "/lacetania/node-11/heartBeat"

#define topicDHTtemperature   "/lacetania/node-11/dhtT"
#define topicDHThumidity      "/lacetania/node-11/dhtH"
#define topicTemperature      "/lacetania/node-11/temperature"
#define topicBMEtemperature   "/lacetania/node-11/bmeT"
#define topicBMEhumidity      "/lacetania/node-11/bmeH"
#define topicBMEpressure      "/lacetania/node-11/bmeP"

#define topicState            "/lacetania/node-11/state"
#define topicError            "/lacetania/node-11/error"
#define topicReplyDimmer      "/lacetania/node-11/reply/dimmer"
#define topicReplyColor       "/lacetania/node-11/reply/color"
#define topicAllNodes         "/lacetania/allNodes/commands"

//*******************************************************************************************

long lastMsg = 0;
char msg[50];
int value = 0;

boolean Q0=1;
boolean Q2=1;
boolean Q13=0;
boolean Q15=0;
byte commandOK=0;
boolean estatI4=0;
boolean estatI4ant=0;
boolean estatI5=0;
boolean estatI5ant=0;
boolean estatI14=0;
boolean estatI14ant=0;
boolean estatI12=0;
boolean estatI12ant=0;

float t=0;
float h=0;
float p=0;
int temperatura=0;
int humedad=0;
int presion=0;
char strTemperatura[10];
char strHumedad[10];
char strPresion[10];
int AI0=0;
String estatESP="";

int red=0, green=0, blue=0;
byte dimmer=0;
char strIP[20];

void setup() 
{
  pinMode(0,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(15,OUTPUT);
  pinMode(16,OUTPUT);
  pinMode(4,INPUT);//pinMode(4, INPUT_PULLUP);//comentar si fem servir BME280
  pinMode(5,INPUT);//pinMode(5,INPUT_PULLUP);//comentar si fem servir BME280
  pinMode(12, INPUT);//pinMode(12,INPUT_PULLUP);
  pinMode(14, INPUT);//pinMode(14,INPUT_PULLUP);
  digitalWrite(0,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(13,HIGH);
  digitalWrite(15,LOW);
  digitalWrite(16,LOW);
  
  Serial.begin(115200);
  delay(100);
  Serial.print("[NODE] ********** This Node is: ");
  Serial.print(thisNode);
  Serial.println(" **********");
  delay(200);
  // Set WIFI hostname (otherwise it would be ESP-XXXXXX)
  jw.setHostname(thisNode);
  // Callbacks
  jw.subscribe(infoCallback);
  // -------------------------------------------------------------------------
  // AP mode only as fallback
  jw.enableAP(false);
  jw.enableAPFallback(true);
  // -------------------------------------------------------------------------
  // Enable STA mode (connecting to a router)
  jw.enableSTA(true);
  // Configure it to scan available networks and connect in order of dBm
  jw.enableScan(true);
  // Clean existing network configuration
  jw.cleanNetworks();
  // Add a network with password
  jw.addNetwork(ssid1, pass1);
  jw.addNetwork(ssid2, pass2);
  jw.addNetwork(ssid3, pass3);
  
  Serial.println("[WIFI] Connecting Wifi...");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  timer.setInterval(60000L, publishUpTime); 
  timer.setInterval(500L, alive);
  timer.setInterval(64000L, publishA0);
  timer.setInterval(600000L, publishHeartBeat);
  //timer.setInterval(35000L, publishDHT);
  //timer.setInterval(35000L, publishDS18B20);
  //timer.setInterval(30000L, publishBME280);
  timer.setInterval(65000L, publishState);
  timer.setInterval(305L, pushButtonI4);
  timer.setInterval(305L, pushButtons); 
  timer.setInterval(5000L, checkMQTT); 
  Serial.printf("[TMRS] timers running\n");
  //DS18B20.begin();
  //dht.begin();
  //pixels.begin(); // This initializes the NeoPixel library.
  //pixels.show(); // Initialize all pixels to 'off'
  //bool status = bme.begin();  
  //if (!status) {
  //      Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //}
}

void loop() 
{
  // This call takes care of it all
  jw.loop();
  client.loop();
  timer.run(); // SimpleTimer is working
}


void callback(char* topic, byte* payload, unsigned int length) {
  char buffer_[50];
  int i = 0;
  Serial.print("[MQTT] Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    buffer_[i]=payload[i];
  }
  buffer_[i]='\0';
  //Serial.println();
  Serial.println(buffer_);
  
  //set Q0
  if (strcmp(topic, setQ0)==0)
  {    
      if (strcmp_P(buffer_, PSTR("1"))==0){
          Q0=1;
          digitalWrite(0,LOW);
          client.publish(replyQ0, "1");
          commandOK=0;  
          }
      if (strcmp_P(buffer_, PSTR("0"))==0){
          Q0=0;
          digitalWrite(0,HIGH);
          client.publish(replyQ0, "0");
          commandOK=0;  
          }    
  }
  //set Q2
  if (strcmp(topic, setQ2)==0)
  {
      if (strcmp_P(buffer_, PSTR("1"))==0){
          Q2=1;
          digitalWrite(2,LOW);
          client.publish(replyQ2, "1");
          commandOK=0;  
          }
      if (strcmp_P(buffer_, PSTR("0"))==0){
          Q2=0;
          digitalWrite(2,HIGH);
          client.publish(replyQ2, "0");
          commandOK=0;  
          }    
  }
  //set Q13
  if (strcmp(topic, setQ13)==0)
  {
      if (strcmp_P(buffer_, PSTR("1"))==0){
          Q13=1;
          digitalWrite(13,HIGH);
          client.publish(replyQ13, "1");
          commandOK=0;  
          }
      if (strcmp_P(buffer_, PSTR("0"))==0){
          Q13=0;
          digitalWrite(13,LOW);
          client.publish(replyQ13, "0");
          commandOK=0;  
          }    
  }
  //set Q15
  if (strcmp(topic, setQ15)==0)
  {
      if (strcmp_P(buffer_, PSTR("1"))==0){
          Q15=1;
          digitalWrite(15,HIGH);
          client.publish(replyQ15, "1");
          commandOK=0;  
          }
      if (strcmp_P(buffer_, PSTR("0"))==0){
          Q15=0;
          digitalWrite(15,LOW);
          client.publish(replyQ15, "0");
          commandOK=0;  
          }    
  }

  
  if (strcmp(topic, INcommands)==0)
  {
      //sortida Q0
      if (strcmp_P(buffer_, PSTR("q0on"))==0){
          Q0=1;
          digitalWrite(0,LOW);
          client.publish(replyQ0, "1");
          commandOK=0;
          
          }
       else if (strcmp_P(buffer_, PSTR("q0off"))==0){
          Q0=0;
          digitalWrite(0,HIGH); 
          client.publish(replyQ0, "0");
          commandOK=0;
          
          }    
        else if (strcmp_P(buffer_, PSTR("q0tog"))==0){
          //Q0=!Q0;
          Q0=!digitalRead(0); 
          digitalWrite(0,Q0); 
          if (Q0==1) client.publish(replyQ0, "0");
          if (Q0==0) client.publish(replyQ0, "1");
          commandOK=0;
          
          }
        else if (strcmp_P(buffer_, PSTR("q0?"))==0){
          Q0=digitalRead(0); 
          if (Q0==1) client.publish(replyQ0, "0");
          if (Q0==0) client.publish(replyQ0, "1");
          commandOK=0;
          
          }
        //sortida Q2
        else if (strcmp_P(buffer_, PSTR("q2on"))==0){
          Q2=1;
          digitalWrite(2,LOW);
          client.publish(replyQ2, "1");
          commandOK=0;
          
          }
        else if (strcmp_P(buffer_, PSTR("q2off"))==0){
          Q2=0;
          digitalWrite(2,HIGH); 
          client.publish(replyQ2, "0");
          commandOK=0;
          
          }    
        else if (strcmp_P(buffer_, PSTR("q2tog"))==0){
          Q2=!Q2;
          digitalWrite(2,Q2); 
          if (Q2==1) client.publish(replyQ2, "0");
          if (Q2==0) client.publish(replyQ2, "1");
          commandOK=0;
          
          }
        else if (strcmp_P(buffer_, PSTR("q2?"))==0){
          Q2=digitalRead(2); 
          if (Q2==1) client.publish(replyQ2, "0");
          if (Q2==0) client.publish(replyQ2, "1");
          commandOK=0;
          
          }  
      //sortida Q13
        else if (strcmp_P(buffer_, PSTR("q13on"))==0){
          Q13=1;
          digitalWrite(13,HIGH);
          client.publish(replyQ13, "1");
          commandOK=0;
          
          }
        else if (strcmp_P(buffer_, PSTR("q13off"))==0){
          Q13=0;
          digitalWrite(13,LOW); 
          client.publish(replyQ13, "0");
          commandOK=0;
          
          }    
        else if (strcmp_P(buffer_, PSTR("q13tog"))==0){
          Q13=!Q13;
          digitalWrite(13,Q13); 
          if (Q13==0) client.publish(replyQ13, "0");
          if (Q13==1) client.publish(replyQ13, "1");
          commandOK=0;
          
          }
        else if (strcmp_P(buffer_, PSTR("q13?"))==0){
          Q13=digitalRead(13); 
          if (Q13==0) client.publish(replyQ13, "0");
          if (Q13==1) client.publish(replyQ13, "1");
          commandOK=0;
          
          }
        //sortida Q15
        else if (strcmp_P(buffer_, PSTR("q15on"))==0){
          Q15=1;
          digitalWrite(15,HIGH);
          client.publish(replyQ15, "1");
          commandOK=0;
          
          }
        else if (strcmp_P(buffer_, PSTR("q15off"))==0){
          Q15=0;
          digitalWrite(15,LOW); 
          client.publish(replyQ15, "0");
          commandOK=0;
          
          }    
        else if (strcmp_P(buffer_, PSTR("q15tog"))==0){
          //Q15=!Q15;
          Q15=!digitalRead(15);
          digitalWrite(15,Q15); 
          if (Q15==0) client.publish(replyQ15, "0");
          if (Q15==1) client.publish(replyQ15, "1");
          commandOK=0;
          
          }
        else if (strcmp_P(buffer_, PSTR("q15?"))==0){
          Q15=digitalRead(15); 
          if (Q15==0) client.publish(replyQ15, "0");
          if (Q15==1) client.publish(replyQ15, "1");
          commandOK=0;
          
          }
        //entrada I5  
        else if (strcmp_P(buffer_, PSTR("i5?"))==0){
          boolean i5=digitalRead(5); 
          if (i5==0) client.publish(replyI5, "0");
          if (i5==1) client.publish(replyI5, "1");
          commandOK=0;
          
          }
        //entrada I4  
        else if (strcmp_P(buffer_, PSTR("i4?"))==0){
          boolean i4=digitalRead(4); 
          if (i4==0) client.publish(replyI4, "0");
          if (i4==1) client.publish(replyI4, "1");
          commandOK=0;
          
          }
        //entrada I12  
        else if (strcmp_P(buffer_, PSTR("i12?"))==0){
          boolean i12=digitalRead(12); 
          if (i12==0) client.publish(replyI12, "0");
          if (i12==1) client.publish(replyI12, "1");
          commandOK=0;
          
          }  
        //entrada I14  
        else if (strcmp_P(buffer_, PSTR("i14?"))==0){
          boolean i14=digitalRead(14); 
          if (i14==0) client.publish(replyI14, "0");
          if (i14==1) client.publish(replyI14, "1");
          commandOK=0;
          
          }
        //entrada AI0  
        else if (strcmp_P(buffer_, PSTR("ai0?"))==0){
          publishA0();
          commandOK=0;
          
          }       
        //info
        else if (strcmp_P(buffer_, PSTR("info?"))==0){
          publishState();
          commandOK=0;
          
          }
        //neopixel-color=r,g,b command
        else if(strstr(buffer_,"color=")!=0){
          String strColor(buffer_);
          String strRed;
          String strGreen;
          String strBlue;
          strRed=strColor.substring(6, strColor.indexOf(","));
          strColor=strColor.substring(strRed.length()+7);
          strGreen=strColor.substring(0, strColor.indexOf(","));
          strBlue=strColor.substring(strGreen.length()+1);
          red=strRed.toInt();
          green=strGreen.toInt();
          blue=strBlue.toInt();
          red=constrain(red, 0, 255);
          green=constrain(green, 0, 255);
          blue=constrain(blue, 0, 255);
          //updateNeopixel(); 
          commandOK=3;   
          }  
        //neopixel-rgb(r,g,b) command
        else if(strstr(buffer_,"rgb(")!=0){
          String strColor(buffer_);
          String strRed;
          String strGreen;
          String strBlue;
          strRed=strColor.substring(4, strColor.indexOf(","));
          strColor=strColor.substring(strRed.length()+5);
          strGreen=strColor.substring(0, strColor.indexOf(","));
          strBlue=strColor.substring(strGreen.length()+1, strColor.indexOf(")"));
          red=strRed.toInt();
          green=strGreen.toInt();
          blue=strBlue.toInt();
          red=constrain(red, 0, 255);
          green=constrain(green, 0, 255);
          blue=constrain(blue, 0, 255);
          //updateNeopixel();
          commandOK=3;
          
        }
        //dimmer command
        else if(strstr(buffer_,"dimmer=")!=0){
          char temp_buf[15];
          String strDimmer(buffer_);
          strDimmer=strDimmer.substring(7);
          dimmer=strDimmer.toInt();
          dimmer=constrain(dimmer, 0, 100);
          //updateNeopixel();
          sprintf (temp_buf, "%u", dimmer);
          client.publish(topicReplyDimmer, temp_buf);
        }
    
        //dimmer?
        else if (strcmp_P(buffer_, PSTR("dimmer?"))==0){
          char temp_buf[15];
          sprintf (temp_buf, "%u", dimmer);
          client.publish(topicReplyDimmer, temp_buf);
        }
        
        //color?
        else if (strcmp_P(buffer_, PSTR("color?"))==0){
          String colors="";
          char temp_buf[15];
          colors=String(red)+"."+String(green)+","+String(blue);
          colors.toCharArray(temp_buf,15);
          client.publish(topicReplyColor, temp_buf);
        }
        
        //pwm14
        else if (strstr(buffer_, "pwm14=")!=0){
          char strPWM14[6];
          strPWM14[2]=buffer_[8];
          strPWM14[1]=buffer_[7];
          strPWM14[0]=buffer_[6];
          int PWM14value=atoi(strPWM14);
          if (PWM14value>=100) PWM14value=100;
          int pwm14=map(PWM14value, 0, 100, 0, 1023);
         // Qpwm14=pwm14;
          analogWrite(14,pwm14); //write to output
          
        }
        //allOFF
        else if (strcmp_P(buffer_, PSTR("allOFF"))==0){
          digitalWrite(0,HIGH);
          digitalWrite(2,HIGH);
          digitalWrite(13,LOW);
          digitalWrite(15,LOW);
          
        }
        //allON
        else if (strcmp_P(buffer_, PSTR("allON"))==0){
          digitalWrite(0,LOW);
          digitalWrite(2,LOW);
          digitalWrite(13,HIGH);
          digitalWrite(15,HIGH);
          //
        }
        //ERROR
        else{
           client.publish(topicError, "command ERROR");
           Serial.println("[MQTT] command ERROR");                                       
        }
  }
}  

void publishA0()
{
  AI0=analogRead(17);
  char strAI0[10];
  sprintf (strAI0, "%u", AI0);
  client.publish(topicAI0, strAI0);
  Serial.print("[ANAG] ");
  Serial.println(AI0);
}

void publishUpTime(){
  char strUpTime[10];
  unsigned long upTime=millis()/60000;
  sprintf (strUpTime, "%u", upTime);
  client.publish(topicUpTime, strUpTime);
  Serial.print("[TIME] ");
  Serial.println(upTime); 
}

void publishState(){
  char estat[256];
  memset(estat,0,256);//esborra estat
  boolean I4=digitalRead(4);//comentar si BME280
  boolean I5=digitalRead(5);//comentar si BME280
  boolean I12=digitalRead(12);
  boolean I14=digitalRead(14);
  boolean Q0=digitalRead(0);
  boolean Q2=digitalRead(2);
  boolean Q13=digitalRead(13);
  boolean Q15=digitalRead(15);
  AI0=analogRead(17);
  unsigned long uT=millis()/60000;
  estatESP="{\"I4\":\""+ String(I4) + "\", ";
  estatESP=estatESP+"\"I5\":\""+ String(I5) + "\", ";
  estatESP=estatESP+"\"I12\":\""+ String(I12) + "\", ";
  estatESP=estatESP+"\"I14\":\""+ String(I14) + "\", ";
  estatESP=estatESP+"\"Q0\":\""+ String(Q0) + "\", ";
  estatESP=estatESP+"\"Q2\":\""+ String(Q2) + "\", ";
  estatESP=estatESP+"\"Q13\":\""+ String(Q13) + "\", ";
  estatESP=estatESP+"\"Q15\":\""+ String(Q15) + "\", ";
  estatESP=estatESP+"\"AI0\":\""+ String(AI0) + "\", ";
  estatESP=estatESP+"\"t\":\""+ String(temperatura) + "\", ";
  estatESP=estatESP+"\"h\":\""+ String(humedad) + "\", ";
  estatESP=estatESP+"\"p\":\""+ String(presion) + "\", ";
  //estatESP=estatESP+"\"pwm14\":\""+ String(Qpwm14) + "\", ";
  
  estatESP=estatESP+"\"uT\":\""+ String(uT) + "\"";
  estatESP=estatESP + "}";
  estatESP.toCharArray(estat,256);
  //Serial.println(estat);
  client.publish(topicState, estat);
}

void reconnecting() {
  // Loop until we're reconnected
  if (!client.connected()) {
    Serial.print("[MQTT] Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(thisNode))//per broker sense seguretat
    //if (client.connect(thisNode, userMQTT, passMQTT))//per broker amb ususari i password
    {
      Serial.print("connected to ");
      Serial.println(mqtt_server);
      // Once connected, publish an announcement...
      IPAddress ipLocal;
      ipLocal=WiFi.localIP();
      sprintf(strIP, "%d.%d.%d.%d",ipLocal[0],ipLocal[1],ipLocal[2],ipLocal[3]);
      String tempStr="";
      tempStr=String(thisNode)+"/IP";
      char temp[40];
      tempStr.toCharArray(temp,40);
      client.publish(temp, strIP);
      
      client.subscribe(INcommands);
      client.subscribe(setRelays);
      client.subscribe(topicAllNodes);
      
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying, but cheking pushbutton I4e
      //for (int i=0; i<25; i++){
      //    pushButtonI4();
      //   delay(200);  
      //}      
    }
  }
}


void pushButtonI4()
{
  estatI4=digitalRead(4);
  if (estatI4ant==0 && estatI4==1)
  {
    bool temp=digitalRead(15);
    if (temp){ digitalWrite(15,LOW); client.publish(replyQ15, "0");}
    if (!temp){ digitalWrite(15,HIGH);client.publish(replyQ15, "1");}
    estatI4ant=1;
  }
  if (estatI4ant==1 && estatI4==0)
  {
    estatI4ant=0;
  }
}


void pushButtons()
{
    /*
  estatI4=digitalRead(4);
  if (estatI4ant!=estatI4){
    if (estatI4==0) client.publish(replyI4, "0");
    if (estatI4==1) client.publish(replyI4, "1");
  estatI4ant=estatI4;  
  }
  */  
  
  estatI5=digitalRead(5);
  if (estatI5ant!=estatI5){
    if (estatI5==0) client.publish(replyI5, "0");
    if (estatI5==1) client.publish(replyI5, "1");
  estatI5ant=estatI5;  
  }

  estatI12=digitalRead(12);
  if (estatI12ant!=estatI12){
    if (estatI12==0) client.publish(replyI12, "0");
    if (estatI12==1) client.publish(replyI12, "1");
  estatI12ant=estatI12;  
  }
  estatI14=digitalRead(14);
  if (estatI14ant!=estatI14){
    if (estatI14==0) client.publish(replyI14, "0");
    if (estatI14==1) client.publish(replyI14, "1");
  estatI14ant=estatI14;  
  }
}

void publishHeartBeat(){
  char strHeartBeat[10];
  unsigned long heartBeat=millis()/60000;
  sprintf (strHeartBeat, "%u", heartBeat);
  client.publish(topicHearBeat, strHeartBeat); 
}

/*
void publishDHT()
{
  t=dht.readTemperature();
  h=dht.readHumidity();
  if (isnan(h) || isnan(t)) 
  {
    Serial.println("[DHT] Failed to read from DHT sensor!");
    t=0; h=0;
  }
  temperatura=int(t);
  humedad=int(h);
  sprintf (strTemperatura, "%u", temperatura);
  client.publish(topicDHTtemperature, strTemperatura);
  sprintf (strHumedad, "%u", humedad);
  client.publish(topicDHThumidity, strHumedad);
}
*/
/*
void publishDS18B20(){
  DS18B20.requestTemperatures(); 
  t = DS18B20.getTempCByIndex(0);
  temperatura=int(t);
  sprintf (strTemperatura, "%u", temperatura);
  client.publish(topicTemperature, strTemperatura);
}
*/
void alive()
{
  boolean kk=!digitalRead(16);
  digitalWrite(16,kk);
}


void checkMQTT()
{
    if (!client.connected() && WiFi.isConnected()){
  //if (!client.connected() && jw.connected()) {
    reconnecting();
  } 
}

/* 

void updateNeopixel()
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    pixels.setPixelColor(i, pixels.Color(red*dimmer/100,green*dimmer/100,blue*dimmer/100));
    pixels.show();
  }
}
*/

/*
void publishBME280() {
    
    t = bme.readTemperature();
    temperatura= int(t);
    sprintf (strTemperatura, "%u", temperatura);
    client.publish(topicBMEtemperature, strTemperatura);

    h = bme.readHumidity();
    humedad=int(h);
    sprintf (strHumedad, "%u", humedad);
    client.publish(topicBMEhumidity, strHumedad);

    p = bme.readPressure() / 100.0F;
    presion=int(p);
    sprintf (strPresion, "%u", presion);
    client.publish(topicBMEpressure, strPresion);
   
    //Serial.print("Pressure = ");

    //Serial.print(bme.readPressure() / 100.0F);
    //Serial.println(" hPa");

    //Serial.print("Approx. Altitude = ");
    //Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    //Serial.println(" m");
}

 */
