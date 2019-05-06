#include <PJON.h>
// #include <analogWrite.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(44);


///////////////////////////////////////////////

#include <WiFi.h>
#define CHANNEL 0
#include <Arduino.h>
#include <ESPinfluxdb.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
 #elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiMulti.h>
 #endif

#include "Gsender_32.h"
#include <WiFiClientSecure.h>


#pragma region Globals

//const char* ssid = "put your ssid";                // WIFI network name
//const char* password = "put your wifi password";           // WIFI network password
const char* ssid = "foundation";                // WIFI network name
const char* password = "towifi8aexeikodikopolidiskolo";           // WIFI network password


uint8_t connection_state = 0;           // Connected to WIFI or not
uint16_t reconnect_interval = 5000;    // If not connected wait time to try again



#pragma endregion Globals

// String address[] = {"email1", "email2"};
String address[] = {"mrodarellis@gmail.com"};


const char *INFLUXDB_HOST = "192.168.1.103";
const uint16_t INFLUXDB_PORT = 8086;

const char *DATABASE = "test1";
const char *DB_USER = "mike";
const char *DB_PASSWORD = "12345678";

Influxdb influxdb(INFLUXDB_HOST, INFLUXDB_PORT);

struct __attribute__((packed)) SENSOR_DATA {
   char testdata[20];
} sensorData;

int green = 18; //select the pin for the green LED
int blue = 23; // select the pin for the  blue LED
int red = 19; // select the pin for the red LED

int val;

/************* connect wifi
/*
 * ESP is a pain to ensure statup
 * happens on a clean slate.
 */
/* void WiFiReset() {
    WiFi.persistent(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    Serial.print("reset wifi");
}

uint8_t WiFiConnect(const char* ssID, const char* nPassword)
{
  static uint16_t attempt = 0;
  Serial.print("Connecting to ");
  Serial.println(ssID);
  WiFi.begin(ssID, nPassword);

  uint8_t i = 0;
//  while(WiFi.status() != WL_CONNECTED && i++ < 50) {

while(WiFi.begin(ssID, nPassword) != WL_CONNECTED && i++ < 30) {
    delay(400);
     Serial.print(WiFi.status());
//    Serial.print(".");
  } 
  ++attempt;
  Serial.println("");
  if(i == 31) {
    Serial.print(F("Connection: TIMEOUT on attempt: "));
    Serial.println(attempt);
    if(attempt % 1 == 0){
      Serial.println(F("Check if access point available or SSID and Password are correct\r\n"));
       ESP.restart();}
    return false;
  } 
  Serial.println(F("Connection: ESTABLISHED"));
  Serial.print(F("Got IP address: "));
  Serial.println(WiFi.localIP());
  return true;
}

void Awaits(uint16_t interval)
{
  uint32_t ts = millis();
  while(!connection_state){
    delay(50);
    if(!connection_state && millis() > (ts + interval)){
      connection_state = WiFiConnect(ssid, password);
      ts = millis();
    }
  }
}
//***************** end connect wifi

///******************************************************
*/

void setup() {
  Serial.begin(115200);

  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
  val = 155; 

  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  bus.set_error(error_handler);
  bus.set_receiver(receiver_function);
  bus.strategy.set_pin(5); // ESP8266 GPIO 5 or pin D1
  bus.begin();
  bus.send(45, "ok!", 3);

////////////////////////////////////////////////////////////////////
 Serial.println();
  Serial.println("pjon-influxdb-server");
  
  
Serial.println();
    Serial.println();
    Serial.print("Mac Address in Station: "); 
  Serial.println(WiFi.macAddress());
//    Serial.print("Connecting to ");
    Serial.println(ssid);
//    WiFi.setHostname("hostA"); 
     
/*        WiFiMulti.addAP(ssid, password);
        while (WiFiMulti.run() != WL_CONNECTED) {
                delay(1500);
                Serial.print(".");
        } 
//        WiFi.disconnect();
*/
//********old connect wifi********************
int retry = 0;
WiFi.persistent(false);
WiFi.disconnect();
// WiFi.mode(WIFI_AP_STA);
// WiFi.mode(WIFI_AP);
delay(50);
/*
  WiFi.begin(ssid, password, CHANNEL);
   while (WiFi.status() != WL_CONNECTED) {
    retry++;
        delay(500);
        Serial.print(WiFi.status());
        if (retry == 100) // or Simply Restart
         ESP.restart();
      
    }
*/
    Serial.println("Ready");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
// ******************************************/


// connect wifi
/* WiFiReset();
  connection_state = WiFiConnect(ssid, password);

  if(!connection_state) {         // if not connected to WIFI
    Awaits(reconnect_interval);         // constantly trying to connect
    
  }
 */ 
// connect db
//     influxdb.opendb(DATABASE, DB_USER, DB_PASSWORD);
 while (influxdb.opendb(DATABASE, DB_USER, DB_PASSWORD)!=DB_SUCCESS) {
                Serial.println("Open database failed");
                delay(100);
        } 
        Serial.println("Open database ok!!!!");
    delay(100);

//////////////////////////////////////////////////////////////////////////////////


  
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(bus.packets[data].content[0], DEC);
    Serial.println(" is lost.");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data, DEC);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher PJON_MAX_PACKETS if necessary.");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */

   if(!bus.update()) {// If all packets are delivered, send another
     
      Serial.print("pjon b send -->");
//      for (int i=0 ; i<length ; i++) Serial.print(payload[i],DEC);
//       memcpy(&sensorData, payload, sizeof(sensorData));

        memcpy(&sensorData, payload, length);
// Serial.println(sensorData.testdata);
//****************************************************************************************************
 
  Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
  String subject = "ESP32 Smart Do     or!";
  if (gsender->Subject(subject)->Send(2, address, sensorData.testdata))
  {
    Serial.println("Message send.");
  }
  else
  {
    Serial.print("Error sending message: ");
    Serial.println(gsender->getError());
  }  
//****************************************************************************************************
    Serial.println();  
    
  
  
  Serial.print("Last Packet Recv Data: "); Serial.println(sensorData.testdata);
      bus.reply("B", 1);
      Serial.println("reply to b");
/*    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);*/
    // delay(50);
  }

     if (sensorData.testdata[5] == 'k') { //keypad
                                         
                                         digitalWrite(green, HIGH); // turn the LED off by making the voltage LOW
                                         delay(1000); // wait for a second
                                         }
     if (sensorData.testdata[15] == 'n') { //nfc
                                          digitalWrite(red, HIGH); // turn the LED off by making the voltage LOW
                                          delay(1000); // wait for a second
                                         }    
     if (sensorData.testdata[15] == 'r') { //nfc 522
                                          digitalWrite(red, HIGH); // turn the LED off by making the voltage LOW
                                          delay(1000); // wait for a second
                                         }    
      if (sensorData.testdata[5] == 'i') { //finger
                                          digitalWrite(blue, HIGH); // turn the LED off by making the voltage LOW
                                          delay(1000); // wait for a second
                                         }  
                                         delay(50);   

    digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
                                                                                         
/*  if((char)payload[0] == 'o') {
    if(!bus.update()) // If all packets are delivered, send another
      bus.reply("B", 1);
      Serial.println("reply to a");
/*    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);*/
 //   delay(50);

    /////////////////////////////influxdb///////////////////////////////
  

        dbMeasurement row("smartdoor"); //node & val
        
     
         row.addTag("node", sensorData.testdata); // Add pin tag string
//        row.addField("val", payload[0]); // Add value field ascii value
            row.addField("val", 0); // Add value field ascii value
//  influxdb.write(row) ;
/********************************write db***************************************************  
/*         Serial.println(influxdb.write(row) == DB_SUCCESS ? "Object write success"
                       : "Writing failed"); */ 

                       
               row.empty();        
  //                    ESP.restart();
 /*       influxdb.write(row);
       Serial.println("Object write success");*/
/* Empty field object.
 //       row.empty();
 // Querying Data
        Serial.println("Querying data ........");
// https://docs.influxdata.com/influxdb/v1.5/query_language/
//        String influxQL = "select * from smartdoor order by time desc";
/*        if (influxdb.query(influxQL) != DB_SUCCESS)
               Serial.println("Query Failed");  */
/*    influxdb.query(influxQL);
/*        delay(50);
//####################################################################*/
  
};

void loop() {
  bus.receive(5000);
  bus.update();
};
