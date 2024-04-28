import requests
import json

# Replace with your server URL
SERVER_URL = "NOT_SURE_YET"

# Convert data to JSON format
json_data = json.dumps(data)

try:
    # Send POST request to the server
    response = requests.post(SERVER_URL, data=json_data, headers={
                             'Content-Type': 'application/json'})

    # Check if the request was successful
    if response.status_code == 200:
        print("Data sent successfully!")
    else:
        print(f"Error sending data: {response.text}")

except requests.exceptions.RequestException as e:
    print(f"Error sending data: {e}")
