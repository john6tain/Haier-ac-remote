// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <ESP8266HTTPClient.h>
#include <IRac.h>
#include <IRutils.h>
#include <WiFiClientSecure.h>
#include "DHT.h"


#define DHTTYPE DHT11 

class ButtonState {
  private:
    int degrees;
    String fanSpeed;
    String modeType;
    bool on;
    bool auto_mode;
    float humidity;
    float temperature;
    float minTemperature;
    float maxTemperature;
  public:
  
    ButtonState(int degrees, String fanSpeed, String modeType,bool on,bool auto_mode,
				float humidity, float temperature,float minTemperature,float maxTemperature){
      this->degrees = degrees;
      this->fanSpeed = fanSpeed;
      this->modeType = modeType;
      this->on = on;
      this->auto_mode = auto_mode;
      this->humidity = humidity;
      this->temperature = temperature;
      this->minTemperature = minTemperature;
      this->maxTemperature = maxTemperature;
    }
    
    void set_degrees(int degrees){
      this->degrees = degrees;
    }
    
    void set_fanSpeed(String fanSpeed){
      this->fanSpeed = fanSpeed;
    }
    
    void set_modeType(String modeType){
      this->modeType = modeType;
    }
    void set_on(bool on){
      this->on = on;
    }
    
    void set_auto(bool auto_mode){
      this->auto_mode = auto_mode;
    }
    
    void set_humidity(float humidity){
      this->humidity = humidity;
    }
      
    void set_temperature(float temperature){
      this->temperature  = temperature;
    }
	
	void set_maxTemperature(float maxTemperature){
      this->maxTemperature  = maxTemperature;
    }
	
	void set_minTemperature(float minTemperature){
      this->minTemperature  = minTemperature;
    }
    
    int get_degrees(){
      return this->degrees;
    }
    
    String get_fanSpeed(){
      return this->fanSpeed;
    }
    
    String get_modeType(){
      return this->modeType;
    }
    bool get_on(){
      return this->on;
    }
    
    bool get_auto(){
      return this->auto_mode;
    }
    
    float get_humidity(){
      return this->humidity;
    }
      
    float get_temperature(){
      return this->temperature;
    }
	
	float get_minTemperature(){
      return this->minTemperature;
    }
	
	float get_maxTemperature(){
      return this->get_maxTemperature;
    }
};


IRac ac(4);

// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "PASS";
String HOST = "https://URL";

WiFiClientSecure client;
HTTPClient http;

String fanSpeed = "auto";
String modeType = "Heat";


const int DHTPin = 14; // D5
const int output4 = 4; // D2

//uint8_t DHTPin = D5; 

DHT dht(DHTPin, DHT11); 

ButtonState buttonState(25,"Auto","Heat",false,false,100,100,19,22);

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
    buttonState.set_on(true);
    ac.sendAc();
  } else if (response == "OFF") {
    buttonState.set_on(false);
    ac.next.power = false;
    ac.sendAc();
  } else if (response.indexOf("auto:") > -1) {
    String auto_state = response.substring(5, response.length());
    buttonState.set_auto(auto_state == "ON");
  } else if (response.indexOf("degrees:") > -1) {
    int degrees = response.substring(8, response.length()).toInt();
    ac.next.degrees = degrees;
    buttonState.set_degrees(degrees);
    ac.sendAc();
  } else if (response.indexOf("min-max:") > -1) {
    float min = response.substring(8, response.indexOf('-')).toFloat();
    float max = response.substring(response.indexOf('-'), response.length()).toFloat();

    buttonState.set_minTemperature(min);
    buttonState.set_maxTemperature(max);
    ac.sendAc();
  } else if (response.indexOf("MODE:") > -1) {
    String modeType = response.substring(5, response.length());
    buttonState.set_modeType(modeType);
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
    buttonState.set_fanSpeed(fanSpeed);
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

void setAuto(){
	if(buttonState.get_auto()){
		if(Number(buttonState.get_temperature()) >=Number(buttonState.get_maxTemperature())){
			if(buttonState.get_on()) {
				buttonState.set_on(false);
				response = 'OFF';
			}
		} else if (Number(buttonState.get_temperature()) <=Number(buttonState.get_minTemperature())){	
			if(!buttonState.get_on() {
				buttonState.set_on(true);
			}
		}
	}
}


void sendButtonState() {
  float temperature;
  float humidity;
  temperature = dht.readTemperature(); // Gets the values of the temperature
  humidity = dht.readHumidity(); // Gets the values of the humidity 
  
  buttonState.set_temperature(temperature);
  buttonState.set_humidity(humidity);

  httpClient(HOST+"/get/button/state/"+String(buttonState.get_degrees())+"/"+String(buttonState.get_fanSpeed())+"/"+String(buttonState.get_modeType())+"/"+(buttonState.get_on() ? "true":"false" )+"/"+(buttonState.get_auto() ? "true":"false" )+"/"+String(buttonState.get_humidity())+"/"+String(buttonState.get_temperature())+"/"+String(buttonState.get_minTemperature())+"/"+String(buttonState.get_maxTemperature()));

}

void loop() {
    client.setInsecure(); //the magic line, use with caution
    client.connect(HOST+"", 443);
    checkResponse(httpClient(HOST+"/get/command"));
    sendButtonState();
	setAuto();
}
