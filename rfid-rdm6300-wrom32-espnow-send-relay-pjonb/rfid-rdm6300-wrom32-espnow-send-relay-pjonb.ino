
// wroom 32
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <esp_now.h>
#include <WiFi.h>
#define CHANNEL 0

 esp_now_peer_info_t slave ;
  esp_now_peer_info_t slave1 ; 
  
 static  uint8_t mac1[] =  {0x61, 0x53, 0xc5, 0x3a, 0x7d  , 0x80} ;//pjonb 
static  uint8_t mac[] =  {0x5E, 0xCF, 0x7F, 0xAB, 0xB4, 0xB5}; //8266 nodemcu amica relay
///////////////////////////////////////////////////////////////////////////////////
uint8_t data = 0;
uint8_t data1[12];

const esp_now_peer_info_t *peer = &slave;
const esp_now_peer_info_t *peer1 = &slave1;
const uint8_t *peer_addr = slave.peer_addr;
const uint8_t *peer_addr1 = slave.peer_addr;

struct __attribute__((packed)) SENSOR_DATA {
   char testdata[20];
} sensorData;
// uint8_t bs[sizeof(sensorData)];

HardwareSerial Serial1(1);

#define RX_PIN 16
#define TX_PIN 17

int  serIn;             // var that will hold the bytes-in read from the serialBuffer
int serInString[100];  // array that will hold the different bytes  100=100characters;
                        // -> you must state how long the array will be else it won't work.
int  serInIndx  = 0;    // index of serInString[] in which to insert the next incoming byte
int  serOutIndx = 0;    // index of the outgoing serInString[] array;
String inString = "";    // string to hold input

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
//  delay(3500);
// senddata(); 
}


// send data
bool senddata() {
  data++;
 
    Serial.print("Slave Status: ");
     
     
  Serial.print("Sending: "); Serial.println(inString);
sensorData.testdata[15] = 'n';
  sensorData.testdata[16] = 'f';
  sensorData.testdata[17] = 'c';
   uint8_t bs[sizeof(sensorData)];
  memcpy(bs, &sensorData, sizeof(sensorData));
  
    
  esp_err_t result = esp_now_send(peer_addr, bs, sizeof(sensorData));
 

/*   esp_err_t result = esp_now_send(peer_addr, data1, sizeof(data1)); 
  esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));  */
  
/*  const uint8_t broadcast[] = {0x59, 0x00, 0xfb, 0xa4, 0xae  , 0x30};
  esp_err_t result = esp_now_send(broadcast, &data, sizeof(data));

  esp_err_t result2 = esp_now_send(nullptr, &data, sizeof(data)); */
  
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

 Serial.print("Sending: "); Serial.println(data);

 
  esp_err_t result1 = esp_now_send(slave1.peer_addr, bs,  sizeof(sensorData));  
  
  
/*  const uint8_t broadcast[] = {0x59, 0x00, 0xfb, 0xa4, 0xae  , 0x30};
  esp_err_t result = esp_now_send(broadcast, &data, sizeof(data));

  esp_err_t result2 = esp_now_send(nullptr, &data, sizeof(data)); */
  
  Serial.print("Send Status: ");
  if (result1 == ESP_OK) {
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

//*****************************************************************************


void setup(){
  
  Serial.begin(115200); // debug serial monitor
  Serial1.begin(9600,SERIAL_8N1,RX_PIN,TX_PIN); // connect card reader to TX_PIN and RX_PIN

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  Serial.println("rdm6300 ESPNow/Send to relay and pjonb");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);
  
   for (int i = 0; i < 6; ++i)  { 
    slave.peer_addr[i] = (uint8_t)mac[i];
    Serial.println(slave.peer_addr[i],HEX);}
   
    slave.channel = CHANNEL; // pick a channel
    slave.encrypt = 0; // no encryption

    esp_err_t addStatus = esp_now_add_peer(peer);
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
      Serial.print("send to : "); 
   for (int i = 0; i < 6; ++i)  { 
   
    slave1.peer_addr[i] = (uint8_t)mac1[i];
    Serial.println(slave1.peer_addr[i],HEX);
    }
   esp_err_t addStatus1 = esp_now_add_peer(peer1);
   if (addStatus1 == ESP_OK) {
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
 for (int i = 0; i < 19; ++i)  sensorData.testdata[i] = '0';
 if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);

  
  Serial.printf(" initialized Second UART on pins: Rx=%d, Tx=%d\n",RX_PIN,TX_PIN);
  Serial.println(" START type something : "); 
}


void loop(){
  if(Serial.available()) {
    readSerialString();
//    printSerialString();
//    Serial1.write(serInString);         //Push it through port 1 to Device B
//    Serial.write(serInString);          //Show data on Serial Monitor
    Serial.print(inString);               //Show data on Serial Monitor
    appendFile(SD, "/hello.txt", inString); //Write data on SD
    Serial.println();
    readFile(SD, "/hello.txt");
    Serial.flush();                       //erase any data on serial port after operation 
  } 
     if (Serial1.available())            //If data is received through Serial 1 from Device B...
    {    
       readSerial1String ();
       Serial.print(inString);           //Show data on Serial Monitor
       appendFile(SD, "/hello.txt", inString); //Write data on SD
       readFile(SD, "/hello.txt");
       Serial.flush();                   //Flush port
       senddata()   ;
       delay(500);
    }
  }

  void readSerial1String () {
    int sb1;   
        //Serial.print("reading Serial1 String: ");     //optional confirmation
       while (Serial1.available()){ 
          sb1 = Serial1.read();             
          serInString[serInIndx] = sb1;
           inString += (char)sb1;
          
           sensorData.testdata[serInIndx] = (char)sb1;

          serInIndx++;
          //serialWrite(sb);                        //optional confirmation
       }
       //Serial.println();
       memcpy(data1, &inString, sizeof(inString));
       delay(500);
       serOutIndx = 0;
       serInIndx  = 0;
   
}
     
    void readSerialString () {
    int sb;   
         //Serial.print("reading Serial String: ");     //optional confirmation
       while (Serial.available()){ 
          sb = Serial.read();             
          serInString[serInIndx] = sb;
           inString += (char)sb;

             sensorData.testdata[serInIndx] = (char)sb;
           
          serInIndx++;
          //serialWrite(sb);                        //optional confirmation
       }
       //Serial.println();
       memcpy(data1, &inString, sizeof(inString));
       
       serOutIndx = 0;
       serInIndx  = 0;
   
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

//print the string all in one time
//this func as well uses global variables
void printSerialString() {
   if( serInIndx > 0) {
      Serial.println("Arduino memorized that you said: ");     
      //loop through all bytes in the array and print them out
      for(serOutIndx=0; serOutIndx < serInIndx; serOutIndx++) {
          Serial.print( serInString[serOutIndx] );    //print out the byte at the specified index
          //serInString[serOutIndx] = "";            //optional: flush out the content
      }        
      //reset all the functions to be able to fill the string back with content
      serOutIndx = 0;
      serInIndx  = 0;
      Serial.println();
   }
}
   void appendFile(fs::FS &fs, const char * path,  String   message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);
    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

 
  
