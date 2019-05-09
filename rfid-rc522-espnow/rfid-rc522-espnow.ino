/*
Many thanks to nikxha from the ESP8266 forum
*/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include "MFRC522.h"
#include <Arduino.h>


struct __attribute__((packed)) SENSOR_DATA {
   char testdata[20];
} sensorData;
// uint8_t bs[sizeof(sensorData)];



extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

/* wiring the MFRC522 to ESP8266 (ESP-12)
RST     = GPIO5
SDA(SS) = GPIO4 
MOSI    = GPIO13
MISO    = GPIO12
SCK     = GPIO14
GND     = GND
3.3V    = 3.3V
*/

#define RST_PIN  5  // RST-PIN für RC522 - RFID - SPI - Modul GPIO5 
#define SS_PIN  4  // SDA-PIN für RC522 - RFID - SPI - Modul GPIO4 

const char *ssid =  "foundation";     // change according to your Network - cannot be longer than 32 characters!
const char *pass =  "towifi8aexeikodikopolidiskolo"; // change according to your Network

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
byte readCard[4];
byte checkCard[4] = {215, 153, 101, 69};

//************************************************************************
typedef struct esp_now_peer_info {
    u8 peer_addr[6];    /**< ESPNOW peer MAC address that is also the MAC address of station or softap */
    uint8_t channel;                        /**< Wi-Fi channel that peer uses to send/receive ESPNOW data. If the value is 0,
                                                 use the current channel which station or softap is on. Otherwise, it must be
                                                 set as the channel that station or softap is on. */
} esp_now_peer_info_t;

#define CHANNEL 0

//Mac Address for all slaves (broadcast)
// uint8_t PEER[] {0x59, 0x00, 0xfb, 0xa4, 0xae, 0x30}; // esp32 A
 //static  uint8_t mac[] =  {0x61, 0x53, 0xc5, 0x3a, 0x7d, 0x80} ; //pjonb
 //static  uint8_t mac[] =  {0x5E, 0xCF, 0x7F, 0xAB, 0xB4, 0xB5}; //8266 nodemcu amica 
// static  uint8_t mac[] =  {0x2E, 0x3A, 0xE8, 0x26, 0x3E, 0xCA}; //8266 FEMALE keypad
 //static  uint8_t mac[] =  {0x59, 0x00, 0xFB, 0xA4, 0xAE  , 0x30} ;
 //static uint8_t mac[] =  {0x30, 0xAE, 0xA4, 0xFB, 0x00  , 0x59} ;

uint8_t remoteMac2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 
uint8_t remoteMac1[] = {0x5E, 0xCF, 0x7F, 0xAB, 0xB4, 0xB5}; // relay
uint8_t remoteMac[] = {0x80, 0x7d, 0x3a, 0xc5, 0x53, 0x61}; // pjonb
uint8_t remoteMac3[] = {0x3A, 0x2B, 0x78, 0x04, 0xEF, 0x2A}; // relay B

uint8_t data = 1;
bool retry = true;
 static uint32_t ok = 0;
 static uint32_t fail = 0;

void printMacAddress(uint8_t* macaddre) {
  Serial.print("{");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    Serial.print(macaddre[i], HEX);
    if (i < 5) Serial.print(',');
  }
  Serial.println("};");
}

// Init ESP Now with fallback
void InitESPNow() {
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
}

void sendData() {
  // data++;
 /* int result = esp_now_send(remoteMac2, &data, sizeof(data));
  Serial.print("Send Command: ");
  if (result ==0) {
    Serial.println("Success " + String(result));
  } else {
    Serial.println("Failed " + String(result));
  }
  delay(100);*/
   uint8_t bs[sizeof(sensorData)];
  
  memcpy(bs, &sensorData, sizeof(sensorData));
  
  Serial.println();
   Serial.print("data to sent: --->  ");
  for (int i = 0; i < 4; i++)      { 
    Serial.print(sensorData.testdata[i],HEX); 
 
  }
     Serial.print("------");  
     for (int i = 4; i < 20; i++)       
    Serial.print(sensorData.testdata[i]);  
    Serial.println();
    
  int result = esp_now_send(remoteMac2, bs, sizeof(sensorData));
  Serial.print("Send Command: ");
  if (result ==0) {
    Serial.println("Success " + String(result));
  } else {
    Serial.println("Failed " + String(result));
  }
 delay(500);   // must set for wemos rc522
}

// callback when data is sent from Master to Slave
esp_now_send_cb_t OnDataSent(const uint8_t *mac_addr, u8 status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == 0 ? "Delivery Success   ********" : "Delivery Fail    ********");
}

//************************************************************************


void setup() {
  Serial.begin(115200);    // Initialize serial communications
  delay(250);
  Serial.println(F("Booting...."));
  
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  
 /* WiFi.begin(ssid, pass);
  
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (retries < 2)) {
    retries++;
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected"));
  } */

  //********************************************************************************
 Serial.println( );
    Serial.println( "start nfc rc522" );
    
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow rc522");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  // esp_now_register_send_cb(OnDataSent);
 
  int addStatus = esp_now_add_peer((u8*)remoteMac, ESP_NOW_ROLE_CONTROLLER, CHANNEL, NULL, 0);
  if (addStatus == 0) {
    // Pair success
    Serial.println("Pair success");
  } else {
    Serial.println("Pair failed");
  }
  for (int i = 0; i < 19; ++i)  sensorData.testdata[i] = '0';
   sensorData.testdata[15] = 'r';
   sensorData.testdata[16] = 'c';
   sensorData.testdata[17] = '5';
   sensorData.testdata[18] = '2';
   sensorData.testdata[19] = '2';
   int addStatus1 = esp_now_add_peer((u8*)remoteMac1, ESP_NOW_ROLE_CONTROLLER, CHANNEL, NULL, 0);
  if (addStatus1 == 0) {
    // Pair success
    Serial.println("Pair success");
  } else {
    Serial.println("Pair failed");
  }

   esp_now_register_send_cb([](uint8_t* macaddre, uint8_t status) {
    printMacAddress(macaddre);
   
    if (status == 0) {
      Serial.println("ESPNOW: ACK_OK");
      ok++;
      retry = false;
    }
    else {
      Serial.println("ESPNOW: SEND_FAILED");
      fail++;
    }
    Serial.printf("[SUCCESS] = %lu/%lu \r\n", ok, ok+fail);
  });



 //*********************************************************************************
  
  Serial.println(F("Ready!"));
  Serial.println(F("======================================================")); 
  Serial.println(F("Scan for Card and print UID:"));
}



void loop() { 
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(500);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
//  for (int i = 0; i < 19; ++i)  sensorData.testdata[i] = '0';
   Serial.println("Scanned PICC's UID:");
   for (int i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
//   Serial.print(checkCard[i], HEX);
    sensorData.testdata[i]=readCard[i];
       if ( sensorData.testdata[i]>=122)  sensorData.testdata[i]=sensorData.testdata[i]-128;
    if ( sensorData.testdata[i]<=48)  sensorData.testdata[i]=sensorData.testdata[i]+48;
    
  }
 
// Serial.println();
 
//  Serial.println();   
        
         for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac1[i];
         sendData();
         for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac[i];
         sendData();  
         for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac3[i];
         sendData();
  
/* String myString = String((char *)mfrc522.uid.uidByte);
 Serial.println("myString:");
  Serial.println(myString);*/
  
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buf, byte bufferSize) {
  Serial.println(bufferSize);
  for (byte i = 0; i < bufferSize; i++) {
  //  Serial.print(buf[i] < 0x10 ? " 0" : " ");
  //  Serial.print(buf[i], HEX);
  Serial.print(buf[i], HEX);
        }
        
         Serial.println();
   
}
