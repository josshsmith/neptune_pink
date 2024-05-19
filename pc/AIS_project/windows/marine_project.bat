@echo off

REM Check if Python is installed
python --version 2>NUL
if errorlevel 1 (
    REM Python is not installed, so let's download and install it
    echo Python is not installed. Downloading and installing Python...
    start /wait "" https://www.python.org/ftp/python/3.10.0/python-3.10.0-amd64.exe
    REM Modify the URL above to match the desired Python version and architecture
    echo Python installation completed.
) else (
    REM Python is already installed
    echo Python is already installed.
)

:: Get the directory of the current script
set SCRIPT_DIR=%~dp0

:: Install required packages
pip install pyserial requests

:: Run the Python script
python "%SCRIPT_DIR%..\code\marine_project.py"