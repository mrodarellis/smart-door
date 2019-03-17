//slave/receiver  ESP-8266 from fingerprint/nfc/keypad open relay
int relayPin = 2; //D4

extern "C" {
#include <espnow.h>
}
#include <ESP8266WiFi.h>

#define CHANNEL 0

unsigned long timeout = millis();
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

// Init ESP Now with fallback
void InitESPNow() {
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
//   digitalWrite(relayPin, LOW);  //Set the pin to LOW (0V)
Serial.println("relay opened*********************************");
   pinMode(relayPin, OUTPUT);
   
    Serial.println("relay closed");
  delay(2000);
  Serial.println("ESPNow receive from fingerprint/nfc/keypad and opens relay  ");

  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP_STA);
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());

  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

// callback when data is recv from Master
void OnDataRecv(uint8_t *mac_addr, uint8_t *data, uint8_t len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);
 
   digitalWrite(relayPin, LOW); //Set the pin to HIGH (3.3V)
   Serial.println("relay opened*********************************");
   delay(15000);                  //Delay 5 seconds
  
  Serial.println("");
}

void loop() {
  if (millis() - timeout > 9000) {
    Serial.println("Waiting for data ...");
    timeout = millis();
    digitalWrite(relayPin, HIGH);  //Set the pin to LOW (0V)
    Serial.println("relay closed#################################");
 
  }
}
