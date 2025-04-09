#include <WiFi.h>
#include <HttpClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

// WiFi credentials
char ssid[] = "iPhone";
char pass[] = "ajHsb123";

// AWS EC2 IP and port
IPAddress ec2IP(3, 141, 193, 37);  
const int ec2Port = 5000;

const int kNetworkTimeout = 30000;
const int kNetworkDelay = 1000;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Init sensor
  if (!aht.begin()) {
    Serial.println("❌ AHT20 sensor not found");
    while (1) delay(10);
  }
  Serial.println("✅ AHT20 sensor initialized");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  Serial.printf("🌡️ Temp: %.2f °C | 💧 Humidity: %.2f %%\n", temp.temperature, humidity.relative_humidity);

  // Build query string
  String url = "/?var=Temp:" + String(temp.temperature, 2) + "_Hum:" + String(humidity.relative_humidity, 2);

  WiFiClient client;
  HttpClient http(client);

  Serial.print("Sending request to: http://");
  Serial.print(ec2IP);
  Serial.print(":");
  Serial.println(ec2Port);
  Serial.println("Path: " + url);

  // send HTTP GET request
  int err = http.get(ec2IP, NULL, ec2Port, url.c_str(), NULL);

  if (err == 0) {
    Serial.println("✅ HTTP request sent");
    int statusCode = http.responseStatusCode();
    Serial.print("Status code: ");
    Serial.println(statusCode);

    err = http.skipResponseHeaders();
    if (err >= 0) {
      while (http.available()) {
        char c = http.read();
        Serial.print(c);
      }
    } else {
      Serial.print("Header skip error: ");
      Serial.println(err);
    }
  } else {
    Serial.print("❌ HTTP connection failed: ");
    Serial.println(err);
  }

  http.stop();
  delay(5000); // wait 5 seconds before next reading
}
