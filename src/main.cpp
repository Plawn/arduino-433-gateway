#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ESP8266WiFi.h> // Include the Wi-Fi library
#include <Gyver433.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>


String serverName = "http://192.168.2.6:3555/";
const auto ssid = "STARCOM-NET";
const auto password = "EPR408EPR408";
Gyver433_RX<SDA, 12> rx; // указали пин и размер буфера
// Gyver433_TX<2> tx;  // указали пин
// формат пакета для приёма (такой же как отправляется)
struct DataPack
{
  byte id;
  byte counter;
  byte randomNum;
  float temperature;
  uint16_t value;
  // uint32_t time;
};

void connectWifi() {
  WiFi.begin(ssid, password); // Connect to the network
}

void awaitWifi() {
  int i = 0;
  while (!WiFi.isConnected())
  { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }
}

void setup()
{
  connectWifi();
  Serial.begin(9600); // Start the Serial communication to send messages to the computer
  awaitWifi();
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
}


void loop()
{
  // put your main code here, to run repeatedly:
  if (rx.tick())
  {
    DataPack data;
    if (rx.readData(data))
    {
      Serial.printf("id: %x\n", data.id);
      // Serial.println(data.counter);
      // Serial.println(data.randomNum);
      // Serial.printf("analog: %d\n", data.temperature);
      // Serial.println(data.temperature);
      // Serial.println(data.time);
      // Serial.println();

      if (WiFi.isConnected())
      {
        WiFiClient client;
        HTTPClient http;

        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName.c_str());
        JSONVar doc;
        // Set the values in the document
        doc["id"] = data.id;
        doc["temperature"] = data.temperature;
        doc["value"] = data.value;
        // Send HTTP GET request
        auto jsonString = JSON.stringify(doc);

        const auto httpResponseCode = http.POST(jsonString);

        if (httpResponseCode > 0)
        {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          // String payload = http.getString();
          // Serial.println(payload);
        }
        else
        {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();
      } else {
        connectWifi();
        awaitWifi();
      }
    }
    else
    {
      Serial.println("Wrong data");
    }
  }
}