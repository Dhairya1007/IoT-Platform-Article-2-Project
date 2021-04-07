/*
 * Title : IoT Platform Article 2 Final Code
 * Author : Dhairya Parikh
*/

// ########################################################################################

// Importing all the required Libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// defining permanent value variable for DHT Type. 
#define DHTTYPE DHT11   

// ########################################################################################

// ########################################################################################

// variables and object initializations

// 1. WiFi Credentials
const char* ssid = "wifi_ssid";
const char* password = "wifi_password";

// 2. MQTT Broker Credentials
const char* mqtt_server = "your_server_id";

/* 
 *  These are optional as we have not set any authentication for our Broker.
 *    const char* mqttUser = "…";   
 *    const char* mqttPassword = "…";   
 */

// 3. Initializes the espClient. 
// Note : You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

// 4. Relay Module Switch configuration and DHT Pin configuration
uint8_t switch1 = D0;
uint8_t switch2 = D1;
uint8_t switch3 = D2;
uint8_t switch4 = D3;
uint8_t DHTpin = D5;

// Timers auxiliary variables for DHT value measurements. 
long now = millis();
long lastMeasure = 0;

DHT dht(DHTpin, DHTTYPE);     // Initialize DHT sensor.

// Callback function: Called every time a message is received on one of the subscribed topics.

void callback(String topic, byte* message, unsigned int length) 
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
 
  // Feel free to add more if statements to control more GPIOs with MQTT
  // If a message is received on some specific topics, 
  // you check if the message is either on or off. 
  // Control the GPIO according to the message
  
  if(topic == "control/relay_1")
  {
      Serial.print("Changing Switch 1 to ");
      if(messageTemp == "1"){
        digitalWrite(switch1, LOW);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(switch1, HIGH);
        Serial.print("Off");
      }
  }
  
  else if(topic == "control/relay_2")
  {
      Serial.print("Changing Switch 2 to ");
      if(messageTemp == "1"){
        digitalWrite(switch2, LOW);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(switch2, HIGH);
        Serial.print("Off");
      }
  }

  else if(topic== "control/relay_3")
  {
      Serial.print("Changing Switch 3 to ");
      if(messageTemp == "1"){
        digitalWrite(switch3, LOW);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(switch3, HIGH);
        Serial.print("Off");
      }
  }

  else if(topic== "control/relay_4")
  {
      Serial.print("Changing Switch 4 to ");
      if(messageTemp == "1"){
        digitalWrite(switch4, LOW);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(switch4, HIGH);
        Serial.print("Off");
      }
  }

  Serial.println();
}

// ########################################################################################

// ########################################################################################

// Reconnect Function : Called when the client disconnects from the MQTT Broker. This function
// helps in reconnecting to it. 

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if(client.connect("test_client_2")) 
    {
      Serial.println("connected");
      client.publish("outTopic", "Reconnected!");

      client.subscribe("control/relay_1");
      client.subscribe("control/relay_2");
      client.subscribe("control/relay_3");
      client.subscribe("control/relay_4");
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

// ########################################################################################

// ########################################################################################

// Wifi Setup Function : To connect to a WiFi network (credentials defined above)

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
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// ########################################################################################

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs

void setup() 
{
  // GPIO mode set and initilization-------------------------------------------------------
  
  pinMode(switch1, OUTPUT);
  pinMode(switch2, OUTPUT);
  pinMode(switch3, OUTPUT);
  pinMode(switch4, OUTPUT);
  dht.begin();

  //Turn all of them off initially : LOW ACTIVE
  digitalWrite(switch1, HIGH);
  digitalWrite(switch2, HIGH);
  digitalWrite(switch3, HIGH);
  digitalWrite(switch4, HIGH);
  
  // --------------------------------------------------------------------------------------
  
  Serial.begin(115200);       // Begin the Serial Monitor

  // Connect to WiFi and setup MQTT Server Credentials
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Connect to our Broker
  
  while (!client.connected()) 
  {
    Serial.println("Connecting to MQTT...");
    
    /*
     * If you are using username and password authentication, the statement will be
     * if (client.connect("test_client", mqttUser, mqttPassword ))
     * NOT
     * if (client.connect("test_client"))
     * 
     * NOTE : The Client Name should be unique.
     */
     
    if (client.connect("test_client_1")) 
    {
      client.publish("outTopic", "Connected!");
      Serial.println("connected");  
    } 
    else 
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  // Subscribe to all the relay related topics. 
  client.subscribe("control/relay_1");
  client.subscribe("control/relay_2");
  client.subscribe("control/relay_3");
  client.subscribe("control/relay_4");

}

// ########################################################################################

// ########################################################################################

// The loop function is the one which always runs on our device. We want to:
// 1. Send the DHT Sensor readings every 2 seconds.
// 2. Make Sure our device is connected to MQTT Server. If not, call the reconnect() function.

void loop() 
{
  if (!client.connected()) {
    reconnect();
  }
    
  client.loop();
  
  now = millis();
  // Publishes new temperature and humidity every 2 seconds
  if (now - lastMeasure > 2000) 
  {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    
 
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) 
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
 
    // Computes temperature values in Celsius
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);
 
    // Publishes Temperature and Humidity values
    client.publish("monitor/temperature", temperatureTemp);
    client.publish("monitor/humidity", humidityTemp);
    
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t Temperature: ");
    Serial.print(t);
    Serial.print(" *C "); 
    Serial.println();  
  }
}

// ########################################################################################
