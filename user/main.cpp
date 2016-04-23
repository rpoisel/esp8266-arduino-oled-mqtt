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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OLED_128x64.h>
#include <Wire.h>

// Update these with values suitable for your network.

const char* ssid = "...";
const char* password = "...";
const char* mqtt_server = "hostname.domain";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);

void setup()
{
  /** WiFi, MQTT, etc. **/
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  /** DISPLAY **/

  Wire.begin(2 /* sda */, 4 /* scl */);

  oled_setup();

  print_text(1, " OLIMEX");
  print_smtext(3, (unsigned char *) "     Hello World!", 1);
  print_smtext(5, (unsigned char *) "        Sensei will", 1);
  print_smtext(6, (unsigned char *) "        teach you the", 1);
  print_smtext(7, (unsigned char *) "        Olimexino way", 1);

  drawLine(1, 25, 128, 25);

  drawEllipse(20, 70, 5, 20);
  drawLine(23, 52, 37, 60);
  drawLine(17, 52, 10, 64);

  drawCircle(20, 40, 10);
  drawLine(15, 38, 18, 38);
  drawLine(22, 38, 25, 38);
  drawLine(20, 40, 21, 42);
  drawLine(15, 43, 16, 45);
  drawLine(17, 46, 20, 47);

  drawTriangle(5, 35, 20, 28, 35, 35);

  drawRectangle(37, 55, 40, 65);
  drawSolidRectangle(37, 54, 40, 30);

  //invert_screen();
  //normal_screen();

  oled_update();
  delay(200);
}

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char) payload[0] == '1')
  {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  }
  else
  {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

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
void loop()
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
}
