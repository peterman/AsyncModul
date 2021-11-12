w.htm
#include <ArduinoOTA.h>
#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <ArduinoJson.h>;
//#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <InfluxDbClient.h>


// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

#include "parameter.h";
#include "sockets.h";
#include "functions.h";
#include "influxdb.h";
#include "ota.h";
#include "crontab.h";

void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);
   // Serial.setDebugOutput(true);
  Serial.println();
  pinMode(ONBOARD_LED,OUTPUT);
  
  SPIFFS.begin();
  bmp.begin(0x76);
  
  delay(500);
  readsettings();  
  Save_Wifiscan_Result();
  
  
  WiFi.mode(WIFI_STA); 
  delay(100);
  WiFi.begin(settings.ssid, settings.pass);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(settings.ssid, settings.pass);
  }
  
  Serial.println(WiFi.localIP());
  
  SetupOTA();
  
  MDNS.addService("http","tcp",80);

  SPIFFS.begin();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!",NULL,millis(),1000);
  });
  server.addHandler(&events);

#ifdef ESP32
  server.addHandler(new SPIFFSEditor(SPIFFS, settings.edit_user,settings.edit_pass));
#elif defined(ESP8266)
  server.addHandler(new SPIFFSEditor(settings.edit_user,settings.edit_pass));
#endif
  
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });


server.on("/setwifi", HTTP_GET, [](AsyncWebServerRequest *request){
  String para1;
  String para2;
  if (request->hasParam("ssid") && request->hasParam("key")) {
    para1 = request->getParam("ssid")->value();
    para2 = request->getParam("key")->value();
    request->send(200, "text/plain", "OK");
  } else {
    request->send(200, "text/plain", "nothing");
  }
    
});


//----------------------------------------------------------------------------------------
  // Check server connection
  if (influx_client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(influx_client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influx_client.getLastErrorMessage());
  }

//---------------------------------------------------------------------------------------

  Serial.println("Setup done");
  digitalWrite(ONBOARD_LED,HIGH);
  server.begin();

}

void loop(){
  ArduinoOTA.handle();
  ws.cleanupClients();
  do_cronjobs(); 
}
