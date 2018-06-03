from umqtt.simple import MQTTClient
from utime import sleep
from json import dumps
from ubinascii import b2a_base64
from ustruct import pack
from ucollections import OrderedDict

import dht
import machine

ESSID = "toolbox"
PASSWORD = "Toolbox.Torino"
BROKER = "192.168.2.112"
CLIENT = "huzzah-mp"
TOPIC = "/lorawan/hacknight/devices/{}/up".format(CLIENT)


def do_connect():
    import network
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('Connecting to network...')
        sta_if.active(True)
        sta_if.connect(ESSID, PASSWORD)
        while not sta_if.isconnected():
            pass
    print('Network Config:', sta_if.ifconfig())


def create_payload(dht_sensor):
    print("Measuring...")
    dht_sensor.measure()
    measures = OrderedDict([
        ('temperature', int(dht_sensor.temperature() * 100)),
        ('humidity', int(dht_sensor.humidity() * 100))
    ])

    values = [v for _, v in measures.items()]
    packed = pack('>HH', *values)
    base64 = b2a_base64(packed)
    return base64.strip()  # dump trailing \n


def send_message(client, msg):

    json_msg = dumps(msg)

    print("Sending...")
    client.connect()
    client.publish(TOPIC.encode(), json_msg.encode())
    client.disconnect()
    sleep(1)


def main():

    # Connect RST to GPIO16 to enable triggering wakeup via alarm.
    rtc = machine.RTC()
    rtc.irq(trigger=rtc.ALARM0, wake=machine.DEEPSLEEP)

    # Pin D2 is GPIO 4
    d = dht.DHT22(machine.Pin(4))

    do_connect()

    client = MQTTClient(CLIENT, BROKER)

    while True:

        msg = {'payload_raw': create_payload(d)}

        send_message(client, msg)

        print('Sleeping...')
        # sleep(5)
        # Wake me up in 5 seconds
        rtc.alarm(rtc.ALARM0, 10000)
        # Now power off everything but RTC
        machine.deepsleep()

        print('Looping...')


if __name__ == "__main__":
    main()
