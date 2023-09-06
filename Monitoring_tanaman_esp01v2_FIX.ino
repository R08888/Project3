#define BLYNK_TEMPLATE_ID "***********"
#define BLYNK_TEMPLATE_NAME "**********"
#define BLYNK_AUTH_TOKEN "*****************"  // Token untuk komunikasi dengan blynk

#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <DHT.h>

char ssid[] = "************";     // Your WiFi SSID
char pass[] = "************";  // Paswword SSID/WiFi

#include <SoftwareSerial.h>      //Library yang digunakan untuk membuat objek espserial
SoftwareSerial EspSerial(2, 3);  // RX, TX //Mendeskripsikan pin Rx dan Tx yang digunakan oleh modul ESP-01

#define ESP8266_BAUD 9600  //  untuk mengatur kecepatan baud rate pada komunikasi serial dengan perangkat ESP8266.
ESP8266 wifi(&EspSerial);  //untuk mengatur, menginisialisasi, dan berinteraksi dengan modul ESP8266

#define DHTPIN 9           //Mendefinisikan Variabel DHTPIN menggunakan Pin 9
#define DHTTYPE DHT11      //Mendefinisikan Tipe dari sensor yaitu DHT11
DHT dht(DHTPIN, DHTTYPE);  // menginisialisasinya dengan nilai DHTPIN dan DHTTYPE.

const int msensor = A0;  //mendefinisikan varibel msensor dengan tipe data konstan integer pada pin Analog 0
int relay = 8;

int relayBlynk_1;
int otoblynk;

int sensorKelembabanVal1;

float h;  //Kelembapan = Humidity
float t;  //Temperature = Suhu

BlynkTimer timer;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);  // inisialisasi dengan template(Blynk) dan wifi
  // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Waiting for connections...");

  pinMode(msensor, INPUT);  //Sensor Tanah
  pinMode(relay, OUTPUT);   //Relay

  digitalWrite(relay, HIGH);

  dht.begin();

  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, otomatis);
}


void otomatis() {
  if (otoblynk == 1) {

    if ((sensorKelembabanVal1 < 50) || (t > 35) || (h < 75)) {
      digitalWrite(relay, LOW);  // low = nyala
      delay(2000);
    }

    else {
      digitalWrite(relay, HIGH);  //High = Mati
      delay(2000);
    }
  }
}

int convert(int sensor) {
  int output = map(sensor, 1000, 700, 0, 100);  //mapping data
  return output;
}

void sendSensor() {
  h = dht.readHumidity();
  t = dht.readTemperature();

  int msvalue = analogRead(msensor);
  Serial.println(msvalue);
  sensorKelembabanVal1 = convert(msvalue);

  if (isnan(h) || isnan(t)) {  //Nan (Not-a-Number)
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  Blynk.virtualWrite(V1, h);                     //Untuk mengirim data h ke datastream V1
  Blynk.virtualWrite(V2, t);                     //Untuk mengirim data t ke datastream V2
  Blynk.virtualWrite(V3, sensorKelembabanVal1);  //Untuk mengirim data sensorKelembabanVal1 ke datastream V3
}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V7);  //Pemanggilan data pada V7
  Blynk.syncVirtual(V6);  //Pemanggilan data pada V8
}

//Manual Solenoid //kontrol
BLYNK_WRITE(V7) {                     // Lampu
  relayBlynk_1 = param.asInt();       //Menyamakan data variabel pada V7 dengan relayBlynk_1
  digitalWrite(relay, relayBlynk_1);  //
}
BLYNK_WRITE(V6) {  // Otomatis
  otoblynk = param.asInt();
}

void loop() {
  Blynk.run();
  timer.run();
}
