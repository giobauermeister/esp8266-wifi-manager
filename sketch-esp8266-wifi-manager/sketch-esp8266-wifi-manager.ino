#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <FS.h>

// Definições da rede Wifi
const char* SSID = "xxxxx";
const char* PASSWORD = "xxxxx";


// Set web server port number to 80
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

void setup() {
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
    
  Serial.begin(115200);
  delay(5000);
  initWiFi();
  startSPIFFS();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", handleRoot);
  server.serveStatic("/icons/wifi-solid.svg", SPIFFS, "/icons/wifi-solid.svg");
  server.serveStatic("/icons/key-solid.svg", SPIFFS, "/icons/key-solid.svg");
  server.serveStatic("/icons/eye-slash-solid.svg", SPIFFS, "/icons/eye-slash-solid.svg");
  server.serveStatic("/icons/eye-solid.svg", SPIFFS, "/icons/eye-solid.svg");

  server.begin();
}

void loop() {
  webSocket.loop();
  server.handleClient();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            //webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            Serial.println();
            Serial.printf("[%u] get Text: %s\n", num, payload);
            //String message = (char *) &payload[0];
            //Serial.println(message);

            // Exemplo versão ArduinoJson 5
            /*StaticJsonBuffer<200> jBuffer;
            JsonObject& jObject = jBuffer.parseObject(data);
            int colorR = jObject["r"];
            Serial.print(colorR);*/

            // Cria Objeto Json
            //DynamicJsonDocument jsonBuffer(length+3);
            StaticJsonDocument<512> jsonBuffer;
            Serial.print("Payload Length: ");
            Serial.println(length);
            deserializeJson(jsonBuffer, payload);
            JsonObject jsonObject = jsonBuffer.as<JsonObject>();

            // Rotina de informações de conexão do socket
            if(jsonObject.containsKey("connectionInfo"))
            {
              Serial.print("Socket status: ");
              Serial.print(jsonObject["connectionInfo"]["status"].as<char*>());
            }            

            // Rotina de informações de rede
            if(jsonObject.containsKey("networkInfo"))
            {
              JsonVariant jvarSsid = jsonObject["networkInfo"]["ssid"];
              JsonVariant jvarPassword = jsonObject["networkInfo"]["password"];
              const char* ssid = jvarSsid.as<const char*>();
              const char* password = jvarPassword.as<const char*>();
              
              Serial.print("SSID: ");
              Serial.print(ssid);
              Serial.print(" | Password: ");
              Serial.println(password);
            }            
            
            if(jsonObject.containsKey("ioControlInfo"))
            {
              //Neste 
              Serial.print("ioPin: ");
              Serial.print(jsonObject["ioControlInfo"]["ioPin"].as<char*>());
              Serial.print(" | ioState: ");
              Serial.print(jsonObject["ioControlInfo"]["ioState"].as<char*>());              
                            
              JsonVariant ioPin = jsonObject["ioControlInfo"]["ioPin"];
              JsonVariant ioState = jsonObject["ioControlInfo"]["ioState"];
              uint8_t _ioPin = ioPin.as<uint8_t>();
              uint8_t _ioState = ioState.as<uint8_t>();
              digitalWrite(_ioPin, _ioState);
            }

            // Rotina de mensagens genéricas
            if(jsonObject.containsKey("messageInfo"))
            {
              //Neste caso a mensagem contém um valor em formato string(já vem assim do javascript) que é convertido para inteiro  
              //Serial.print(jsonObject["messageInfo"]["message"].as<char*>());
              JsonVariant messageContent = jsonObject["messageInfo"]["message"];
              const char* _messageContent = messageContent.as<char*>();     
              JsonVariant messageSize = jsonObject["messageInfo"]["size"];
              int _messageSize = messageSize.as<int>();

              Serial.print("Message: ");
              Serial.print(_messageContent);
              Serial.print(" | Size: ");
              Serial.println(_messageSize);

              if(strcmp(_messageContent, "getNetworks") == 0)
              {
                int n;
                size_t len;
                DynamicJsonDocument networksList(JSON_ARRAY_SIZE(50));
                
                Serial.println("scan start");
                n = WiFi.scanNetworks();
                if (n == 0) {
                  Serial.println("no networks found");
                } else {
                  Serial.print(n);
                  Serial.println(" networks found");
                  for (int i = 0; i < n; ++i) {
                    // Print SSID and RSSI for each network found
                    Serial.print(i + 1);
                    Serial.print(": ");
                    Serial.print(WiFi.SSID(i));
                    Serial.print(" (");
                    Serial.print(WiFi.RSSI(i));
                    Serial.print(")");
                    Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
                    //networks[i] = WiFi.SSID(i);
                    networksList.add(WiFi.SSID(i));
                    delay(10);
                  }
                }
                len = measureJson(networksList);              
                char networksArray[len];              
                serializeJson(networksList, Serial);
                Serial.println("");              
                Serial.println(len);
                serializeJson(networksList, networksArray, len+1);
                webSocket.sendTXT(num, networksArray, strlen(networksArray));
              }
              if(strcmp(_messageContent, "getTest2") == 0)
              {
                Serial.println("getTest2");
              }
              if(strcmp(_messageContent, "getTest3") == 0)
              {
                Serial.println("getTest3");
              }
            }

//            if(payload[0] == '#') {
//                // we get RGB data
//
//                // decode rgb data
//                uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);
//
//                analogWrite(LED_RED,    ((rgb >> 16) & 0xFF));
//                analogWrite(LED_GREEN,  ((rgb >> 8) & 0xFF));
//                analogWrite(LED_BLUE,   ((rgb >> 0) & 0xFF));
//            }

            break;
    }

}

void handleRoot() {
  //server.send(200, "text/html", page);   // Send HTTP status 200 (Ok) and send some text to the browser/client
  File file = SPIFFS.open("/index.html.gz", "r");
  server.streamFile(file, "text/html");
  file.close();
}

void startSPIFFS() { // Start the SPIFFS and list all contents
  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}
String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

//*************** Inicia WiFi *******************
void initWiFi() 
{
  
  if(WiFi.status() == WL_CONNECTED)
    return; 
   
  WiFi.begin(SSID, PASSWORD);
  
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println();
  Serial.print("Connected to: ");
  Serial.print(SSID);
  Serial.print(" | IP address: ");
  Serial.println(WiFi.localIP());  
}
