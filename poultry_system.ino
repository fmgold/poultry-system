#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "" //username 
#define WIFI_PASSWORD ""  //password

/* 2. Define the API Key */
#define API_KEY "" //API
//-----------------------------------------------------------------------
/* 3. Define the RTDB URL */
#define DATABASE_URL "" //Database url
//-----------------------------------------------------------------------

FirebaseJson jsonData;
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;


#define DHTPIN 19      // what pin we're connected to
#define DHTTYPE DHT11  // DHT 22  (AM2302)
//
DHT dht(DHTPIN, DHTTYPE);

int led = 18;
int bulb = 5;
int ldr = 34;


void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(led, OUTPUT);
  pinMode(bulb, OUTPUT);
  pinMode(ldr, INPUT);
  //dht.begin();
  lcd.print(".....SYSTEM......");
  lcd.setCursor(0, 1);
  lcd.print("....BOOTING......");
  delay(2000);
  lcd.clear();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    lcd.print(".");
    delay(300);
  }
  lcd.clear();
  lcd.print("connected");
  //-----------------------------------------------------------------------
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  //-----------------------------------------------------------------------
  /*Or Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signUp ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  delay(1000);

}

void loop() {
  float ldr_value = analogRead(ldr);
  float humidity_value = dht.readHumidity();
  float temperature_value = dht.readTemperature();
  float percentage = map(ldr_value, 0, 4095, 0, 99);
  Serial.print("LDR Value:");
  Serial.println(ldr_value);
  Serial.print("Humidity Value:");
  Serial.println(humidity_value);
  Serial.print("Temperature Value:");
  Serial.println(temperature_value);
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature_value);
  lcd.print(" | ");
  lcd.print("H:");
  lcd.print(humidity_value);
  lcd.setCursor(0, 1);
  lcd.print("LDR:");
  lcd.print(percentage);

  //CONDITIONAL STATEMENT
  if (temperature_value >= 35) {
    digitalWrite(bulb, LOW);
    lcd.setCursor(0, 0);
    lcd.print("TEMPERATURE");
    lcd.setCursor(0, 1);
    lcd.print("IS HIGH");
    delay(500);
  }
  else {
    digitalWrite(bulb, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperature_value);
    lcd.print(" | ");
    lcd.print("H:");
    lcd.print(humidity_value);
    lcd.setCursor(0, 1);
    lcd.print("LDR:");
    lcd.print(percentage);
    //lcd.clear();
    delay(500);

  }
  //GETTING DATA FROM FIREBASE
  //  if (Firebase.RTDB.getString(&fbdo, "/P1")) {
  //    String Status = fbdo.to<String>();
  //    if (Status == "1") {
  //      digitalWrite(LIGHT_A, HIGH);
  //      Serial.println("light A on");
  //    }
  //    if (Status == "0") {
  //      digitalWrite(LIGHT_A, LOW);
  //      Serial.println("light A off");
  //    }
  //  }
  //
  //
  //  //PUSHING TO FIREBASE REALTIME
  Firebase.RTDB.setFloat(&fbdo, "/Poultry/Temperature", temperature_value);
  Firebase.RTDB.setFloat(&fbdo, "/Poultry/Humidity", humidity_value);
  Firebase.RTDB.setFloat(&fbdo, "/Poultry/LDR", percentage);



  delay(500);
  lcd.clear();
}
