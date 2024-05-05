from typing import Optional
import requests
import re

POST_SHIP_DATA_URL = "https://cameronm.tech/add_ship_data.php"
NO_MMSI_GIVEN = 5
COULD_NOT_CONNECT_TO_SERVER = 6

def post_ship_data(mmsi: int, vessel_name: Optional[str], call_sign: Optional[str],
                    latitude: Optional[float], longitude: Optional[float],
                      speed: Optional[float], heading: Optional[float]) -> int:
    """
    function to post ship data that is received from the marine vessel SDR.

    Args:
    mmsi: int, (Required to idenntify vessel)
    vessel_name: The name of the marine vessel (Optional), 
    call_sign: The vessels call sign (Optional),
    latitude: The last known latitude of the ship (Optional), 
    longitude: The last known longitude of the ship (Optional),
    speed: The last known speed of the ship (Optional), 
    heading: The last known heading of the ship (Optional)

    Returns:
        0 - On a success
        1 - On fail
    """
    
    post_data = {}

    if mmsi and isinstance(mmsi, int):
        post_data["mmsi"] = mmsi
    else:
        return NO_MMSI_GIVEN

    if vessel_name is not None:
        post_data["vessel_name"] = vessel_name

    if call_sign is not None:
        post_data["call_sign"] = call_sign
    
    if latitude is not None:
        post_data["latitude"] = latitude
    
    if longitude is not None:
        post_data["longitude"] = longitude
    
    if speed is not None:
        post_data["speed"] = speed
    
    if heading is not None:
        post_data["heading"] = heading
    
    post_ship_data_request = requests.post(POST_SHIP_DATA_URL, json = post_data)
    
    if not post_ship_data_request.ok:
        print(f"BAD RESPONSE {post_ship_data_request.text} CODE: {post_ship_data_request.status_code}")
        return COULD_NOT_CONNECT_TO_SERVER
    
    #Ensure you have received a single digit back in response to 
    if bool(re.match(r'^\d$', post_ship_data_request.text)):
        if int(post_ship_data_request.text) != 0:
            return int(post_ship_data_request.text)     #This is the servers error code for debugging.
    
    return 0


#Example usage DELETE ME
if post_ship_data(696969696, "MDS FAST RACER", "MDS69", 100, 40,
                      0.1, None):
    print("Failed to post data")

else:
    print("Added Ship data")