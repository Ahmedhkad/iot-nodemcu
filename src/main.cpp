#include <Arduino.h> //for PlatformIO extension in VSCode

#include "settings.h" //GPIO defines, NodeMCU good-pin table
#include "secret.h"   //Wifi and mqtt server info
#include <ESP8266WiFi.h>
#include <PubSubClient.h> //mqtt lib
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h> //works with dht.h
#include "DHT.h"             //Temp sensor

DHT dht(dhtpin, DHT11); //dht Pin , sensor type

const char *ssid = ssidWifi;       //defined on secret.h
const char *password = passWifi;   //defined on secret.h
const char *mqtt_server = mqttURL; //defined on secret.h

WiFiClient espClient;
PubSubClient client(espClient);

boolean button01State;
boolean button02State;
boolean button03State;
boolean oldbutton01State;
boolean oldbutton02State;
boolean oldbutton03State;
boolean relay1State;
boolean relay2State;
boolean relay3State;
boolean relay4State;
boolean relay5State;
boolean relay6State;

int ldrState;

boolean loopLDRTimer = true;
boolean loopDHTTimer = true;
long unsigned int ldrTimer = 2000;
long unsigned int dhtTimer = 5000;

unsigned long lasttime = 0;
unsigned long lasttime2 = 0;
#define MSG_BUFFER_SIZE (120)
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];
int value = 0;

StaticJsonDocument<200> doc;


int device;
int valuejson;

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void readLDR(){
    ldrState = map(analogRead(ldrpin), 0, 1023, 0, 100);
    StaticJsonDocument<250> doc;
    doc["LDR"] = ldrState;
    serializeJson(doc, msg2);
    //      Serial.print(msg2);
    client.publish("ldrTopic", msg2);
}

void readDHT(){
    float h = dht.readHumidity();
    float t = dht.readTemperature();     // Read temperature as Celsius (the default)
    float f = dht.readTemperature(true); // Read temperature as Fahrenheit
    if (isnan(h) || isnan(t) || isnan(f)) { // Check if any reads failed
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    float hif = dht.computeHeatIndex(f, h);        // Compute heat index
    float hic = dht.computeHeatIndex(t, h, false); // Compute heat index in Celsius
                                                   
    StaticJsonDocument<250> doc;
    doc["Humidity"] = h;
    doc["TempC"] = t;
    doc["IndexC"] = hic;
    doc["TempF"] = f;
    doc["IndexF"] = hif;
    serializeJson(doc, msg);
    //    snprintf (msg, MSG_BUFFER_SIZE, "The path is clear 1");
    //    Serial.print(msg);
    client.publish("tempTopic", msg);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Test if parsing succeeds.
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  // Print the values to data types
  device = doc["device"].as<unsigned int>();
  valuejson = doc["value"].as<unsigned int>();
  Serial.print("device: ");
  Serial.print(device);
  Serial.print(" valuejson: ");
  Serial.println(valuejson);

  switch (device)
  {
  case 1:
    if (valuejson == 1)
    {
      digitalWrite(light01, HIGH);
      client.publish("ledTopic", "{\"light01\":\"ON\"}");
    }
    else if (valuejson == 0)
    {
      digitalWrite(light01, LOW);
      client.publish("ledTopic", "{\"light01\":\"OFF\"}");
    }
    break;
  case 2:
    if (valuejson == 1)
    {
      digitalWrite(light02, HIGH);
      client.publish("ledTopic", "{\"light02\":\"ON\"}");
    }
    else if (valuejson == 0)
    {
      digitalWrite(light02, LOW);
      client.publish("ledTopic", "{\"light02\":\"OFF\"}");
    }
    break;
  case 3:
    if (valuejson == 1)
    {
      digitalWrite(light03, HIGH);
      client.publish("ledTopic", "{\"light03\":\"ON\"}");
    }
    else if (valuejson == 0)
    {
      digitalWrite(light03, LOW);
      client.publish("ledTopic", "{\"light03\":\"OFF\"}");
    }
    break;
  case 4:
    if (valuejson == 1)
    {
      digitalWrite(relay01, HIGH);
      client.publish("ledTopic", "{\"relay01\":\"ON\"}");
    }
    else if (valuejson == 0)
    {
      digitalWrite(relay01, LOW);
      client.publish("ledTopic", "{\"relay01\":\"OFF\"}");
    }
    break;
  case 5:
    if (valuejson == 1)
    {
      digitalWrite(relay02, HIGH);
      client.publish("ledTopic", "{\"relay02\":\"ON\"}");
    }
    else if (valuejson == 0)
    {
      digitalWrite(relay02, LOW);
      client.publish("ledTopic", "{\"relay02\":\"OFF\"}");
    }
    break;
    case 6:
      readLDR();
      client.publish("SystemTopic", "{\"readLDR\":\"Sent\"}");
    break;
    case 7:
      readDHT();
      client.publish("SystemTopic", "{\"readDHT\":\"Sent\"}");
    break;
    case 8:
    if (valuejson >= 1)
    {
      loopLDRTimer = true;
      ldrTimer = valuejson * 1000;
      Serial.print("ldrTimer = ");
      Serial.println(ldrTimer);
      client.publish("SystemTopic", "{\"loopLDRTimer\":\"ON\"}");
    }
    else if (valuejson == 0)
    {
      loopLDRTimer = false;
      client.publish("SystemTopic", "{\"loopLDRTimer\":\"OFF\"}");
    }
    break;
    case 9:
    if (valuejson >= 1)
    {
      loopDHTTimer = true;
      dhtTimer = valuejson * 1000;
      Serial.print("dhtTimer = ");
      Serial.println(dhtTimer);

      client.publish("SystemTopic", "{\"loopDHTTimer\":\"ON\"}");
    }
    else if (valuejson == 0)
    {
      loopDHTTimer = false;
      client.publish("SystemTopic", "{\"loopDHTTimer\":\"OFF\"}");
    }
    break;
  default:
    Serial.print("Err device in case-switch invalid.");
    break;
  }
}


void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("mqttClient"))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("LifeTopic", "{\"device\":\"ESPTestClient\"}");
      //      "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}"
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

void setup()
{
  pinMode(light01, OUTPUT);
  pinMode(light02, OUTPUT);
  pinMode(light03, OUTPUT);
  pinMode(relay01, OUTPUT);
  pinMode(relay02, OUTPUT);

  pinMode(dhtpin, INPUT);

  pinMode(button01, INPUT);
  pinMode(button02, INPUT);
  pinMode(button03, INPUT);

  pinMode(ldrpin, INPUT);

  Serial.begin(9600); //debug print on Serial Monitor
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqttPORT);
  client.setCallback(callback);
}


void loop()
{

  if (!client.connected())
  {
    Serial.println("reconnect ...");
    reconnect();
  }
  client.loop();

  button01State = digitalRead(button01);
  if (button01State != oldbutton01State)
  {
    oldbutton01State = button01State;
    Serial.print("button01 read: ");
    Serial.println(button01State);

    if (button01State == LOW)
    {
      client.publish("buttonTopic", "{\"button01\":\"OFF\"}");
    }
    else if (button01State == HIGH)
    {
      client.publish("buttonTopic", "{\"button01\":\"ON\"}");
    }
    delay(50);
  }

  button02State = digitalRead(button02);
  if (button02State != oldbutton02State)
  {
    oldbutton02State = button02State;
    Serial.print("button02  read: ");
    Serial.println(button02State);

    if (button02State == LOW)
    {
      client.publish("buttonTopic", "{\"button02\":\"OFF\"}");
    }
    else if (button02State == HIGH)
    {
      client.publish("buttonTopic", "{\"button02\":\"ON\"}");
    }
    delay(50);
  }

  button03State = digitalRead(button03);
  if (button03State != oldbutton03State)
  {
    oldbutton03State = button03State;
    Serial.print("button03   read: ");
    Serial.println(button03State);

    if (button03State == LOW)
    {
      client.publish("buttonTopic", "{\"button03\":\"OFF\"}");
    }
    else if (button03State == HIGH)
    {
      client.publish("buttonTopic", "{\"button03\":\"ON\"}");
    }
    delay(50);
  }


if(loopLDRTimer == true){
  unsigned long now = millis();
  if (now - lasttime > ldrTimer)
  {
    lasttime = now;
    ldrState = map(analogRead(ldrpin), 0, 1023, 0, 100);
    StaticJsonDocument<250> doc;
    doc["LDR"] = ldrState;
    serializeJson(doc, msg2);
    client.publish("ldrTopic", msg2);
  }
} //end if loop LDR Timer

if(loopDHTTimer == true){
  unsigned long now2 = millis();
  if (now2 - lasttime2 > dhtTimer)
  {
    lasttime2 = now2;
    float h = dht.readHumidity();
    float t = dht.readTemperature();     // Read temperature as Celsius (the default)
    float f = dht.readTemperature(true); // Read temperature as Fahrenheit
    if (isnan(h) || isnan(t) || isnan(f))
    { // Check if any reads failed
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    float hif = dht.computeHeatIndex(f, h);        // Compute heat index
    float hic = dht.computeHeatIndex(t, h, false); // Compute heat index in Celsius

    StaticJsonDocument<250> doc;
    doc["Humidity"] = h;
    doc["TempC"] = t;
    doc["IndexC"] = hic;
    doc["TempF"] = f;
    doc["IndexF"] = hif;
    serializeJson(doc, msg);
    client.publish("tempTopic", msg);
  }
} //end if loop DHT Timer

}
