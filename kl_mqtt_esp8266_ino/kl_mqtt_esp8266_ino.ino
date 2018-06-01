#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <base64.h>

//const char* ssid = "toolbox";
//const char* password = "Toolbox.Torino";
//const char* mqtt_server = "ballad.local";

const char* ssid = "Endymion";
const char* password = "Caterina01";
const char* mqtt_server = "192.168.2.112";



const char* client_name = "thn4-ardu";
const char* topic = "/lorawan/hacknight/devices/thn4-ardu/up";

DHT_Unified dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

int lastMsg;

typedef struct __attribute__ ((packed)) kiotlog_payload {
  int16_t dhtTemperature;
  uint16_t dhtHumidity;
} KiotlogPayload;

KiotlogPayload klPayload;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.begin();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(client_name)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(10);
  
  long now = millis();
  if (now - lastMsg > 5000) {
    
    sensors_event_t event_temp, event_hum;
    dht.temperature().getEvent(&event_temp);
    dht.humidity().getEvent(&event_hum);

    klPayload.dhtTemperature = event_temp.temperature;
    klPayload.dhtHumidity = event_hum.relative_humidity;

    String b64Payload = base64::encode((uint8_t *)&klPayload, sizeof(klPayload), false);
    String msg = "{\"payload_raw\":\"" + b64Payload  + "\"}";

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic, msg.c_str());
    lastMsg = now;
  }
}
