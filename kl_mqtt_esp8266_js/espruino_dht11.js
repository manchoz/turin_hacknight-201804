var dht;
var wifi;
var mqtt;

var payload;

const dhtPin = NodeMCU.D3;

const wifiSSID = "toolbox";
const wifiPasswd = "Toolbox.Torino";

const mqttBroker = "10.100.15.54";
const deviceID = "tmmf-espr01";
const mqttTopic = `/lorawan/hacknight/devices/${deviceID}/up`;

function onInit() {
  wifi = require("Wifi");
  mqtt = require("MQTT").create(mqttBroker);
  dht = require("DHT11").connect(dhtPin);

  readDHT();

  mqtt.on("connected", function() {
    console.log("Connected to " + mqttBroker);
  });

  mqtt.on('disconnected', function() {
    console.log("MQTT disconnected... reconnecting.");
    setTimeout(function() {
      mqtt.connect();
    }, 1000);
  });

  wifi.on('disconnected', function(details) {
    connectWiFi();
  });
  connectWiFi();

  payload = new DataView(new ArrayBuffer(4));
  setInterval(sendData, 10 * 1000);
}

function connectWiFi() {
  wifi.connect(
    wifiSSID,
    {password: wifiPasswd},
    function(err){
      console.log("connected? err=", err, "info=", wifi.getIP());
      mqtt.connect();
    }
  );
}

function readDHT() {
  dht.read(function (a) {
    console.log("Temp: " + a.temp + " RH: " + a.rh);
    payload.setInt16(0, a.temp * 100);
    payload.setUint16(2, a.rh * 100);
  });
}

function toBase64(p) {
  return btoa(E.toString(p.buffer));
}

function sendData() {
  var b64Payload = toBase64(payload);
  var mqttMsg = JSON.stringify({
      payload_raw: b64Payload
  });

  console.log(mqttMsg);
  mqtt.publish(mqttTopic, mqttMsg);
  readDHT();
}
