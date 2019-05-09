#include <PJON.h>
// #include <analogWrite.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(44);


///////////////////////////////////////////////


#define CHANNEL 0
// #include <Arduino.h>
// #include <ESPinfluxdb.h>



#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiMulti.h>
#endif


#include <InfluxDb.h>
ESP8266WiFiMulti WiFiMulti;
#define INFLUXDB_HOST "192.168.1.106"
Influxdb influx(INFLUXDB_HOST);


// #include "Gsender_32.h"
#include <ESP8266SMTP.h>

#include <WiFiClientSecure.h>
#pragma region Globals

//const char* ssid = "put your ssid";                // WIFI network name
//const char* password = "put your wifi password";           // WIFI network password
const char* ssid = "..........";                // WIFI network name
const char* password = "....................";           // WIFI network password


uint8_t connection_state = 0;           // Connected to WIFI or not
uint16_t reconnect_interval = 5000;    // If not connected wait time to try again



#pragma endregion Globals


// String address[] = {"email1", "email2"};
String address[] = {"mrodarellis@gmail.com"};


/* const char *INFLUXDB_HOST = "192.168.1.103";
const uint16_t INFLUXDB_PORT = 8086;

const char *DATABASE = "test1";
const char *DB_USER = "mike";
const char *DB_PASSWORD = "12345678";

Influxdb influxdb(INFLUXDB_HOST, INFLUXDB_PORT); */

struct __attribute__((packed)) SENSOR_DATA {
   char testdata[20];
} sensorData;

int green = 18; //select the pin for the green LED
int blue = 23; // select the pin for the  blue LED
int red = 19; // select the pin for the red LED

int val;

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
// bus.strategy.set_pin(15); // ESP8266 GPIO 5 or pin D1
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
     

int retry = 0;
WiFi.persistent(false);
WiFi.disconnect();
// WiFi.mode(WIFI_AP_STA);
// WiFi.mode(WIFI_AP);
// delay(50);
  WiFi.begin(ssid, password, CHANNEL);
   while (WiFi.status() != WL_CONNECTED) {
    retry++;
        delay(500);
        Serial.print(WiFi.status());
        if (retry == 100) // or Simply Restart
         ESP.restart();
      
    }

    Serial.println("Ready");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());


influx.setDb("test1");
  
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
//*******************************************************
 
 
SMTP.setEmail("mrodarellis@gmail.com")
    .setPassword("qwerty&764")
    .Subject("smart door 2019")
    .setFrom("smart door 2019")
    .setForGmail();           // simply sets port to 465 and setServer("smtp.gmail.com");           
                            
                             //  if(SMTP.Send("Recipient Email, use comma for multiple recipients", message)) {
    if(SMTP.Send("mrodarellis@gmail.com", sensorData.testdata)) {
    Serial.println(F("Message sent"));
  } else {
    Serial.print(F("Error sending message: "));
    Serial.println(SMTP.getError());
  } 
  
//*****************************************************
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
                                    //     delay(1000); // wait for a second
                                         }
     if (sensorData.testdata[15] == 'n') { //nfc
                                          digitalWrite(red, HIGH); // turn the LED off by making the voltage LOW
                                     //     delay(1000); // wait for a second
                                         }    
     if (sensorData.testdata[15] == 'r') { //nfc 522
                                          digitalWrite(red, HIGH); // turn the LED off by making the voltage LOW
                                   //       delay(1000); // wait for a second
                                         }    
      if (sensorData.testdata[5] == 'i') { //finger
                                          digitalWrite(blue, HIGH); // turn the LED off by making the voltage LOW
                                  //        delay(1000); // wait for a second
                                         }  
                                         delay(50);   

    digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
                                                                                         
    /////////////////////////////influxdb///////////////////////////////
  
  InfluxData row("smartdoor");
  row.addTag("node", sensorData.testdata);
    row.addValue("val", 0);

  influx.write(row);
// delay(5000);

  
};

void loop() {
  bus.receive(50000);
  bus.update();
};
