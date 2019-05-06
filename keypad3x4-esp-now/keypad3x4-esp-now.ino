
// Include Libraries
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "Keypad.h"


// Pin Definitions
#define KEYPADMEM3X4_PIN_ROW1	2
#define KEYPADMEM3X4_PIN_ROW2	14
#define KEYPADMEM3X4_PIN_ROW3	12
#define KEYPADMEM3X4_PIN_ROW4	13
#define KEYPADMEM3X4_PIN_COL1	5
#define KEYPADMEM3X4_PIN_COL2	4
#define KEYPADMEM3X4_PIN_COL3	0

struct __attribute__((packed)) SENSOR_DATA {
   char testdata[20];
} sensorData;
// uint8_t bs[sizeof(sensorData)];



extern "C" {
  #include <espnow.h>
  #include <user_interface.h>
}

// Global variables and defines
//Use this 2D array to map the keys as you desire
char keypadmem3x4keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
// object initialization
Keypad keypadmem3x4(KEYPADMEM3X4_PIN_COL1,KEYPADMEM3X4_PIN_COL2,KEYPADMEM3X4_PIN_COL3,KEYPADMEM3X4_PIN_ROW1,KEYPADMEM3X4_PIN_ROW2,KEYPADMEM3X4_PIN_ROW3,KEYPADMEM3X4_PIN_ROW4);


char key1=' ',key2=' ';

//****************************************************

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
bool keyok = false;

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
 delay(100);
}

// callback when data is sent from Master to Slave
esp_now_send_cb_t OnDataSent(const uint8_t *mac_addr, u8 status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == 0 ? "Delivery Success" : "Delivery Fail");
}


// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() 
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(115200);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    Serial.println();
    Serial.println("start");
    
    //Initialize the keypad with selected key map
    keypadmem3x4.begin(keypadmem3x4keys);
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
  sensorData.testdata[8] = '3';
  sensorData.testdata[9] = 'x';
  sensorData.testdata[10] = '4';
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

 char read_key() {
// Membrane 3x4 Matrix Keypad - Test Code
//  Read keypad
//    char key = keypadmem3x4.getKey();

    char key = keypadmem3x4.getKey();
 
    if (key)    Serial.println(key);
    return(key);
     
           
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void checkkey() 
{
      key1=read_key();
      if (key1=='5' ) {Serial.println("ok key1 5 ");
      
       for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac1[i];
         sendData();
         for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac[i];
         sendData(); 
         for (int i = 0; i < 6; i++)  remoteMac2[i]=remoteMac3[i];
         sendData();            
      }              
     }

void loop() {
  checkkey();
  delay(200);
}
                     
    





