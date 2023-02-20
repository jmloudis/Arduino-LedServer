#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    4

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 142

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);

#ifndef STASSID
#define STASSID "wifi"
#define STAPSK  "password"
#endif

#define DELAYVAL 100 

const char* ssid = STASSID;
const char* password = STAPSK;



void setup() {
  OTAsetUp();
  
  server.on("/", HTTP_GET, handleRoot);
  //server.on("/setleds", HTTP_GET, handleSetLeds);
  server.on("/violet", HTTP_GET, violetPattern);
  server.on("/wipe", HTTP_GET, colorWipePattern);
  server.on("/off", HTTP_GET, turnOff);
  server.on("/violet2", HTTP_GET, violetPattern2);
  server.on("/night", HTTP_GET, nightLight);
  server.on("/rainbow", HTTP_GET, rainbowPattern);
  server.on("/sparkle", HTTP_GET, sparkleLight);
  server.onNotFound(handleNotFound);

  // Start running the webserver
  server.begin();
  Serial.println("HTTP server started");
  
  //pinMode(LED_BUILTIN, OUTPUT);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

}



void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  

}






// ----------- WEB SERVER -----------

void handleRoot() {
  // Simply sends an 'OK' (200) response to the client, and a plain text
  // string with usage.
  server.send(200, "text/plain", String("Hello from esp8266! Usage: navigate to") +
              String(" /setleds?led1=XX&led2=YY changing XX/YY to ON or OFF."));
}


void handleSetLeds() {
  // We can read the desired status of the LEDs from the expected parameters that
  // should be passed in the URL.  We expect two parameters "led1" and "led2".
  String LED_status = server.arg("LED");
  
  // Check if the URL include a change of the LED status
  bool url_check = false;
  if((LED_status == "ON")||(LED_status == "OFF"))
    url_check = true;

  // It's not required to pass them both, so we check that they're exactly equal to
  // the strings ON or OFF by our design choice (this can be changed if you prefer
  // a different behavior)
  if (LED_status == "ON")
    strip.show();
  else if (LED_status == "OFF")
    
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, (0,0,0));         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match                           //  Pause for a moment
    }
  
  if (url_check)
    // If we've set the LEDs to the requested status, we have the webserver
    // return an "OK" (200) response.  We also include the number of milliseconds
    // since the program started running.
    // Note: This number will overflow (go back to zero), after approximately 50 days.
    server.send(200, "text/plain", "LEDs' status changed! (" + String(millis()) + ")");
  else
    server.send(200, "text/plain", "LEDs' status unchanged! (" + String(millis()) + ")");
}

// If the client requests any other URL than the root directory or the /setled path:
void handleNotFound() {
  // We construct a message to be returned to the client
  String message = "File Not Found\n\n";
  // which includes what URI was requested
  message += "URI: ";
  message += server.uri();
  // what method was used
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  // and what parameters were passed
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  // the response, as expected, is a "Not Found" (404) error
  server.send(404, "text/plain", message);
}



// ----------- LED PATTERNS --------------

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
                                           //  Update strip to match  
    strip.show();  //  Pause for a moment
  }
}

void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(100);
                  // Update strip with new contents
                  // Pause for a moment
  }
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(LED_COUNT);
  strip.setPixelColor(Pixel,red,green,blue);
  strip.show();
  delay(SpeedDelay);
  strip.setPixelColor(Pixel,0,0,0);
}



// ------ initialize --------
void colorWipePattern(){


  
    colorWipe(strip.Color(255,   0,   0), 10); // Red
    colorWipe(strip.Color(  0, 255,   0), 10); // Green
    
  
  
}

void violetPattern(){

  server.send(200, "text/plain", "LEDs' status changed! (" + String(millis()) + ")");
  strip.setBrightness(50);
  colorWipe(strip.Color(238,130,238), 10);
//  strip.show();
  
  
}

void violetPattern2(){

  server.send(200, "text/plain", "LEDs' status changed! (" + String(millis()) + ")");
  strip.setBrightness(25);
  colorWipe(strip.Color(244,172,26), 10);
  
//  strip.show();
  
  
}

void nightLight(){

  server.send(200, "text/plain", "LEDs' status changed! (" + String(millis()) + ")");
  strip.setBrightness(10);
  colorWipe(strip.Color(139, 136, 248), 10);
  
//  strip.show();
  
  
}

void sparkleLight(){

  
  Sparkle(0xff, 0xff, 0xff, 0);
  strip.show();
}


void turnOff(){
  
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, (0,0,0));         
    strip.show();                                                  
    }

  server.send(200, "text/plain", "LEDs' status changed! (" + String(millis()) + ")");
}

void rainbowPattern(){

  
    for(int i = 0; i>=0; i++){
    rainbow(10);
    strip.show();  
  }
  
  
  
  
//  strip.show();

  server.send(200, "text/plain", "LEDs' status changed! (" + String(millis()) + ")");
}





void OTAsetUp(){
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}