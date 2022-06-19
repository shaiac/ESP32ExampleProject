#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

void BlinkTask(void * parameter);
void WebServerTask(void * parameter);
void handle_root();
void handle_stop_blink();
String getRootPage();


TaskHandle_t Task1;
TaskHandle_t Task2;

int LED_BUILTIN = 2;

// SSID & Password
const char* ssid = "ShaiLocal";
const char* password = "^65cG949";
bool isBlinkingMode = false;

WebServer server(80);

void setup() {
  Serial.begin(115200); 
  
  pinMode(LED_BUILTIN, OUTPUT);
  xTaskCreatePinnedToCore(BlinkTask,"Task1",10000,NULL,1,&Task1,0);                         
  delay(500); 

  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  server.on("/", handle_root);
  server.on("/blinking", handle_stop_blink);

  server.begin();
  Serial.println("HTTP server started");
  delay(100); 

  xTaskCreatePinnedToCore(WebServerTask,"Task2",10000,NULL,1,&Task2,1);          
  delay(500); 
}

void WebServerTask( void * parameter ){
  Serial.print("WebServer task is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    server.handleClient();
    delay(1000);
  } 
}

void BlinkTask( void * parameter ){
  Serial.print("Blink task is running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    if (isBlinkingMode) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
    } else {
        delay(2000);
    }
  }
}

void loop() {
  delay(2000);
}

String getRootPage() {
  // HTML & CSS contents which display on web server
  String HTML = "<!DOCTYPE html>\
  <html>\
    <head>\
      <title>Compulab Solarpanel</title>\
    </head>\
    <body style='background-color:rgb(31, 41, 45);text-align:center;color:white'>\
      <h1>That is Compulab webserver, param1 &#128522;</h1>\
      <div style='color:red;margin-bottom:50px;font-size:30px;'>Nikita this site is for you</div>\
      <a href='/blinking'>\
		    <button>Start Blink</button>\
	    </a>\
	    <a href='/'>\
		    <button>Stop Blink</button>\
	    </a>\
    </body>\
  </html>";  
  HTML.replace("param1", "Replace test");
  return HTML;
}


// Handle root url (/)
void handle_root() {
  isBlinkingMode = false;
  server.send(200, "text/html", getRootPage());
}

void handle_stop_blink() {
  isBlinkingMode = true;
  Serial.println("is Blinking mode true");
  server.send(200, "text/html", getRootPage());
}

