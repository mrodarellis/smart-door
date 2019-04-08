//master/sender ESP-8266

//#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <Ticker.h>

#include <Wire.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#define I2CADDR 0x27
struct __attribute__((packed)) SENSOR_DATA {
   char testdata[20];
} sensorData;
// uint8_t bs[sizeof(sensorData)];



extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','4'},
  {'5','6','7','8'},
  {'9','A','B','C'},
  {'*','0','#','&'}
};

// Digitran keypad, bit numbers of PCF8574 i/o port
byte rowPins[ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad

Keypad_I2C kpd( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574 );


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
  
    
  int result = esp_now_send(remoteMac2, bs, sizeof(sensorData));
  Serial.print("Send Command: ");
  if (result ==0) {
    Serial.println("Success " + String(result));
  } else {
    Serial.println("Failed " + String(result));
  }
 
}

// callback when data is sent from Master to Slave
esp_now_send_cb_t OnDataSent(const uint8_t *mac_addr, u8 status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == 0 ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  
   Wire.begin( );
    kpd.begin( makeKeymap(keys) );
    Serial.begin(115200);
  //   ESP.wdtDisable();  // turns off watchdog?
     Serial.println( );
    Serial.println( "start keypad 4x4 espnow pjonb Esp-now-sender-i2ckeypad4x4-password" );
    
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow keypad 4x4");
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
  sensorData.testdata[5] = 'k';
  sensorData.testdata[6] = 'b';
  sensorData.testdata[7] = 'r';
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

}

 char getnewkey() {
  char key=0;
  while (key == 0) {
           key = kpd.getKey();
//          ESP.wdtFeed();
                   }  
if (key) return(key);
  }
 
void loop() {
   char key = kpd.getKey();
    
    if (key){
    Serial.println(key);
    data = char(key);
    Serial.println(data);
    if (key=='A') {
      Serial.println("ok A");
      data = char(key);
      sensorData.testdata[1] = char(key);
       Serial.print("----->");
              Serial.println(data); 
         
                
          key = getnewkey();
//          ESP.wdtFeed();
                                 
               if (key=='*') {
               Serial.println("ok *");
               data = char(key);
               sensorData.testdata[2] = char(key);
               Serial.print("----->");
               Serial.println(data);     
           
         for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac1[i];
         sendData();
         for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac[i];
         sendData(); 
               }
                        }
                       }
   
     }           
             

