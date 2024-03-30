/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define DHTTYPE DHT22
#define DHTPIN 25 //connect your dht to D25

DHT dht(DHTPIN, DHTTYPE);

const int HYGRO_PIN = 34;
int hygroval;
float humidity, temp;



// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = ""; //enter wifi name
char pass[] = ""; //enter wifi password

BlynkTimer timer;



// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED() {
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
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
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  analogReadResolution(12);  //set ADC to use 12 bits
  // Setup a function to be called every second
  dht.begin();
  timer.setInterval(1000L, hygro_calibrate);
  timer.setInterval(1000L, dht_sens);
}

void loop() {
  Blynk.run();
  timer.run();
  // hygro_calibrate();
  // dht_sens();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

void hygro_calibrate() {
  int hygro_analog = analogRead(HYGRO_PIN);
  hygroval = (100 - ((hygro_analog / 4095.00) * 100));
  Serial.print("Soil Moisture % = ");
  Serial.print(hygroval);
  Serial.println("%");
  Blynk.virtualWrite(V4, hygroval);  // Send hygroval to pin V4
  delay(100);
}

void dht_sens() {
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
  }else{
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println("˚C");
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V0, humidity);
  }
}
