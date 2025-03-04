#include <WiFiS3.h>
#include <WebSocketServer.h>
#include "index.h"
//COMMANDS
#define CMD_STOP 0
#define CMD_FORWARD 1
#define CMD_BACKWARDS 2
#define CMD_LEFT 4
#define CMD_RIGHT 8
//SENSOR PIN CONNECTIONS
//ULTRASONIC SENSOR BACK
const int trigPinB = 9;
const int echoPinB = 10;
float durationB, distanceB, d;

//MOTOR PIN CONNECTIONS
//RIGHT SIDE
const int AIB1 = 1;
const int AIA1 = 2;
const int BIB1 = 3;
const int BIA1 = 4;
//LEFT SIDE
const int AIB2 = 5;
const int AIA2 = 6;
const int BIB2 = 7;
const int BIA2 = 8;
//MOTOR SPEED
byte speed = 4000;
//WIFI CREDENTIALS
const char ssid[] = "ARDUINO_WIFI_V1.3";  // WIFI POINT NAME
const char password[] = "12345678";       // MIN CHAR LENGTH 8
using namespace net;
//WEBSOCKET SERVER
WebSocketServer webSocket(81);
//WIFI SERVER
WiFiServer server(80);
int status = WL_IDLE_STATUS;
//
void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
  Serial.println("FIRMWARE VERSION CHECK");
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) { Serial.println("Please upgrade the firmware"); }
  Serial.println("FIRMWARE VERSION CHECK SUCCESSFUL!");
  delayMicroseconds(10);
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
  pinMode(trigPinB, OUTPUT);
  pinMode(echoPinB, INPUT);

  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
  Serial.println("MOTOR CONNECTIONS SETUP START");
  pinMode(AIA1, OUTPUT);
  Serial.println(AIA1);
  pinMode(AIB1, OUTPUT);
  Serial.println(AIB1);
  pinMode(BIA1, OUTPUT);
  Serial.println(BIA1);
  pinMode(BIB1, OUTPUT);
  Serial.println(BIB1);
  pinMode(AIA2, OUTPUT);
  Serial.println(AIA2);
  pinMode(AIB2, OUTPUT);
  Serial.println(AIB2);
  pinMode(BIA2, OUTPUT);
  Serial.println(BIA2);
  pinMode(BIB2, OUTPUT);
  Serial.println(BIB2);
  Serial.println("MOTOR CONNECTIONS SETUP SUCCESSFUL!");
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
  Serial.println("IP ADDRESS CREATION");
  WiFi.config(IPAddress(192, 48, 56, 2));
  Serial.println("IP ADDRESS CREATION SUCCESSFUL!");
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);
  Serial.print("With password: ");
  Serial.println(password);
  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, password);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true)
      ;
  }
  // wait 1 second for connection:
  delay(1000);
  // print your board's IP address:
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
  Serial.println("SERVER INITIALIZATION");
  server.begin();
  Serial.println("SERVER INITIALIZATION SUCCESSFUL!");
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
  Serial.println("WEBSOCKET SETUP");
  webSocket.onConnection([](WebSocket &ws) {
    //sets automatically the protocol type
    const auto protocol = ws.getProtocol();
    if (protocol) {
      Serial.print(F("Client protocol: "));
      Serial.println(protocol);
    }
    //ws.send(const DataType, const char *message, uint16_t length)
    //websocket receive message

    ws.onMessage([](WebSocket &ws, const WebSocket::DataType dataType, const char *message, uint16_t length) {
      String cmd_str = String((char *)message);
      int command = cmd_str.toInt();
      Serial.print("command: ");
      Serial.println(command);
      //action besed on websocket
      while (true) {
        switch (dataType) {
          case WebSocket::DataType::TEXT:
            switch (command) {
              case CMD_STOP:
                Serial.println("Stop");
                stop();
                break;
              case CMD_FORWARD:
                Serial.println("Move Forward");
                forward();
                break;
              case CMD_BACKWARDS:
                //checks distance once but the goal is to check again and again
                Serial.println("Move Backwards");
                if (checkDistance() <= 20.0) {
                  stop();
                } else {
                  backwards();
                }
                /*if I make th function backwards(){
                  while(true){
                    if(checkDistance()>=20.0){
                      analogWrite(AIA1, speed);
                      analogWrite(AIB1, 0);
                      analogWrite(BIA1, 0);
                      analogWrite(BIB1, speed);
                      analogWrite(AIA2, speed);
                      analogWrite(AIB2, 0);
                      analogWrite(BIA2, speed);
                      analogWrite(BIB2, 0);
                    }else{
                      stop();
                    }
                  }
                }
                and make the case code like this
                case CMD_BACKWARDS:
                  backwards();
                  break;
                the loop in the function doesn't break and while it's continuously working correctly it's not stoping if I press an other button
                */
                break;
              case CMD_LEFT:
                Serial.println("Turn Left");
                turnleft();
                break;
              case CMD_RIGHT:
                Serial.println("Turn Right");
                turnright();
                break;
              default:
                Serial.println("Unknown command");
            }

            break;
          case WebSocket::DataType::BINARY:
            Serial.println(F("Received binary data"));
            break;
        }
      }
    });
    //websocket closing
    ws.onClose([](WebSocket &, const WebSocket::CloseCode, const char *,
                  uint16_t) {
      Serial.println(F("Disconnected"));
    });
    Serial.print(F("New WebSocket Connnection from client: "));
    Serial.println(ws.getRemoteIP());
  });
  webSocket.begin();
  Serial.println("WEBSOCKET SETUP SUCCESSFUL!");
  Serial.println("////////////////////////////////////////////////////////////////////////////////////////////////");
}

void loop() {
  webSocket.listen();
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    // read the HTTP request header line by line
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        String HTTP_header = client.readStringUntil('\n');  // read the header line of HTTP request
        if (HTTP_header.equals("\r"))                       // the end of HTTP request
          break;
        Serial.print("<< ");
        Serial.println(HTTP_header);  // print HTTP request to Serial Monitor
      }
    }

    // send the HTTP response header
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");  // the connection will be closed after completion of the response
    client.println();                     // the separator between HTTP header and body
    String html = String(HTML_CONTENT);
    client.println(html);
    client.flush();
    // give the web browser time to receive the data
    delay(1000);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void forward() {
  analogWrite(AIA1, 0);
  analogWrite(AIB1, speed);
  analogWrite(BIA1, speed);
  analogWrite(BIB1, 0);
  analogWrite(AIA2, 0);
  analogWrite(AIB2, speed);
  analogWrite(BIA2, 0);
  analogWrite(BIB2, speed);
}

void backwards() {
  analogWrite(AIA1, speed);
  analogWrite(AIB1, 0);
  analogWrite(BIA1, 0);
  analogWrite(BIB1, speed);
  analogWrite(AIA2, speed);
  analogWrite(AIB2, 0);
  analogWrite(BIA2, speed);
  analogWrite(BIB2, 0);
}

void stop() {
  analogWrite(AIA1, 0);
  analogWrite(AIB1, 0);
  analogWrite(BIA1, 0);
  analogWrite(BIB1, 0);
  analogWrite(AIA2, 0);
  analogWrite(AIB2, 0);
  analogWrite(BIA2, 0);
  analogWrite(BIB2, 0);
}

void turnright() {
  analogWrite(AIA1, speed);
  analogWrite(AIB1, 0);
  analogWrite(BIA1, 0);
  analogWrite(BIB1, speed);
  analogWrite(AIA2, 0);
  analogWrite(AIB2, speed);
  analogWrite(BIA2, 0);
  analogWrite(BIB2, speed);
}

void turnleft() {
  analogWrite(AIA1, 0);
  analogWrite(AIB1, speed);
  analogWrite(BIA1, speed);
  analogWrite(BIB1, 0);
  analogWrite(AIA2, speed);
  analogWrite(AIB2, 0);
  analogWrite(BIA2, speed);
  analogWrite(BIB2, 0);
}

float checkDistance() {
  digitalWrite(trigPinB, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinB, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinB, LOW);
  durationB = pulseIn(echoPinB, HIGH);
  distanceB = (durationB * .0343) / 2;
  Serial.print("Distance: ");
  Serial.println(distanceB);
  delay(1);
  return distanceB;
}
