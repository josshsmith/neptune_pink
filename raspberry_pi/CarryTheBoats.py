import sys 
import json
import requests

def main():
    headers = {'Content-Type': 'application/json'}
    url = 'https://cameronm.tech/add_ship_data.php'
    echo_bool = 1
    sys.stdout = open('BoatyOutput.log','a')

    while True:
        line = sys.stdin.readline().strip()

        if not line:
            continue

        try: 
            json_data = json.loads(line)
            print("-------------------")
            print("Successfully parsed JSON. Sending to server.")
            print("-------------------", flush=True)
            echo_bool = 1
        except json.JSONDecodeError as e:
            if echo_bool:
                print("AIS-catcher echo:\n-------------------\n", line, flush=True)
                echo_bool = 0
            else: 
                print(line, flush=True)
            continue

        response = requests.post(url, json=json_data, headers=headers)

        if response.status_code == 200:
            print("Data sent successfully.", flush=True)
        else:
            print("Failed to send data. Status code:, response.status_code", flush=True)

            
if __name__ == "__main__":
    main()
