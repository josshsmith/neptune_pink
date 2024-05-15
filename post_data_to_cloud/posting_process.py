# process.py
import sys
from post_to_server import post_ship_data


def convert_string_to_dict(input: str):

    #result = eval(input)
    print(f"Convert to json: {input}")
    return {}

def main():
    for line in sys.stdin:
        line = line.strip()
        print(line)
        if '{' in line and '}' in line:
            parsed_dict = convert_string_to_dict(line)
            print(f"Have the following data for the db: {parsed_dict}")
            #post_ship_data(parsed_dict.get("mmsi"), None, None, parsed_dict.get("lat"), parsed_dict.get("lon"), parsed_dict.get("speed"),
            #               parsed_dict.get("course"))
            
if __name__ == "__main__":
    main()
