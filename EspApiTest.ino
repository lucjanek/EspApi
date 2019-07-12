#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// WIFI network connection credentials
const char* ssid = "*******";
const char* password = "********";

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#define D3 0
#define D4 2 
#define D5 14 
#define D7 13 
#define D8 15 
#define TFT_CS     D8
#define TFT_RST    D4  
#define TFT_DC     D3
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST); // Instance of the ST7735 object that receives as arguments control pins
#define TFT_SCLK D5   
#define TFT_MOSI D7

void setup() 
{
  Serial.begin(115200);
  
  tft.initR(INITR_BLACKTAB); // Initialize the screen with black background
  tft.setTextSize(1);
  tft.setTextColor(ST7735_BLUE);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextWrap(true);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting...");
    tft.println("Connecting...");
  }
  Serial.println("Connected");
  tft.println("Connected");
}

void loop() 
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    HTTPClient http; //Object of class HTTPClient
    http.begin("http://api.weatherbit.io/v2.0/current?city_id=758626&lang=pl&key=*******************"); // weatherbit.io API key

    HTTPClient stan; //Object of class HTTPClient
    stan.begin("http://groszek.ml/*******************/isHardwareConnected"); // Blynk project API key

    HTTPClient temp; //Object of class HTTPClient
    temp.begin("http://groszek.ml/********************/get/v0"); // Blynk project API key

    HTTPClient humi; //Object of class HTTPClient
    humi.begin("http://groszek.ml/*******************/get/v1");  // Blynk project API key
    
    int httpCode = http.GET();
    stan.GET(); temp.GET(); humi.GET();
    Serial.println(stan.getString());
    if (httpCode > 0) 
    {
      //const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
      DynamicJsonDocument root(4096);
      DeserializationError error = deserializeJson(root, http.getString());
      if (error)
        return;
      JsonArray repos = root["data"];

      for (JsonObject repo : repos) {
        Serial.print("Miejscowość: ");
        Serial.println(repo["city_name"].as<char *>());
        Serial.print("Temperatura: ");
        Serial.println(repo["temp"].as<double>());
        Serial.print("Temperatura odczuwalna: ");
        Serial.println(repo["app_temp"].as<double>());
        Serial.print("Opis: ");
        Serial.println(repo["weather"]["description"].as<char *>());
        Serial.print("Wilgotność: ");
        Serial.println(repo["rh"].as<int>());
        Serial.print("Ciśnienie: ");
        Serial.println(repo["slp"].as<double>());

        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(0,0);
        
        tft.setTextColor(ST7735_BLUE);
        tft.println("Miejscowosc:");
        tft.setTextColor(ST7735_WHITE);
        tft.println("Sokolow Podlaski");
        
        tft.setTextColor(ST7735_BLUE);
        tft.println("Temperatura:");
        tft.setTextColor(ST7735_WHITE);
        tft.print(repo["temp"].as<double>());
        tft.println(" C");
        
        tft.setTextColor(ST7735_BLUE);
        tft.println("Temperatura");
        tft.println("odczuwalna:");
        tft.setTextColor(ST7735_WHITE);
        tft.print(repo["app_temp"].as<double>());
        tft.println(" C");
        
        tft.setTextColor(ST7735_BLUE);
        tft.println("Opis: ");
        tft.setTextColor(ST7735_WHITE);
        tft.println(repo["weather"]["description"].as<char *>());
        
        tft.setTextColor(ST7735_BLUE);
        tft.println("Wilgotnosc: ");
        tft.setTextColor(ST7735_WHITE);
        tft.print(repo["rh"].as<int>());
        tft.println(" %");
        
        tft.setTextColor(ST7735_BLUE);
        tft.println("Cisnienie: ");
        tft.setTextColor(ST7735_WHITE);
        tft.print(repo["slp"].as<double>());
        tft.println(" hPa");

        if (stan.getString() == "true")
        {
          tft.setTextColor(ST7735_BLUE);
          tft.println("Status:");
          tft.setTextColor(ST7735_GREEN);
          tft.println("Polaczono");

          tft.setTextColor(ST7735_BLUE);
          tft.println("Temperatura(dzialka):");
          tft.setTextColor(ST7735_WHITE);
          tft.print(temp.getString().substring(2,6));
          tft.println(" C");

          tft.setTextColor(ST7735_BLUE);
          tft.println("Wilgotnosc(dzialka):");
          tft.setTextColor(ST7735_WHITE);
          tft.print(humi.getString().substring(2,6));
          tft.println(" %");
        }
        else
        {
          tft.setTextColor(ST7735_BLUE);
          tft.println("Status: ");
          tft.setTextColor(ST7735_RED);
          tft.println("Niepolaczono");
        }
          
        
      }
    }
    http.end(); stan.end(); temp.end(); humi.end(); // Close connection
  }
  delay(3*60*1000);
}
