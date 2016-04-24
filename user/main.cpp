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
#include <OLED_128x64.h>
#include <FreeSansBold9pt7b.h>

#include "credentials.h"

#define BUILTIN_RELAY 5

WiFiClient espClient;
PubSubClient client(espClient);
Olimex_128x64 display;
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

  pinMode(BUILTIN_RELAY, OUTPUT);

  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(mqtt_callback);
  // subscription to MQTT topics is performed in reconnect()

  display.begin(2 /* sda */, 4 /* scl */);
}

void draw_screen(void)
{
  static uint8_t x = 6;
  static uint8_t delta = 4;

  display.clear_screen();

  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(1, 15);
  display.print("MQTT");
  display.setCursor(115, 15);
  display.print(client.connected() ? "X" : "-");

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
  /* Serial handling */
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  /* Display data handling */
  strncpy(mqtt_topic, (const char*) topic, sizeof(mqtt_topic));
  strncpy(mqtt_msg, (const char*) payload, length < sizeof(mqtt_msg) ? length : sizeof(mqtt_msg));
  mqtt_msg[length < sizeof(mqtt_msg) ? length : sizeof(mqtt_msg) - 1] = '\0';

  /* I/O handling */
  if (mqtt_msg[0] == '1')
  {
    digitalWrite(BUILTIN_RELAY, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  }
  else if (mqtt_msg[0] == '0')
  {
    digitalWrite(BUILTIN_RELAY, LOW);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect()
{
  static long lastConnect = 0;
  long now = millis();
  if (now - lastConnect < 5000)
  {
    return;
  }
  lastConnect = now;
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
  }
}
void loop(void)
{
  static long lastMsg = 0;
  static int value = 0;
  char msg[50] = { '\0' };

  draw_screen();

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
}
