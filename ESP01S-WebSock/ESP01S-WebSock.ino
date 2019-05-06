/* 
 1.PC connect the wifi : smartdoor   
              password :123456789
 2.Browser open: http://192.168.4.1/
  
 3.click the on or off button
  */


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define USE_SERIAL Serial

static const char ssid[] = "smartdoor";
static const char password[] = "12345678";
MDNSResponder mdns;

static void writeLED(bool);

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>ESP8266 WebSocket Demo</title>
<style>
"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }"
</style>
<script>
var websock;
function start() {
  websock = new WebSocket('ws://' + window.location.hostname + ':81/');
  websock.onopen = function(evt) { console.log('websock open'); };
  websock.onclose = function(evt) { console.log('websock close'); };
  websock.onerror = function(evt) { console.log(evt); };
  websock.onmessage = function(evt) {
    console.log(evt);
    var e = document.getElementById('ledstatus');
    if (evt.data === 'ledon') {
      e.style.color = 'red';
    }
    else if (evt.data === 'ledoff') {
      e.style.color = 'black';
    }
    else {
      console.log('unknown event');
    }
  };
}
function buttonclick(e) {
  websock.send(e.id);
}
</script>
</head>
<body onload="javascript:start();">
<h1>smart door 2019</h1>
<div id="ledstatus"><b>Door</b></div>
<button id="ledon"  type="button" onclick="buttonclick(this);">open</button> 
<button id="ledoff" type="button" onclick="buttonclick(this);">close</button>
</body>
</html>
)rawliteral";

// GPIO#0 is for Adafruit ESP8266 HUZZAH board. Your board LED might be on 13.
const int LEDPIN = 0;
// Current LED status
bool LEDStatus;

// Commands sent through Web Socket
const char LEDON[] = "ledon";
const char LEDOFF[] = "ledoff";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  USE_SERIAL.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch (type) {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[%u] Disconnected!\r\n", num);
    break;
  case WStype_CONNECTED:
  {
               IPAddress ip = webSocket.remoteIP(num);
               USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
               // Send the current LED status
               if (LEDStatus) {
                 webSocket.sendTXT(num, LEDON, strlen(LEDON));
               }
               else {
                 webSocket.sendTXT(num, LEDOFF, strlen(LEDOFF));
               }
  }
    break;
  case WStype_TEXT:
    USE_SERIAL.printf("[%u] get Text: %s\r\n", num, payload);

    if (strcmp(LEDON, (const char *)payload) == 0) {
      writeLED(true);
    }
    else if (strcmp(LEDOFF, (const char *)payload) == 0) {
      writeLED(false);
    }
    else {
      USE_SERIAL.println("Unknown command");
    }
    // send data to all connected clients
    webSocket.broadcastTXT(payload, length);
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[%u] get binary length: %u\r\n", num, length);
    hexdump(payload, length);

    // echo data back to browser
    webSocket.sendBIN(num, payload, length);
    break;
  default:
    USE_SERIAL.printf("Invalid WStype [%d]\r\n", type);
    break;
  }
}

void handleRoot()
{
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

static void writeLED(bool LEDon)
{
  LEDStatus = LEDon;
  // Note inverted logic for Adafruit HUZZAH board
  if (LEDon) {
    digitalWrite(LEDPIN, 0);
  }
  else {
    digitalWrite(LEDPIN, 1);
  }
}

void setup()
{
  pinMode(LEDPIN, OUTPUT);
  writeLED(false);

  USE_SERIAL.begin(115200);

  //Serial.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\r\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

//  WiFiMulti.addAP(ssid, password);
//
//  while (WiFiMulti.run() != WL_CONNECTED) {
//    Serial.print(".");
//    delay(100);
//  }

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  USE_SERIAL.print("AP IP address: ");
  USE_SERIAL.println(myIP);

  USE_SERIAL.println("");
  USE_SERIAL.print("Connected to ");
  USE_SERIAL.println(ssid);
  USE_SERIAL.print("IP address: ");
  USE_SERIAL.println(WiFi.localIP());

  if (mdns.begin("espWebSock", WiFi.localIP())) {
    USE_SERIAL.println("MDNS responder started");
    mdns.addService("http", "tcp", 80);
    mdns.addService("ws", "tcp", 81);
  }
  else {
    USE_SERIAL.println("MDNS.begin failed");
  }
  USE_SERIAL.print("Connect to http://espWebSock.local or http://");
  USE_SERIAL.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop();
  server.handleClient();
}
