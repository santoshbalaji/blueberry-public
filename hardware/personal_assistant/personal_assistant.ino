// For NTPClient libraries 
#include <WiFiUdp.h>
#include <NTPClient.h>

// For WIFI 
#include <ESP8266WiFi.h>
#include <SSD1306Wire.h>
#include "images.h"

// For HTTPClient
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// For HTTPSClient
#include <WiFiClientSecure.h> 

//HARDWARE PROPERTIES
const int OLED_SCL = 5;
const int OLED_SDA = 4;
const int OLED_ADDRESS = 0x3c;
const int SWITCH_PIN = 14;
const int SERIAL_BAUD_RATE = 9600;

//WIFI PROPERTIES
const char* WIFI_SSID = "xxxxxx";
const char* WIFI_PASSWORD = "xxxxxxx";
const int WIFI_LOGO_X = 34;
const int WIFI_LOGO_Y = 15;

//FOR CLOCK
const char* MENU_CLOCK = "CLOCK";
const int MENU_CLOCK_X = 10;
const int MENU_CLOCK_Y = 2;
const int CLOCK_OLED_X = 20;
const int CLOCK_OLED_Y = 12;
const int DATE_OLED_X = 25;
const int DATE_OLED_Y = 44;
const long TIME_UTC_OFFSET_IN_SECONDS = 28800;
const char* TIME_HOST_NAME = "pool.ntp.org";

//FOR CURRENCY APP
const char* MENU_CURRENCY = "CURRENCY";
const int MENU_CURRENCY_X = 10;
const int MENU_CURRENCY_Y = 2;
const char* CURRENCY_FINGERPRINT = "41:ED:80:A7:16:4F:A8:74:E3:50:9D:DD:9D:2C:89:4A:C8:66:C7:85";
const char* CURRENCY_HOST = "api.exchangeratesapi.io";
const int CURRENCY_PORT = 443;
const String CURRENCY_MESSAGE = String("GET ") + "/latest?base=SGD HTTP/1.1\r\nHost: " + CURRENCY_HOST + "\r\nConnection: close\r\n\r\n";
const char* CURRENCY_FROM = "SGD";
const char* CURRENCY_TO = "INR";
const char* CURRENCY_MID = "=";
const int CURRENCY_FROM_X = 10;
const int CURRENCY_FROM_Y = 15;
const int CURRENCY_TO_X = 85;
const int CURRENCY_TO_Y = 15;
const int CURRENCY_MID_X = 60;
const int CURRENCY_MID_Y = 15;
const int CURRENCY_FROM_VALUE_X = 10;
const int CURRENCY_FROM_VALUE_Y = 35;
const int CURRENCY_TO_VALUE_X = 85;
const int CURRENCY_TO_VALUE_Y = 35;
const int CURRENCY_INTERVAL = 60000;
int CURRENCY_CURRENT_MILLIS = 0;

//FOR LTA BUS APP
const char* MENU_BUS = "BUS";
const int MENU_BUS_X = 10;
const int MENU_BUS_Y = 2;
const int BUS_X[] = {10, 71, 10, 71};
const int BUS_Y[] = {14, 14, 31, 31};
const char* LTA_API = "http://datamall2.mytransport.sg/ltaodataservice/BusArrivalv2?BusStopCode=xxxxxx";
const char* KEY_ACCOUNT = "AccountKey";
const char* KEY_ACCOUNT_VALUE = "xxxxxxxxxxxx";
const char* KEY_ACCEPT = "Accept";
const char* KEY_ACCEPT_VALUE = "application/json";
const int HTTP_CODE_SUCCESS = 0;
const int LTA_INTERVAL = 30000;
int LTA_CURRENT_MILLIS = 0;

//FOR NEA WEATHER APP
const char* MENU_WEATHER = "WEATHER";
const int MENU_WEATHER_X = 10;
const int MENU_WEATHER_Y = 2;
const char* WEATHER_FINGERPRINT = "5F:11:08:DB:43:23:6C:3B:A0:08:60:E4:0F:44:64:A7:57:CB:B3:B1";
const char* WEATHER_HOST = "api.data.gov.sg";
const int WEATHER_PORT = 443;
const String WEATHER_MESSAGE = String("GET ") + "/v1/environment/air-temperature HTTP/1.1\r\nHost: " + WEATHER_HOST + "\r\nConnection: close\r\n\r\n";
const int WEATHER_VAL_X = 32;
const int WEATHER_VAL_Y = 20;
const int WEATHER_DEG_X = 62;
const int WEATHER_DEG_Y = 20;
const int WEATHER_DEG_RADIUS = 2;
const int WEATHER_INTERVAL = 60000;
int WEATHER_CURRENT_MILLIS = 0;

int menuValue = 1;
int totalMenuCount = 4;
bool switchStatus = false;
SSD1306Wire display(OLED_ADDRESS, OLED_SDA, OLED_SCL);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, TIME_HOST_NAME, TIME_UTC_OFFSET_IN_SECONDS);

void setup() 
{
  initializeSerial();
  initializeDisplay();
  initializePin();
  initializeWifi();
  initializeTime();
}

void loop() 
{
  changeSwitch();
  changeMenu();
  displayClock();
  displayCurrency();
  displayBusTiming();
  displayWeather();
}

void initializeDisplay()
{
  display.init();
  display.flipScreenVertically();
}

void initializePin()
{
  pinMode(SWITCH_PIN, INPUT);
}

void initializeSerial()
{
  Serial.begin(SERIAL_BAUD_RATE);
}

void initializeWifi()
{
  display.clear();
  display.drawXbm(WIFI_LOGO_X, WIFI_LOGO_Y, WIFI_LOGO_WIDTH, WIFI_LOGO_HEIGHT, WIFI_LOGO_BITS);
  display.display();  
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  
  display.clear();
} 

void initializeTime()
{
  timeClient.begin();
}

void changeSwitch()
{
  int temp = digitalRead(SWITCH_PIN);
  if(temp == 0)
  {
    switchStatus = false;
  }
}

void changeMenu()
{
  int temp = digitalRead(SWITCH_PIN);
  if(temp == 1 && !switchStatus)
  {
    menuValue = menuValue < totalMenuCount ? (menuValue + 1) : 1;
    switchStatus = true;
    CURRENCY_CURRENT_MILLIS = millis() + CURRENCY_INTERVAL;
    LTA_CURRENT_MILLIS = millis() + LTA_INTERVAL;
    WEATHER_CURRENT_MILLIS = millis() + WEATHER_INTERVAL;
  }
}

void displayClock()
{
  while(!timeClient.update()) 
  {
    timeClient.forceUpdate();
  }
      
  if(menuValue == 1 && !switchStatus)
  { 
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(MENU_CLOCK_X, MENU_CLOCK_Y, MENU_CLOCK);
    display.setFont(ArialMT_Plain_24);
    display.drawString(CLOCK_OLED_X, CLOCK_OLED_Y, timeClient.getFormattedTime());
    display.setFont(ArialMT_Plain_16);
    display.drawString(DATE_OLED_X, DATE_OLED_Y, timeClient.getFormattedDate());    
    display.display();
  }
}

void displayCurrency()
{
  if(menuValue == 2 && !switchStatus && (millis() - CURRENCY_CURRENT_MILLIS >= CURRENCY_INTERVAL))
  {
    CURRENCY_CURRENT_MILLIS = millis();
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(MENU_CURRENCY_X, MENU_CURRENCY_Y, MENU_CURRENCY);
    display.display();

    WiFiClientSecure httpsClient;
    httpsClient.setFingerprint(CURRENCY_FINGERPRINT);
    httpsClient.setTimeout(15000);   

    int r=0;
    while((!httpsClient.connect(CURRENCY_HOST, CURRENCY_PORT)) && (r < 30))
    {
      delay(100);
      r++;
    }

    if(r!=30) 
    {
      httpsClient.print(CURRENCY_MESSAGE);
      while (httpsClient.connected()) 
      {
        String line = httpsClient.readStringUntil('\n');
        if (line == "\r") 
        {
          break;
        }
      }

      String line;
      while(httpsClient.available())
      {        
        line = line + httpsClient.readStringUntil('\n');
      }
      line = line.substring(4);
      DynamicJsonDocument doc(4028);
      deserializeJson(doc, line);
      JsonObject rateObject = doc["rates"].as<JsonObject>();
      const float value = rateObject["INR"].as<float>();

      display.setFont(ArialMT_Plain_16);
      display.drawString(CURRENCY_FROM_X, CURRENCY_FROM_Y, CURRENCY_FROM);
 
      display.setFont(ArialMT_Plain_16);
      display.drawString(CURRENCY_MID_X, CURRENCY_MID_Y, CURRENCY_MID);

      display.setFont(ArialMT_Plain_16);
      display.drawString(CURRENCY_TO_X, CURRENCY_TO_Y, CURRENCY_TO);

      display.setFont(ArialMT_Plain_16);
      display.drawString(CURRENCY_FROM_VALUE_X, CURRENCY_FROM_VALUE_Y, "1");

      display.setFont(ArialMT_Plain_16);
      display.drawString(CURRENCY_TO_VALUE_X, CURRENCY_TO_VALUE_Y, String(value));
      display.display();

      doc.clear();
    }        
  }
}

void displayBusTiming()
{
  if(menuValue == 3 && !switchStatus && (millis() - LTA_CURRENT_MILLIS >= LTA_INTERVAL))
  {
    LTA_CURRENT_MILLIS = millis();
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(MENU_BUS_X, MENU_BUS_Y, MENU_BUS);
    display.setFont(ArialMT_Plain_16);
    display.display();

    HTTPClient http;
    http.begin(LTA_API);
    http.addHeader(KEY_ACCOUNT,KEYxxx_ACCOUNT_VALUE);
    http.addHeader(KEY_ACCEPT, KEY_ACCEPT_VALUE);
    int httpCode = http.GET();

    if(httpCode > HTTP_CODE_SUCCESS)
    {
      String payload = http.getString();
      DynamicJsonDocument doc(4028);
      deserializeJson(doc, payload);
      JsonArray servicesArray = doc["Services"];
      for(int i = 0; i < servicesArray.size(); i++)
      {
        JsonObject servicesObject = servicesArray[i];
        JsonObject nextBusObject = servicesObject["NextBus"];
        String serviceNoStr = String(servicesObject["ServiceNo"].as<char*>());
        String arrivalTimeStr = String(nextBusObject["EstimatedArrival"].as<char*>());
        int hours = arrivalTimeStr.substring(11, 13).toInt();
        int minutes = arrivalTimeStr.substring(14,16).toInt();
        int seconds = arrivalTimeStr.substring(17,19).toInt();      
        hours = hours - timeClient.getHours();
        minutes = minutes - timeClient.getMinutes();
        seconds = seconds - timeClient.getSeconds();
        if(hours > 0)
        {
           minutes = (hours * 60) + minutes;          
        }
        if(minutes <= 0)
        {
          display.drawString(BUS_X[i], BUS_Y[i], serviceNoStr + ":-");        
        }
        else
        {
          display.drawString(BUS_X[i], BUS_Y[i], serviceNoStr + ":" + minutes);                  
        }
      }
      doc.clear();
    }
    http.end();
    display.display();
  }  
}

void displayWeather()
{
  if(menuValue == 4 && !switchStatus && (millis() - WEATHER_CURRENT_MILLIS >= WEATHER_INTERVAL))
  {
    WEATHER_CURRENT_MILLIS = millis();
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(MENU_WEATHER_X, MENU_WEATHER_Y, MENU_WEATHER);
    display.setFont(ArialMT_Plain_24);
    display.display();

    WiFiClientSecure httpsClient;
    httpsClient.setFingerprint(WEATHER_FINGERPRINT);
    httpsClient.setTimeout(15000);   

    int r=0;
    while((!httpsClient.connect(WEATHER_HOST, WEATHER_PORT)) && (r < 30))
    {
      delay(100);
      r++;
    }

    if(r!=30) 
    {
      httpsClient.print(WEATHER_MESSAGE);
      while (httpsClient.connected()) 
      {
        String line = httpsClient.readStringUntil('\n');
        if (line == "\r") 
        {
          break;
        }
      }

      String line;
      while(httpsClient.available())
      {        
        line = line + httpsClient.readStringUntil('\n');
      }

      DynamicJsonDocument doc(4028);
      deserializeJson(doc, line);
      JsonArray itemsArray = doc["items"].as<JsonArray>();
      Serial.println(itemsArray);
      for(int i = 0; i < itemsArray.size(); i++)
      {
        JsonObject itemObject = itemsArray[i].as<JsonObject>();
        JsonArray readingsArray = itemObject["readings"].as<JsonArray>();
        for(int j=0; j < readingsArray.size(); j++)
        {
          if(strcmp(readingsArray[j]["station_id"].as<char*>(), "S50"))
          {
            int value = readingsArray[j]["value"].as<int>();
            display.setFont(ArialMT_Plain_24);
            display.drawString(WEATHER_VAL_X, WEATHER_VAL_Y, String(value) + " " + "C");
            display.drawCircle(WEATHER_DEG_X, WEATHER_DEG_Y, WEATHER_DEG_RADIUS);
            display.display(); 
            break;           
          }
        }
      }
      doc.clear();
    }
  }  
}
