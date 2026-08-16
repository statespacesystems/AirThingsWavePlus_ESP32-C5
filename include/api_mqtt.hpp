// #include <WiFi.h>
// #include <PubSubClient.h>

// namespace api_mqtt {

// WiFiClient espClient;
// PubSubClient client(espClient);

// bool api_mqtt::setup() 
// {
//   // Connect to WiFi
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) 
//   {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("WiFi connected");

//   // Connect to MQTT Broker
//   client.setServer(mqtt_broker, mqtt_port);
//   while (!client.connected()) 
//   {
//     String client_id = "esp32-client-";
//     client_id += String(WiFi.macAddress());
//     Serial.printf("Connecting to MQTT as %s...\n", client_id.c_str());
//     if (client.connect(client_id.c_str())) 
//     {
//       Serial.println("Connected to MQTT broker");
//     } 
//     else 
//     {
//       Serial.print("Failed, state: ");
//       Serial.print(client.state());
//       delay(2000);
//     }
//   }
  
//   client.setCallback(callback);
// }

// void callback(char* topic, byte* payload, unsigned int length) {
//   // Handle received message
// }

// } // namespace api_mqtt