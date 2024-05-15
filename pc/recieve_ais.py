"""
Script to recieve ais data through json format.

There will be two threads runnning:
One recieving json formatted information via uart
The other will be using a cloud based service to upload the ais information
"""
import serial
import json
import numpy
import time
import threading
import queue
from vessel_data_client import get_vessel_data

PORT = '/dev/tty.usbmodem0010502338821'
BAUD_RATE = 115200
SLEEP_DURATION = 0.5

# this will need to be replaced depending what machine is being used
ser = serial.Serial(PORT, BAUD_RATE)
# queue for passing information between threads
q = queue.Queue()


def ais_rec():
    while True:
        try:
            data = q.get()  # Get data from the queue
            new_data = {key: str(val) for key, val in data.items()}  # Convert all items to strings
            json_data_str = json.dumps(new_data)  # Convert data to JSON format
            send_str = "json_send \'{json_data}\'\n".format(json_data = json_data_str)
            ser.write(send_str.encode())  # Send JSON data over serial
            print(send_str)
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

def web_cloud_thread():
    # get data from server
    while True:
        vessel_data = get_vessel_data(5)
        # put data on a queue
        for dictionary in vessel_data:
            q.put(dictionary)
            print("Updated queue")
        
        time.sleep(SLEEP_DURATION)

if __name__ == "__main__":
    json_thread = threading.Thread(target=ais_rec)
    web_thread = threading.Thread(target=web_cloud_thread)

    json_thread.start()
    web_thread.start()
