#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 7
#define DHTTYPE DHT11

#include <WiFi1010.h>
int status = WL_IDLE_STATUS;

#include <PubSubClient.h>

char ssid[] = "toolbox";
char pass[] = "Toolbox.Torino";

const char mqtt_server[] = "10.100.15.54";
const int mqtt_port = 1883;
const char client_name[] = "thn4-mkr1010";
const char topic[] = "/sigfox/hacknight/devices/thn4-mkr1010/up";

DHT_Unified dht(DHTPIN, DHTTYPE);
WiFiClient net;
PubSubClient client(net);

int lastMsg;

typedef struct __attribute__ ((packed)) kiotlog_payload {
  int16_t dhtTemperature;
  uint16_t dhtHumidity;
} KiotlogPayload;

KiotlogPayload klPayload;
int klPayloadLen = sizeof(klPayload);
char *klPayloadBuf = (char *)&klPayload;

void setup() {
  Serial.begin(115200);
  //  while (!Serial) {}
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  dht.begin();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    setup_wifi();
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

/// Convert byte array to hex string
String hexilify(const char* buf, const size_t len)
{
  String hex;

  hex.reserve(len * 2);

  for (size_t i = 0; i < len; i++) {
    if (buf[i] < 16)
      hex += "0";
    hex += String(buf[i], HEX);
  }

  return hex;
}

// Fill payload struct with measured data
void fillPayload()
{
  sensors_event_t event_temp, event_hum;
  dht.temperature().getEvent(&event_temp);
  dht.humidity().getEvent(&event_hum);

  // Send previous measures if current ones are failing
  klPayload.dhtTemperature = isnan(event_temp.temperature) ? klPayload.dhtTemperature : event_temp.temperature;
  klPayload.dhtHumidity = isnan(event_hum.relative_humidity) ? klPayload.dhtHumidity : event_hum.relative_humidity;
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(10);

  long now = millis();
  if (now - lastMsg > 5000) {

    fillPayload();

    // Create a simple JSON message
    // with data as hex string
    // as in Sigfox API.
    String msg = "{\"payload_raw\":\"";
    msg += hexilify(klPayloadBuf, klPayloadLen);
    msg += "\"}";

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic, msg.c_str());
    lastMsg = now;
  }
}
