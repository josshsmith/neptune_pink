"""
Script to recieve ais data through json format.

There will be two threads runnning:
One sending json formatted information via uart
The other will be using a cloud based service to recieve the ais information
"""
import serial
import json
import time
import threading
import queue
from vessel_data_client import get_vessel_data
import os

port = "/dev/tty.usbmodem0010502338821"
BAUD_RATE = 115200
SLEEP_DURATION = 5


ser = serial.Serial(port, BAUD_RATE)
# queue for passing information between threads
q = queue.Queue()

"""
ais_rec()
Finds path to the logging file
Takes data from queue and sends along serial connection
Recieves any logging information from the nrf52840 

"""


def ais_rec():
    log_dir = os.path.join(os.path.dirname(
        os.path.abspath(__file__)), os.pardir, 'output_log')
    log_file = os.path.join(log_dir, 'log.txt')
    with open(log_file, 'w') as f:
        f.write('')

    while True:
        try:
            data = q.get()  # Get data from the queue
            # Convert all items to strings
            new_data = {key: str(val) for key, val in data.items()}
            json_data_str = json.dumps(new_data)  # Convert data to JSON format
            send_str = "{json_data}$".format(json_data=json_data_str)
            ser.write(send_str.encode())  # Send JSON data over serial
            print(f"Sent data : {send_str}")

            # Read data from the serial connection
            data_from_nrf = ser.read_all()
            if data_from_nrf:
                # Append the received data to the log file
                with open(log_file, 'ab') as f:
                    f.write(data_from_nrf)
                    # print(f"Received data from nrf52840 and appended to {log_file}")

        except (KeyboardInterrupt, SystemExit):
            # Close the serial connection if the script is interrupted
            ser.close()
            print("Closed")
            break
        except Exception as e:
            # Handle any other exceptions
            print(f"Error: {e}")
            time.sleep(SLEEP_DURATION/10)  # Sleep duration can be altered

        # Ensure ability to swap
        time.sleep(SLEEP_DURATION/10)


"""
def web_cloud_thread()
Queries data from web server
Puts dictionary onto the queue

"""


def web_cloud_thread():
    # get data from server
    while True:
        vessel_data = get_vessel_data(4)
        # put data on a queue
        pos = 1  # adding index for 4 vehicles
        for dictionary in vessel_data:
            dictionary["position"] = pos
            pos += 1
            q.put(dictionary)

        time.sleep(SLEEP_DURATION)


if __name__ == "__main__":
    json_thread = threading.Thread(target=ais_rec)
    web_thread = threading.Thread(target=web_cloud_thread)

    json_thread.start()
    web_thread.start()
