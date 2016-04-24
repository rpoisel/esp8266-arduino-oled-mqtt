/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
 - publishes "hello world" to the topic "outTopic" every two seconds
 - subscribes to the topic "inTopic", printing out any messages
 it receives. NB - it assumes the received payloads are strings not binary
 - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
 else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
 - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
 http://arduino.esp8266.com/stable/package_esp8266com_index.json
 - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
 - Select your ESP8266 in "Tools -> Board"

 */

#include <string.h>
#include <inttypes.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <OLED_128x64.h>
#include <FreeSansBold9pt7b.h>

#include "credentials.h"

WiFiClient espClient;
PubSubClient client(espClient);
Olimex_128x64 display;
long lastMsg = 0;
int value = 0;
char msg[50] = { '\0' };
char mqtt_topic[50] = { '\0' };
char mqtt_msg[50] = { '\0' };

void setup(void);
void loop(void);
void reconnect(void);
void draw_screen(void);
void setup_wifi(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);

void setup(void)
{
  Serial.begin(115200);

  setup_wifi();

  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(mqtt_callback);
  // subscription to MQTT topics is performed in reconnect()

  Wire.begin(2 /* sda */, 4 /* scl */);
  display.begin();
}

void draw_screen(void)
{
  static uint8_t x = 6;
  static uint8_t delta = 4;

  display.clear_screen();

  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(1, 15);
  display.print("MQTT");

  display.drawLine(1, 20, 128, 20, 0);

  display.setFont(0);
  display.setCursor(1, 30);
  display.print("Topic: ");
  display.print(mqtt_topic);
  display.setCursor(1, 40);
  display.print("Msg:   ");
  display.print(mqtt_msg);

  display.drawCircle(x, 59, 5, 0);
  x += delta;
  if (x > 123 || x < 5)
  {
    delta *= -1;
    x += 2 * delta;
  }

  display.display();
}

void setup_wifi(void)
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  strncpy(mqtt_topic, (const char*) topic, sizeof(mqtt_topic));
  strncpy(mqtt_msg, (const char*) payload, length < sizeof(mqtt_msg) ? length : sizeof(mqtt_msg));
  mqtt_msg[length < sizeof(mqtt_msg) ? length : sizeof(mqtt_msg) - 1] = '\0';
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop(void)
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000)
  {
    lastMsg = now;

    ++value;
    sprintf(msg, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }

  draw_screen();
}
