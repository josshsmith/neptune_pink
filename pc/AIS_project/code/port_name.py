import serial
import glob
import platform


def get_os():
    os_name = platform.system().lower()
    if os_name == "windows":
        return "windows"
    elif os_name == "darwin":
        return "macos"
    else:
        return "linux"


def get_port_name():

    current_os = get_os()
    ports = ""
    if current_os == "macos":
        ports = ports = glob.glob('/dev/tty.usb*')
    elif current_os == "windows":
        ports = glob.glob('COM*')
    else:
        print("ERROR: AIS Transponder Project is only compatible with Macos or Windows")
        exit(1)
    for port in ports:
        try:
            # Try to open the serial port
            ser = serial.Serial(port, 115200, timeout=1)
            ser.close()
            return port
        except (OSError, serial.SerialException):
            # If the port is not available, move to the next one
            pass

    # If no available port is found, return None
    print("ERROR: Couldn't find port to connect to, please check nrf52840dk is connected")
