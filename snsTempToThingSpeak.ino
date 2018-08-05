#include <ESP8266WiFi.h>
#include <TaskScheduler.h>

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

// setup sensor
#include "DHTesp.h"
DHTesp dht;

// Wifi info
const char* ssid     = "LanDownUnder";
const char* password = "beandip123";

// ThingSpeak info
const char* apiKey = "WGIUBQB5D6FNYFPL";
const char* resource = "/update?api_key=";
const char* server = "api.thingspeak.com";

// Task Scheduler setup
void logStateCallback();
Task logState(300000, TASK_FOREVER, &logStateCallback);
Scheduler runner;

// what happens every 5 min
void logStateCallback() {
  initWifi();
  makeHTTPRequest();
}

void setup() {
  Serial.begin(115200);
  delay(10);

  dht.setup(12, DHTesp::DHT22); // Connect DHT sensor to GPIO 17

  // adding task to scheduler
  runner.init();
  runner.addTask(logState);
  logState.enable();

  // log initial reading
  initWifi();
  makeHTTPRequest();

}

void loop() {
  runner.execute();
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to wifi");
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

// Make an HTTP request to Thing Speak
void makeHTTPRequest() {
  // read sensor data
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  // check if sensor is working
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
  }

  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect to ThingSpeak");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

  // sending data to ThingSpeak
  client.print(String("GET ") + resource + apiKey + "&field1=" + dht.toFahrenheit(temperature) + "&field2=" + humidity +
               " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Connection: close\r\n\r\n");

  int timeout = 5 * 10; // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    Serial.println("No response from ThingSpeak");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("\nclosing connection");
  client.stop();
}
