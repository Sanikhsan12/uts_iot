// ! Config pin
const int potensio_slider = A0; // * pengganti sensor dht
const int pin_led[] = {5,10,12}; // * hijau 5, kuning 10, merah 12
const int relay_pompa = 7;
const int pin_buzzer = 9;

// ! setup
void setup(){
    Serial.begin(9600);

    // ? Inisialisasi lampu led
    for (int i = 0; i < 3; i++){
        pinMode(pin_led[i], OUTPUT);
    }

    // ? Inisialisasi relay pompa
    pinMode(relay_pompa, OUTPUT);
    digitalWrite(relay_pompa, HIGH); // * matikan pompa

    // ? Inisialisasi buzzer
    pinMode(pin_buzzer, OUTPUT);
    digitalWrite(pin_buzzer, LOW); // * matikan buzzer
}

void cekSuhu(int suhu){
    // ? Cek suhu dan atur led, pompa, dan buzzer
    if (suhu < 30){
        // * Suhu rendah
        digitalWrite(pin_led[0], HIGH); 
        digitalWrite(pin_led[1], LOW);
        digitalWrite(pin_led[2], LOW);
        digitalWrite(relay_pompa, HIGH); 
    } else if (suhu >= 30 && suhu <= 35){
        // * Suhu sedang
        digitalWrite(pin_led[0], LOW);
        digitalWrite(pin_led[1], HIGH); 
        digitalWrite(pin_led[2], LOW);
        digitalWrite(relay_pompa, LOW); 
    } else if (suhu > 35){
        // * Suhu tinggi
        digitalWrite(pin_led[0], LOW);
        digitalWrite(pin_led[1], LOW);
        digitalWrite(pin_led[2], HIGH); 
        digitalWrite(relay_pompa, LOW); 
        tone(pin_buzzer, 262, 250); 
    }
}

// ! loop
void loop(){
    // ? Baca nilai potensio sebagai pengganti sensor suhu
    int potValue = analogRead(potensio_slider);
    int suhu = map(potValue, 0, 1023, 20, 45); // * mapping ke rentang suhu 20-45 derajat Celsius

    // ? Tampilkan suhu di serial monitor
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.println(" C");

    // ? Cek suhu dan atur perangkat
    cekSuhu(suhu);

    // ? Delay sebelum pembacaan berikutnya
    delay(2000);
}