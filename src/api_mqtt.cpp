#include <PubSubClient.h>
#include <arduino.h>

#include "api_config.hpp"
#include "api_mqtt.hpp"
#include "api_WiFiPlus.hpp"
#include "api_config.hpp"

namespace api_mqtt {

void callback(char* topic, byte* payload, unsigned int length);

WiFiClient wifiIotHubClient;
PubSubClient IotHubClient(wifiIotHubClient);

MqttPubSubPlus mqttPubSub;

// Create a eprom object configured at address 7
MqttPubSubPlus::MqttPubSubPlus()
{
    lastMsg = 0;
    value = 0;
    temperature = 0.0;
    humidity = 0.0;
}

bool MqttPubSubPlus::setupMqtt() 
{
   // Connect to WiFi
    if (!api_WiFiPlus::wiFiPlus.isWiFiConnected())
    {
        return false;
    }

//uint8_t (&get_mqtt_broker())[4];
const char* mqtt_server = "192.168.1.70";

    // Connect to MQTT Broker
    IotHubClient.setServer(mqtt_server, api_config::get_mqttPort());
    while (!IotHubClient.connected()) 
    {
        String client_id = "IotHub-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT as %s...\r\n", client_id.c_str());
        if (IotHubClient.connect(client_id.c_str())) 
        {
            Serial.println("Connected to MQTT broker");
        } 
        else 
        {
            Serial.print("Failed, state: ");
            Serial.print(IotHubClient.state());
            delay(2000);
        }
    }
  
    IotHubClient.setCallback(callback);
    return true;
}

void MqttPubSubPlus::reconnect() 
{
  // Loop until we're reconnected
  while (!IotHubClient.connected()) {
    // Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (IotHubClient.connect("ESP8266Client")) {
    //   Serial.println("connected");
      // Subscribe
      IotHubClient.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(IotHubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
    //   Serial.print("MQTT connected...");
}

bool MqttPubSubPlus::publishWithRetries(char *mqttTopic, char *mqttValueString)
{
  if (!IotHubClient.connected()) 
  {
    api_mqtt::MqttPubSubPlus::reconnect();
  }

  int loopCounter = 0;
  bool published = false;

  do
  {
    loopCounter++;
    published = IotHubClient.publish(mqttTopic, mqttValueString);
    if (!published)
    {
      Serial.print("ERROR: IotHubClient.publish FAILED, count = ");
      Serial.println(loopCounter);
    }
  } while ((loopCounter < 5) && (!published));
  
  if (loopCounter >= 5)
  {
    Serial.println("ERROR: IotHubClient.publish ABORTED -> max retry count hit ");
  }

  return published;
}

void MqttPubSubPlus::updateMqtt(char* timeString, api_AirThingsWavePlus::CurrentValues currentValues, float temperatureEsp32Cpu)
{
  if (!IotHubClient.connected()) 
  {
    reconnect();
  }

  IotHubClient.loop();
    char mqttTopic[40];
    char mqttFloatValueString[8];

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectTemperature());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(currentValues.temperature, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectHumidity());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(currentValues.humidity, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectCO2());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(currentValues.co2, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectShortTermRadon());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(currentValues.radonSTA, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectLongTermRadon());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(currentValues.radonLTA, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectPressure());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(currentValues.pressure, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectVoc());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(currentValues.voc, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), "ESP32CPUTemp");
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    dtostrf(temperatureEsp32Cpu, 1, 2, mqttFloatValueString);
    publishWithRetries(mqttTopic, mqttFloatValueString);

    // Convert the value to a char array
    snprintf(mqttTopic, sizeof(mqttTopic), "%s/%s", api_config::get_mqttTopicAirThings(), api_config::get_mqttTopicAirThingsSubjectTimeDate());
    mqttTopic[sizeof(mqttTopic) - 1] = '\0';
    publishWithRetries(mqttTopic, timeString);
  }

 void callback(char* topic, byte* payload, unsigned int length)
{
//   // Handle received message
}

} // namespace api_mqtt
