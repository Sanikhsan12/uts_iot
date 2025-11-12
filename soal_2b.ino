#include <WiFi.h>
#include <PubSubClient.h>

// Pin
const int potSuhu = 34;
const int potKelembapan = 35;

// Config
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "uts/iot/datasuhu"; 

WiFiClient espClient;
PubSubClient client(espClient);

// Wifi Setup
void setup_wifi() {
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi terkoneksi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32ClientUTS")) {
      Serial.println("Terhubung ke broker HiveMQ!");
    } else {
      Serial.print("Gagal (rc=");
      Serial.print(client.state());
      Serial.println("), coba lagi 5 detik...");
      delay(5000);
    }
  }
}


// Setup
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Baca nilai dari potensiometer
  int rawSuhu = analogRead(potSuhu);
  int rawKelembapan = analogRead(potKelembapan);

  // Konversi ke nilai realistis
  float suhu = map(rawSuhu, 0, 4095, 0, 50);
  float kelembapan = map(rawKelembapan, 0, 4095, 0, 100);

  // Buat payload JSON
  char payload[64];
  snprintf(payload, sizeof(payload), "{\"suhu\":%.2f,\"kelembapan\":%.2f}", suhu, kelembapan);

  // Tampilkan di Serial Monitor
  Serial.println(payload);

  // Kirim ke MQTT
  client.publish(mqtt_topic, payload);
  Serial.println("Data terkirim ke broker!\n");

  delay(3000); 
}
