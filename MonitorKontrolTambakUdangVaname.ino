#include <WiFi.h>

#include "ThingsBoard.h"
#include "DHTesp.h"

#define CURRENT_FIRMWARE_TITLE    "TEST"
#define CURRENT_FIRMWARE_VERSION  "1.0.0"

#define WIFI_SSID           "Wokwi-GUEST"
#define WIFI_PASSWORD       ""

#define TOKEN               "?????????????????"
#define THINGSBOARD_SERVER  "thingsboard.cloud"

const int DHT_PIN = 15;
const int PIN_RELAY = 5;
const int PIN_RELAY_ = 5;

DHTesp dhtSensor;

WiFiClient espClient;
ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;

int motor_status;

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  Serial.println();
  //InitWiFi();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  pinMode(PIN_RELAY, OUTPUT);
}

void loop() {
  delay(1000);

  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  Serial.print("Suhu : "+String(data.temperature)+ "°C");

  if(data.temperature < 26 || data.temperature > 34){
    digitalWrite(PIN_RELAY, LOW);
    motor_status = 1;
    

  }else{
    digitalWrite(PIN_RELAY, HIGH);
    motor_status = 0;
  }

  Serial.print(" | Status Motor : "+String(motor_status == 1 ? "Motor Menyala" : "Motor Mati "));

  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Koneksi ke: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" dengan token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Gagal untuk mengkoneksikan");
      return;
    }
    tb.sendTelemetryFloat("latitude", 	7.46725893);
    tb.sendTelemetryFloat("longitude", 	112.7747726);
  }

  Serial.println(" | Mengirim data...");

  switch(motor_status){
    case 1 : tb.sendTelemetryString("motor_status", "Motor Menyala");break;
    case 0 : tb.sendTelemetryString("motor_status", "Motor Mati");break;
    default : Serial.println("Status motor belum didefinisikan");
  }
  
  tb.sendTelemetryInt("temperature", data.temperature);
  tb.loop();
}

void InitWiFi()
{
  Serial.println("Sedang Mengkoneksikan ke Akses Point ...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }
  Serial.println("Terhubung ke Akses Poin");
}

void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  Serial.println("Terhubung ke Akses Poin");
  }
}