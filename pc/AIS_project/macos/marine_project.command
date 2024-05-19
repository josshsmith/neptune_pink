#!/bin/bash

# Check if Python 3 is installed
if command -v python3 >/dev/null 2>&1; then
    echo "Python 3 is already installed."
else
    echo "Python 3 is not installed. Downloading and installing Python..."
    # this will install python3.10
    curl -O https://www.python.org/ftp/python/3.10.0/python-3.10.0-amd64.exe
    chmod +x python-3.10.0-amd64.exe
    ./python-3.10.0-amd64.exe
    echo "Python installation completed."
fi

# Get the directory of the current script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Install required packages
pip install pyserial requests

# Run the Python script
python "$SCRIPT_DIR/../code/marine_project.py"