var dht;
var mqtt;
var wifi;
var payload;

var wifiSSID = "toolbox";
var wifiPasswd = "Toolbox.Torino";
var mqttBroker = "192.168.2.112";

// Use toBase64 conversion when sending to 'lorawan' targets
// var mqttTopic = "/lorawan/hacknight/devices/tmmf-espr01/up";

// Use toHex conversion when sending to 'sigfox' targets
var mqttTopic = "/sigfox/hacknight/devices/tmmf-espr01/up";

var dhtPin = NodeMCU.D2;

function onInit() {
  dht = require("DHT11").connect(dhtPin);

  wifi = require("Wifi");

  mqtt = require("MQTT").create(mqttBroker);
  mqtt.on("connected", function() {
    console.log("Connected to " + mqttBroker);
  });

  wifi.connect(
    wifiSSID,
    {password: wifiPasswd},
    function(err){
      console.log("connected? err=", err, "info=", wifi.getIP());
      mqtt.connect();
    }
  );
  payload = new DataView(new ArrayBuffer(4));
  readDHT();
  setInterval(sendData, 10000);
}

function readDHT() {
  dht.read(function (a) {
    payload.setInt16(0, a.temp * 100);
    payload.setUint16(2, a.rh * 100);
  });
}

function toHex(p) {
    return Uint8Array(p.buffer).reduce(
      (output, elem) =>
      (output + ('0' + elem.toString(16)).slice(-2)), '');
}

function toBase64(p) {
  return btoa(E.toString(p.buffer));
}

function sendData() {
  // let asciiPayload = toBase64(payload);
  let asciiPayload = toHex(payload);
  let mqttMsg = JSON.stringify({
      payload_raw: asciiPayload
  });

  console.log(mqttMsg);
  mqtt.publish(mqttTopic, mqttMsg);
  readDHT();
}

//onInit();
