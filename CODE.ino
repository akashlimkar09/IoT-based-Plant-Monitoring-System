#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// WiFi credentials
char ssid[] = "Limkar";
char password[] = "09876543";

// Blynk authentication token
#define BLYNK_TEMPLATE_ID "TMPL3Al1VC9XI"
#define BLYNK_TEMPLATE_NAME "ESP32 Plant Monitor"
#define BLYNK_AUTH_TOKEN "YAkjvYD7Cpw3nol6Ulqu4SI2BmqgFafQ"
char auth[] = "YAkjvYD7Cpw3nol6Ulqu4SI2BmqgFafQ";

// Define I2C address for LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pins for Soil Moisture Sensor and Valve
#define SOIL_MOISTURE_SENSOR_PIN 32
#define VALVE_PIN 27
#define DHT_PIN 23

// Define Soil Moisture Sensor Variables
int soilMoistureThreshold = 380 ;
int soilMoistureValue;

// Define DHT11 Sensor Variables
DHT dht(DHT_PIN, DHT11);
float temperatureValue;
float humidityValue;

void setup() {
  Serial.begin(115200);

  // Setup the Soil Moisture Sensor and Valve Pins
  pinMode(SOIL_MOISTURE_SENSOR_PIN, INPUT);
  pinMode(VALVE_PIN, OUTPUT);

  // Initialize the LCD display
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Soil Moisture");
  lcd.print(soilMoistureValue);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Connect to Blynk
  Blynk.begin(auth, WiFi.SSID().c_str(), WiFi.psk().c_str());
  Serial.println("Connected to Blynk");

  // Start the DHT11 sensor
  dht.begin();
}

void loop() {
  // Get Soil Moisture Value
  soilMoistureValue = analogRead(SOIL_MOISTURE_SENSOR_PIN);
  Serial.print("Soil Moisture: ");
  Serial.println(1000000/soilMoistureValue);

  // Determine if the Soil Moisture is below the threshold
  if(1000000/soilMoistureValue < soilMoistureThreshold){
    // Turn Valve On
    digitalWrite(VALVE_PIN, HIGH);
    lcd.setCursor(0,1);
    lcd.print("Valve: ON ");
    Serial.println("Valve On");
  }
  else{
    // Turn Valve Off
    digitalWrite(VALVE_PIN, LOW);
    lcd.setCursor(0,1);
    lcd.print("Valve: OFF");
    Serial.println("Valve Off");
  }

  // Get Temperature and Humidity from DHT11 sensor
  humidityValue = dht.readHumidity();
  temperatureValue = dht.readTemperature();

  // Check if the sensor is reading correctly
  if (isnan(humidityValue) || isnan(temperatureValue)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidityValue);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.println(" *C");

  lcd.setCursor(0,0);
  lcd.print("Soil Moist: ");
  lcd.print(1000000/soilMoistureValue);

  // Send Temperature and Humidity values to Blynk
  Blynk.virtualWrite(V5, humidityValue);
  Blynk.virtualWrite(V6, temperatureValue);
  Blynk.virtualWrite(V4, 1000000/soilMoistureValue);
  //Blynk.virtualWrite(V0,VALVE_PIN);

  delay(1000);
}
