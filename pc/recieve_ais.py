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

PORT = '/dev/tty.usbmodem0010502338821'
BAUD_RATE = 115200
SLEEP_DURATION = 0.001

# this will need to be replaced depending what machine is being used
ser = serial.Serial(PORT, BAUD_RATE)
# queue for passing information between threads
q = queue.Queue()


def ais_rec():
    while True:
        try:
            data = q.get()  # Get data from the queue
            json_data = json.dumps(data)  # Convert data to JSON format
            ser.write(json_data.encode())  # Send JSON data over serial
        except (KeyboardInterrupt, SystemExit):
            # Close the serial connection if the script is interrupted
            ser.close()
            print("Closed")
            break
        except Exception as e:
            # Handle any other exceptions
            print(f"Error: {e}")
            time.sleep(SLEEP_DURATION)  # Sleep duration can be altered


def web_cloud_thread():
    # get data from server
    # put data on a queue
    pass


if __name__ == "__main__":
    json_thread = threading.Thread(target=ais_rec)
    web_thread = threading.Thread(target=web_cloud_thread)

    json_thread.start()
    web_thread.start()
