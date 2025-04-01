#include <WiFi.h>
#include <HttpClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

// WiFi credentials
char ssid[] = "iPhone";
char pass[] = "ajHsb123";

// AWS EC2 IP and port
IPAddress ec2IP(3, 141, 193, 37);  // Replace with your public EC2 IP
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

  // Send HTTP GET request
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


/*
include <WiFi.h>
#include <HttpClient.h>
#include <Adafruit_Sensor.h> 
//#include <Adafruit_AHTX0.h>
#include <DHT20.h>

// This example downloads the URL "http://arduino.cc/"

//char ssid[] = "Michael";    // your network SSID (name) 
//char pass[] = "o61be9cbwd3tj"; // your network password (use for WPA, or use as key for WEP)

char ssid[] = "iPhone";    // your network SSID (name) 
char pass[] = "ajHsb123"; // your network password (use for WPA, or use as key for WEP)

// Name of the server we want to connect to
const char *kHostname = "ec2-3-141-193-37.us-east-2.compute.amazonaws.com";
// Path to Download (this is the bit after the hostname in the URL
// that you want to download
const char *kPath = "?val=50"; // replace with var=XX - done?

const int ec2port = 5000; // ***NEW***

//char[] pathData = kPath + "?val=50"; 
IPAddress ourEc2IP(3,141,193,37);

DHT20 dht20(&Wire);

// 3.141.193.37

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

void setup() {
  Serial.begin(9600);

  // We start by connecting to a WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  Wire.begin();
  if (dht20.begin() != 0) {
    Serial.println("DHT20 not found");
    while (1);
  } else {
    Serial.println("DHT20 found");
  }

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  float temp = dht20.getTemperature();
  float hum = dht20.getHumidity();

  Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\n", temp, hum);

  String url = "/?var=Temp:" + String(temp, 2) + "_Hum:" + String(hum, 2);
  int err =0;
  
  WiFiClient c;
  HttpClient http(c);
  
  //char[] pathData = kPath + "?val=50";
  //IPAddress ourEc2IP(3,141,193,37); // ***NEW*** MOVED UP TO FRONT

  // 3.141.193.37

  //err = http.get(ourEc2IP, NULL, 5000, "/?var=50", NULL); // change kHostname with aws ec2 public ip - done?

  err = http.get(ourEc2IP, NULL, 5000, url.c_str(), NULL);

  Serial.print("Connecting to: http://");
  Serial.print(ourEc2IP);
  Serial.println(":5000/?val=50");
  

  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
               
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();

  // And just stop, now that we've tried a download
  //while(1);
  delay(5000);
}
  */