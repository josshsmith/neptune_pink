import requests
from config import Config

def get_vessel_data(num_vessels: int = 5) -> list[dict]:
    """
    Get the vessel information for at most 
        
    Args:
        num_vessels: the maximum number of most recently added marine vessels to get information for.
    Returns:
        dict: A json like dictionary containing each row of the queried database table. 
    """
    if num_vessels < 0 or num_vessels > Config.MAX_VESSELS_TO_GET:
        num_vessels = 5 

    try:
        ship_data_respone = requests.get(Config.SHIP_DATA_URL, params = {"num_ships": str(num_vessels)})    
    except Exception as e:
        print(f"Server Request Error: {e}")
        return []

    if ship_data_respone.ok:
        #Server is working correctly. Probably a status 200. 
        ship_data = ship_data_respone.json()
        return ship_data
    
    else:
        #got a bad response code from the server. 
        print(f"Server NOT operating properly. SERVER STATUS: {ship_data_respone.status_code}")
        return []

    

print(get_vessel_data(1))
