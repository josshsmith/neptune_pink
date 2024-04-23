# neptune_pink

Cobolt 4 - Marine Vehicle AIS Transponder

## CSSE4011 Project  
Team Members:  
1. Marcus Von Rausch
2. Cameron Miller
3. Connor Davis
4. Joshua Smith

## Tasks
dwa

## KPIs
dwa

## Requirements
- At least one sensor -> Radio Receiver
- At least one actuaor -> NOT PRESENT IN OUR THING, MAY NEED VGA MONITOR
- Non-trivial wireless networking, at least 3 nodes -> Base (Raspberry Pi), NRF52840, Display (PC/M5)
    - Base will receive radio waves using sensor, Lime SDR Mini is currently desired 
    - nrf52840 will processed info from base using uart 
    - Display will receive info from nrf52940 via bluetooth
- Non-trivial techniques from Lectures 1-9
    - Kalman Filter?
    - Blockchain?
- PC/Mobile App of some kind -> Will be done on the display
- Maintain a group wiki -> This github will need to have a wiki pages as seen in project overview tasks.

### Project Overview Tasks for Group Wiki
- Write a project description
- Need 5 KPIs -> As seen above
- Write a system overview -> Hardware Architecture, Top Level Flowchar
- Snesor Integration - What sensors, what data, etc.
- Wireless Network, IoT Protocols, or Web Dashboards
- Algorithm Schemes Used
- DIKW Pyramid Abstraction
- Daily Prohect updates
