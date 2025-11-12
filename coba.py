import paho.mqtt.client as mqtt
import time
import json
import random

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "uts/iot/datasuhu"

# === Fungsi callback saat terkoneksi ke broker ===
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Terhubung ke broker MQTT!")
        client.subscribe(TOPIC)
    else:
        print("Gagal konek, code:", rc)

# === Fungsi callback saat pesan diterima ===
def on_message(client, userdata, msg):
    print(f"Pesan diterima dari {msg.topic}: {msg.payload.decode()}")

# === Setup client MQTT ===
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print("🔌 Menghubungkan ke broker...")
client.connect(BROKER, PORT, 60)

client.loop_start()  # Jalankan loop MQTT di thread lain

try:
    while True:
        # Buat data dummy
        suhu = round(random.uniform(20.0, 35.0), 2)
        kelembapan = round(random.uniform(50.0, 90.0), 2)

        payload = {
            "suhu": suhu,
            "kelembapan": kelembapan
        }

        # Kirim ke MQTT broker
        client.publish(TOPIC, json.dumps(payload))
        print(f"Data dikirim -> {payload}")

        time.sleep(2)

except KeyboardInterrupt:
    print("\n Simulasi dihentikan.")
    client.loop_stop()
    client.disconnect()
