"""
Recieve data over a serial connection from the radio module.
Then this is put into a BLE advertising packet
"""


import serial
from bluetooth import *
import time


# update these appropriately
PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200
service_uuid = '00001800-0000-1000-8000-00805f9b34fb'
SLEEP_TIME = 0.01


# set up the serial port
serial_port = PORT
baud_rate = BAUD_RATE

# open serial port
try:
    ser = serial.Serial(serial_port, baud_rate)
except serial.SerialException:
    print(f"Failed to open port {serial_port}")
    exit(1)

# Configure Bluetooth advertising
bt_name = 'RaspberryPi'

# Start Bluetooth advertising
sock = BluetoothSocket(RFCOMM)
advertise_service(sock, service_name=bt_name,
                  service_uuid=UUID(service_uuid),
                  service_classes=[UUID(service_uuid), SERIAL_PORT_CLASS],
                  profiles=[SERIAL_PORT_PROFILE])

print(f"Advertising Bluetooth service: {bt_name}")


while True:
    # Read data from the serial port
    data = ser.readline().strip()

    if data:
        # Advertise the data over Bluetooth
        sock.send(data)
    else:
        # No data received
        continue
    time.sleep(SLEEP_TIME)
