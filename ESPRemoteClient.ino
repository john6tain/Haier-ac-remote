// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <ESP8266HTTPClient.h>
#include <IRac.h>
#include <IRutils.h>
#include <WiFiClientSecure.h>
#include "DHT.h"


#define DHTTYPE DHT11 


IRac ac(4);

// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "PASSWORD";

WiFiClientSecure client;
HTTPClient http;

int degrees = 25;
String fanSpeed = "auto";
String modeType = "Heat";


const int DHTPin = 14; // D5
const int output4 = 4; // D2

//uint8_t DHTPin = D5; 

DHT dht(DHTPin, DHT11); 

void setupAC() {
  ac.next.protocol = decode_type_t::HAIER_AC;  // Set a protocol to use.
  ac.next.model = 2;  // Some A/Cs have different models. Try just the first.
  //  ac.next.mode = stdAc::opmode_t::kCool;  // Run in cool mode initially.
  ac.next.mode = stdAc::opmode_t::kHeat;
  ac.next.celsius = true;  // Use Celsius for temp units. False = Fahrenheit
  ac.next.degrees = 25;  // 25 degrees.
  ac.next.fanspeed = stdAc::fanspeed_t::kMax;  // Start the fan at medium.
  ac.next.swingv = stdAc::swingv_t::kOff ;  // Don't swing the fan up or down.
  //  ac.next.swingh = stdAc::swingh_t::kOff;  // Don't swing the fan left or right.
  ac.next.light = true;  // Turn off any LED/Lights/Display that we can.
  ac.next.beep = true;  // Turn off any beep from the A/C if we can.
  ac.next.econo = false;  // Turn off any economy modes if we can.
  //  ac.next.filter = false;  // Turn off any Ion/Mold/Health filters if we can.
  ac.next.turbo = true;  // Don't use any turbo/powerful/etc modes.
  ac.next.quiet = false;  // Don't use any quiet/silent/etc modes.
  //  ac.next.sleep = -1;  // Don't set any sleep time or modes.
  ac.next.clean = false;  // Turn off any Cleaning options if we can.
  ac.next.clock = -1;  // Don't set any current time if we can avoid it.
  ac.next.power = false;  // Initially start with the unit off.

}
void setup() {
  Serial.begin(115200);
  pinMode(DHTPin, INPUT);
  setupAC();
  dht.begin();   
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void checkResponse(String response) {
  if (response.length() > 2) {
    Serial.println(response);
  }
  if (response == "ON") {
    ac.next.power = true;
    ac.sendAc();
  } else if (response == "OFF") {
    ac.next.power = false;
    ac.sendAc();
  } else if (response.indexOf("degrees:") > -1) {
    int degrees = response.substring(8, response.length()).toInt();
    ac.next.degrees = degrees;
    ac.sendAc();
  } else if (response.indexOf("MODE:") > -1) {
    String modeType = response.substring(5, response.length());
    if (modeType == "Cool") {
      ac.next.mode = stdAc::opmode_t::kCool;
    } else if (modeType == "Heat") {
      ac.next.mode = stdAc::opmode_t::kHeat;
    } else if (modeType == "Dry") {
      ac.next.mode = stdAc::opmode_t::kDry;
    }else if (modeType == "Fan") {
      ac.next.mode = stdAc::opmode_t::kFan;
    } else if (modeType == "Auto") {
      ac.next.mode = stdAc::opmode_t::kAuto;
    }
    ac.sendAc();
  } else if (response.indexOf("FAN:") > -1) {
    String fanSpeed = response.substring(4, response.length());

    if (fanSpeed == "Min") {
      ac.next.fanspeed = stdAc::fanspeed_t::kMin;
    } else if (fanSpeed == "Low") {
      ac.next.fanspeed = stdAc::fanspeed_t::kLow;
    } else if (fanSpeed == "Medium") {
      ac.next.fanspeed = stdAc::fanspeed_t::kMedium;
    } else if (fanSpeed == "High") {
      ac.next.fanspeed = stdAc::fanspeed_t::kHigh;
    } else if (fanSpeed == "Max") {
      ac.next.fanspeed = stdAc::fanspeed_t::kMax;
    } else if (fanSpeed == "Auto") {
      ac.next.fanspeed = stdAc::fanspeed_t::kAuto;
    }
    ac.sendAc();
  }

}
String httpClient(String url){
    if (http.begin(client, url)) { // HTTP/HTTPS  with fingerprint

        //        Serial.print("[HTTPS] GET...\n");
    int httpCode = http.GET();

    if (httpCode > 0) {
       //Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        //        Serial.println(payload);
        return payload;
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());

      return "[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str();
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
    return "[HTTP} Unable to connect\n";
  }
  
}


void sendTempAndHumidity() {
  float temperature;
  float humidity;
  temperature = dht.readTemperature(); // Gets the values of the temperature
  humidity = dht.readHumidity(); // Gets the values of the humidity 

  httpClient("https://localhost/get/sensors/"+String(temperature)+"/"+String(humidity));

}

void loop() {
    client.setInsecure(); //the magic line, use with caution
    client.connect("https://localhost", 443);
    checkResponse(httpClient("https://localhost/get/command"));
    sendTempAndHumidity();
}
