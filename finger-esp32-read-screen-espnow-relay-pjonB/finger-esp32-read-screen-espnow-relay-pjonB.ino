#include <Adafruit_Fingerprint.h>
 #include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
 SSD1306Wire  display(0x3c, 5, 4);
HardwareSerial mySerial(2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// *************************ESP NOW**********************************
#include <esp_now.h>
#include <WiFi.h>
#define CHANNEL 0

 esp_now_peer_info_t slave ;
 esp_now_peer_info_t slave1 ;
 esp_now_peer_info_t slave2 ;

//Mac Address for all slaves (broadcast)
// uint8_t PEER[] {0x59, 0x00, 0xfb, 0xa4, 0xae, 0x30}; // esp32 A

 //static  uint8_t mac[] =  {0x61, 0x53, 0xc5, 0x3a, 0x7d, 0x80} ;
 static  uint8_t mac[] =  {0x5E, 0xCF, 0x7F, 0xAB, 0xB4, 0xB5}; //8266 nodemcu amica relay
 static  uint8_t mac1[] =  {0x61, 0x53, 0xc5, 0x3a, 0x7d  , 0x80} ;  //pjonb 
 //static  uint8_t mac2[] =  {0x2E, 0x3A, 0xE8, 0x26, 0x3E, 0xCA}; //8266 FEMALE PINS work
 static  uint8_t mac2[] = {0x3A, 0x2B, 0x78, 0x04, 0xEF, 0x2A}; // relay B


 //static  uint8_t mac[] =  {0x59, 0x00, 0xFB, 0xA4, 0xAE  , 0x30} ;
 //static uint8_t mac[] =  {0x30, 0xAE, 0xA4, 0xFB, 0x00  , 0x59} ;

uint8_t data = 0;
const esp_now_peer_info_t *peer = &slave;
const esp_now_peer_info_t *peer1 = &slave1;
const esp_now_peer_info_t *peer2 = &slave2;
const uint8_t *peer_addr = slave.peer_addr;
const uint8_t *peer_addr1 = slave1.peer_addr;
const uint8_t *peer_addr2 = slave2.peer_addr;
int id;
struct __attribute__((packed)) SENSOR_DATA {
   char testdata[20];
} sensorData;
// uint8_t bs[sizeof(sensorData)];

// Init ESP Now with fallback
void InitESPNow() {
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
/*  delay(3500);
senddata();  */
}


// send data
bool senddata() {
  data++;
 
    
    sensorData.testdata[15]=id+48;
     sensorData.testdata[4] = 'f';
  sensorData.testdata[5] = 'i';
  sensorData.testdata[6] = 'g';
     sensorData.testdata[7] = 'e';
  sensorData.testdata[8] = 'r';
 Serial.println("data to send"); 
Serial.println(sensorData.testdata);  //Show data on Serial Monitor
Serial.print("Slave Status: ");
/*  esp_now_send(NULL, data, len);/* the demo will send to two devices which added by esp_now_add_peer() 
 esp_now_send(mac_addr, data, len); send to the specified mac_addr */ 
 uint8_t bs[sizeof(sensorData)];
  memcpy(bs, &sensorData, sizeof(sensorData));
  
    
  esp_err_t result = esp_now_send(peer_addr, bs, sizeof(sensorData));
 
/*  const uint8_t broadcast[] = {0x59, 0x00, 0xfb, 0xa4, 0xae  , 0x30};
  esp_err_t result = esp_now_send(broadcast, &data, sizeof(data));
  esp_err_t result2 = esp_now_send(nullptr, &data, sizeof(data)); */
  
result_t( result);
 //  Serial.print("Sending: "); Serial.println(*bs);

  esp_err_t result1 = esp_now_send(slave1.peer_addr, bs,  sizeof(sensorData));  
  result_t( result1);
   esp_err_t result2 = esp_now_send(slave2.peer_addr, bs,  sizeof(sensorData));  
  result_t( result2);
 }
//*****************************************************************************

void result_t(esp_err_t result)
{
 Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}

void addstatus_p(esp_err_t addStatus)
{
 if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW Not Init");
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
       } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
       } else {
        Serial.println("Not sure what happened");
      }
}
 const char* test[] = { "Hello"};
 
void setup()
{ 
  display.init();
  display.setContrast(255);
    display.setLogBuffer(5, 30);
  display.clear(); 
    // Print to the screen
   //  display.println(finger.confidence);
    display.println(test[0]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display(); 
  
  Serial.begin(115200);
  Serial.println("fingerprint  ESPNow/Send to relay and pjonb");
//  delay(1000);
  Serial.println("\n\nAdafruit finger detect test");

  mySerial.begin(57600, SERIAL_8N1, 14, 16);
  // set the data rate for the sensor serial port
  //finger.begin(57600);
 
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");

 WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  Serial.println("ESPNow/Send to relay and pjonb");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);
  Serial.print("send to : "); 
   for (int i = 0; i < 6; ++i)  { 
    slave.peer_addr[i] = (uint8_t)mac[i];
    Serial.println(slave.peer_addr[i],HEX);
     }
   
    slave.channel = CHANNEL; // pick a channel
    slave.encrypt = 0; // no encryption
for (int i = 0; i < 19; ++i)  sensorData.testdata[i] = '0';

    esp_err_t addStatus = esp_now_add_peer(peer);
  addstatus_p(addStatus);
  
   for (int i = 0; i < 6; ++i)  { 
     slave1.peer_addr[i] = (uint8_t)mac1[i];
    Serial.println(slave1.peer_addr[i],HEX);
    }
   esp_err_t addStatus1 = esp_now_add_peer(peer1);
  addstatus_p(addStatus1);
  
    for (int i = 0; i < 6; ++i)  { 
     slave2.peer_addr[i] = (uint8_t)mac2[i];
    Serial.println(slave2.peer_addr[i],HEX);
    }
   esp_err_t addStatus2 = esp_now_add_peer(peer2);
  addstatus_p(addStatus2);
}

void loop() {// getFingerprintID();
  getFingerprintIDez();
  delay(50);            //don't ned to run this at full speed.
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  
  Serial.println("data to sent");
  Serial.println(sensorData.testdata);   
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
 
//  itoa(finger.fingerID, id, 10);
// sensorData.testdata[1]=finger.fingerID;
  id=finger.fingerID;
//  Serial.println(id);
// Serial.println("data to send"); 
// Serial.println(sensorData.testdata);  //Show data on Serial Monitor

//  display.setLogBuffer(5, 30);
  display.clear(); 
    // Print to the screen
    display.println(finger.confidence);
    display.println(test[0]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display(); 
        
    senddata(); 
  return finger.fingerID; 
}
