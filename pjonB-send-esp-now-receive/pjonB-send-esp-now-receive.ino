#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

struct __attribute__((packed)) SENSOR_DATA {
   char testdata[40];
} sensorData;

// uint8_t bs[sizeof(sensorData)];

int  serIn;             // var that will hold the bytes-in read from the serialBuffer
int serInString[100];  // array that will hold the different bytes  100=100characters;
                        // -> you must state how long the array will be else it won't work.
// int  serInIndx  = 0;    // index of serInString[] in which to insert the next incoming byte
// int  serOutIndx = 0;    // index of the outgoing serInString[] array;
// String inString = "1234567890abcdefgigk";    // string to hold input
// char   inchar[40]= "1234567890abcdefgijk" ; 
char *inchar;
/////////////////////////////////////////////////////////////////////////////////
#include <esp_now.h>
#include <WiFi.h>
#define CHANNEL 0

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

// config AP SSID
void configDeviceAP() {
  char* SSID = "Slave";
  bool result = WiFi.softAP(SSID, nullptr, CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

//////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
Serial.println("ESPNow receive Pjon send Example");
////////////////////////////////////////////////////////////////
 Serial.println("ESPNow/Basic/Slave Example");
  //Set device in STATION mode to begin with  
 //  WiFi.mode(WIFI_AP_STA);
 //  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  Serial.print("Mac Address in Station: "); 
  Serial.println(WiFi.macAddress());
  
// configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
  /////////////////////////////////////////////////////////////

//  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  bus.set_error(error_handler);
  bus.set_receiver(receiver_function);
  bus.strategy.set_pin(5); // ESP8266 GPIO 5 or pin D1
  bus.begin();
// bus.send(44, "B", 1);
  
};

// callback when data is recv from Master
// void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
           Serial.println("");
            memcpy(&sensorData, data, sizeof(sensorData));
// Serial.println(sensorData.testdata);
    Serial.println();  
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(sensorData.testdata);
  delay(50);
   bus.send(44, (const char*) sensorData.testdata, sizeof(sensorData.testdata));
//  bus.send(44, (const char*) data, sizeof(data));

}

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
  if((char)payload[0] == 'B') {
  //  if(!bus.update()) senddata(); 
    delay(50);
    Serial.println("reply to a"); 
  //  senddata();
 // readdata();
                               }// If all packets are delivered, send another
   /* bus.reply("B", 1);
    Serial.println("reply to a");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);*/
 };


void senddata()
{
/*        Serial.println(inchar);
//        memcpy(inchar, &inString, sizeof(inString));
   
        Serial.println((char*)inchar);
        const char  *constinchar = inchar;
//        bus.send(44, constinchar, sizeof(inchar));*/
};

void loop() {
  bus.receive(5000);
  bus.update();
};

void readdata() 
{    
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;
// inString = "";   
static byte ndx = 0;
char endMarker = '\n';
char rc;

 Serial.print("reading Serial String: ");    
  while (Serial.available() > 0 && newData == false) {
 rc = Serial.read();
 if (rc != endMarker) {
 receivedChars[ndx] = rc;
 serInString[ndx] = rc;
// inString += rc;
 inchar[ndx] =  rc;
 sensorData.testdata[ndx] = rc;
 ndx++;
 if (ndx >= numChars) {
 ndx = numChars - 1;
 } } else {
 receivedChars[ndx] = '\0'; // terminate the string
 ndx = 0;
 newData = true;
  senddata();
 } } }
