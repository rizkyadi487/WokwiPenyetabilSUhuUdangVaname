#include <WiFi.h>

#include "ThingsBoard.h"
#include "DHTesp.h"

#define CURRENT_FIRMWARE_TITLE    "TEST"
#define CURRENT_FIRMWARE_VERSION  "1.0.0"

#define WIFI_SSID           "Wokwi-GUEST"
#define WIFI_PASSWORD       ""

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define TOKEN               "2VacTghl150ZqZojc6Pp"
#define THINGSBOARD_SERVER  "thingsboard.cloud"

const int DHT_PIN = 15;
const int PIN_RELAY = 5;

DHTesp dhtSensor;

WiFiClient espClient;
ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;

void InitWiFi()
{
  Serial.println("Sedang Mengkoneksikan ke Akses Point ...");
  // attempt to connect to WiFi network

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

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  Serial.println();
  // InitWiFi();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  pinMode(PIN_RELAY, OUTPUT);
}

void loop() {
  delay(1000);

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
  }
  Serial.println("Mengirim data...");

  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  Serial.print("Suhu : ");
  Serial.println(data.temperature);

  if(data.temperature < 26 || data.temperature > 34){
    digitalWrite(PIN_RELAY, LOW);
    Serial.println("Motor Menyala");
  }else{
    digitalWrite(PIN_RELAY, HIGH);
    Serial.println("Motor Mati");
  }

  tb.sendTelemetryInt("temperature", data.temperature);
  tb.loop();
}
