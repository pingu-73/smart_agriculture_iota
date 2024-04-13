/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL3qk6B8Yn8"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "4rvMCWnDrWa4ozErWlg0mVl9qKWDI0Q2"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define LED_BUILTIN 2
#define DHTTYPE DHT22
#define DHTPIN 26
// #define RELAYPIN 25
const int RELAYPIN = 25;

#define MOISTURE_LOW  40
#define MOISTURE_HIGH  60

bool relayState = false;

DHT dht(DHTPIN, DHTTYPE);

const int HYGRO_PIN = 34;
int hygroval;
float humidity, temp;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Connecting...";
char pass[] = "qyrr8447";

BlynkTimer timer;

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED() {
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

void setup() {
  // Debug console
  Serial.begin(115200);
  delay(100);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  analogReadResolution(12);  //set ADC to use 12 bits
  dht.begin();
}

void loop() {
  Blynk.run();
  timer.run();

  int hygro_analog = analogRead(HYGRO_PIN);
  hygroval = (100 - ((hygro_analog / 4095.00) * 100));
  Serial.print("Soil Moisture % = ");
  Serial.print(hygroval);
  Serial.println("%");
  Blynk.virtualWrite(V4, hygroval);
  delay(1000L);

  humidity = dht.readHumidity();
  temp = dht.readTemperature();

  if (isnan(humidity) || isnan(temp)) {
    if (isnan(humidity)){
      humidity = 0;
    }
    if (isnan(temp))
    {
      temp = 0;
    }
    Serial.println("Failed to read from DHT sensor cause getting 0");
  } else {
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println("˚C");
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V0, humidity);
  }
  delay(1000L);

  Serial.print("Relay State: ");Serial.println(relayState);

  if (hygroval <= MOISTURE_LOW && relayState == 0) {
    Serial.println("vaulve Started, Water Flowing");
    digitalWrite(RELAYPIN, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    relayState = 1;
    delay(400);
  } else if (hygroval >= MOISTURE_HIGH && relayState == 1) {
    Serial.println("Vaulve Stopped, Water Not Flowing");
    Blynk.virtualWrite(V3, HIGH);
    digitalWrite(RELAYPIN, HIGH);
    relayState = 0;
    digitalWrite(LED_BUILTIN, LOW); // Turn off LED
    delay(400);
  } //else {
  //   digitalWrite(RELAYPIN, HIGH);
  //   Blynk.virtualWrite(V3, HIGH);
  //   digitalWrite(LED_BUILTIN, LOW); // Turn off LED
  //   relayState = 0;
  // }
  delay(1000L);
}
