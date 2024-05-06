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

#define DHTTYPE DHT22
#define DHTPIN 26
#define RELAYPIN 23
#define flowSensorPin 19

volatile int flow_frequency; // Measures flow sensor pulses
unsigned long total_ml; // Total quantity of water in milliliters
unsigned long prevMillis; // Previous millis value for time tracking
const float calibrationFactor = 220; // Calibration factor for the sensor

// #define MOISTURE_LOW  40
// #define MOISTURE_HIGH  60
// const float MOISTURE_LOW = 40;
// const float MOISTURE_HIGH = 60;

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

void flow() { // Interrupt function
  flow_frequency++;
}

void setup() {
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

  pinMode(flowSensorPin, INPUT);
  digitalWrite(flowSensorPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), flow, RISING);
  prevMillis = millis(); // Initialize prevMillis
  analogReadResolution(12);
  pinMode(RELAYPIN, OUTPUT);
  dht.begin();

  // Set timer for relay control every 1 second
  timer.setInterval(10L, relayControl);
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
  delay(2000); // Increased delay to 2000 milliseconds (2 seconds)

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
  delay(2000); // Increased delay to 2000 milliseconds (2 seconds)
}

void relayControl() {
  // Your existing relay control logic can go here
  int hygro_analog = analogRead(HYGRO_PIN);
  if (hygro_analog < 3000 && hygro_analog > 1200) {
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Valve Started, Water Flowing");
    Blynk.virtualWrite(V3, HIGH);

    unsigned long currentMillis = millis();
    unsigned long elapsedTime = currentMillis - prevMillis;

    // Calculate flow rate every second
    if (elapsedTime >= 1000) {
      prevMillis = currentMillis; // Update prevMillis

      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      // Milliliters/second = (flow_frequency / 7.5) * 1000
      float ml_sec = (flow_frequency / calibrationFactor) * 1000.0; // Convert to ml/sec
      Serial.print("Flow rate: ");
      Serial.print(ml_sec);
      Serial.println(" ml/sec");

      // Calculate quantity of water flowed since last pulse
      float flowed_ml = (ml_sec * elapsedTime) / 1000.0; // Convert to ml
      total_ml += flowed_ml; // Accumulate ml
      Serial.print("Quantity flowed since last pulse: ");
      Serial.print(flowed_ml);
      Serial.println(" ml");

      Serial.print("Total quantity: ");
      Serial.print(total_ml);
      Serial.println(" ml");

      flow_frequency = 0; // Reset flow counter
      Blynk.virtualWrite(V2, total_ml);
    }
    if(total_ml > 1000){
      digitalWrite(RELAYPIN, HIGH);
      Serial.println("Total quantity exceeded 10000ml, stopping water");
      total_ml = 0;
      flow_frequency = 0;
      Blynk.virtualWrite(V2, total_ml);
    }
  } else {
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Valve Stopped, Water Not Flowing"); 
    Blynk.virtualWrite(V3, LOW);
  }
}
